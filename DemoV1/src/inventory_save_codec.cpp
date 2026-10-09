#include "rloft/inventory_save_codec.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace rloft {
namespace {

std::uint16_t read16(
    const std::vector<std::uint8_t>& source,
    std::size_t at) noexcept {
    return static_cast<std::uint16_t>(source[at]) |
        (static_cast<std::uint16_t>(source[at+1U])<<8U);
}
std::uint32_t read32(
    const std::vector<std::uint8_t>& source,
    std::size_t at) noexcept {
    return static_cast<std::uint32_t>(source[at]) |
        (static_cast<std::uint32_t>(source[at+1U])<<8U) |
        (static_cast<std::uint32_t>(source[at+2U])<<16U) |
        (static_cast<std::uint32_t>(source[at+3U])<<24U);
}
std::int16_t signed16(std::uint16_t raw) noexcept {
    // Explicit x86 MOVSX rather than out-of-range uint16->int16
    // implementation-defined narrowing.
    return static_cast<std::int16_t>(
        raw<=0x7FFFU ? static_cast<std::int32_t>(raw) :
        static_cast<std::int32_t>(raw)-0x10000);
}
void write16(
    std::vector<std::uint8_t>& destination,
    std::size_t at,
    std::uint16_t value) noexcept {
    destination[at]=static_cast<std::uint8_t>(value);
    destination[at+1U]=static_cast<std::uint8_t>(value>>8U);
}

struct Layout final {
    std::uint32_t effects{};
    std::size_t container_offset{};
    std::size_t context_offset{};
};

Layout validate_layout(const std::vector<std::uint8_t>& bytes) {
    using Codec=InventorySaveCodec;
    constexpr std::size_t kCountAt=Codec::kTransientCountAt;
    if(bytes.size()<Codec::kZeroTransientSaveBytes)
        throw std::runtime_error("SAVE.DAT shorter than original fixed state");
    const auto effects=read32(bytes,kCountAt);
    if(effects>Codec::kMaxTransientCount)
        throw std::runtime_error("SAVE.DAT transient count exceeds 0x96");

    // CODE.2 serializes 4x0x248 party bytes, the counted
    // 0x16 records, then spell UI dwords[2] and A7E4,
    // immediately followed by 62 variable-width containers.
    const auto delta=static_cast<std::size_t>(effects)*
        Codec::kTransientRecordBytes;
    if(bytes.size()!=Codec::kZeroTransientSaveBytes+delta)
        throw std::runtime_error("SAVE.DAT count-derived size mismatch");

    const auto container_at=kCountAt+4U+delta+8U+4U;
    constexpr std::size_t kAfterContainers=
        8U+15U+10U*PartyImage::kActorSize+4U*4U;
    if(container_at+Codec::kPersistentContainerBytes+
           kAfterContainers!=bytes.size())
        throw std::runtime_error("SAVE.DAT container tail misaligned");

    return Layout{effects,container_at,bytes.size()-16U};
}

void validate_descriptors(
    const InventoryRuntimeState& state) {
    using Codec=InventorySaveCodec;
    std::size_t bytes=0U;
    for(std::size_t i=0U;
        i<InventoryRuntimeState::kSerializedContainerCount;++i) {
        const auto& desc=state.containers[i];
        // All original source descriptors 0..61 use 4,6 or 8
        // item words. Never serialize arbitrary host-width bags.
        if(desc.slot_count!=4U && desc.slot_count!=6U &&
           desc.slot_count!=8U)
            throw std::runtime_error(
                "CODE.2 SAVE container slot geometry invalid");
        if(desc.items.size()!=desc.slot_count)
            throw std::runtime_error(
                "SAVE container contents size differs from descriptor");
        bytes+=2U*desc.slot_count+1U;
    }
    if(bytes!=Codec::kPersistentContainerBytes)
        throw std::runtime_error(
            "CODE.2 SAVE descriptor total differs from 938 bytes");
    // State 62 is never in original SAVE: do not reinterpret or
    // mutate it as the serializable state 61.
}

} // namespace

InventorySaveSnapshot InventorySaveCodec::decode(
    const std::vector<std::uint8_t>& original_save,
    const InventoryRuntimeState& code2_template) {
    const auto layout=validate_layout(original_save);
    validate_descriptors(code2_template);

    InventorySaveSnapshot result{};
    result.preserved_save_bytes=original_save;
    result.container_bank_offset=layout.container_offset;
    result.transient_effect_count=layout.effects;
    result.inventory=code2_template;

    // Pending spellbook message and transient open-container view
    // are not SAVE globals and must not leak across a LOAD.
    result.inventory.mage_scribe_feedback.reset();
    result.inventory.open_container.reset();

    std::copy_n(original_save.begin()+0x32U,
        result.persistent_event_flags.size(),
        result.persistent_event_flags.begin());
    std::copy_n(original_save.begin()+kPartyStart,
        kPartyBytes,result.party.bytes.begin());

    result.inventory.held.object_id=
        signed16(read16(original_save,0x74U));
    result.inventory.held.known=original_save[0x76U];

    auto at=layout.container_offset;
    for(std::size_t i=0U;
        i<InventoryRuntimeState::kSerializedContainerCount;++i) {
        auto& container=result.inventory.containers[i];
        for(std::size_t slot=0U;slot<container.slot_count;++slot) {
            container.items[slot]=
                signed16(read16(original_save,at));
            at+=2U;
        }
        container.per_slot_state_bits=original_save[at++];
    }
    if(at!=layout.container_offset+kPersistentContainerBytes)
        throw std::runtime_error("SAVE container bank decode mismatch");

    for(std::size_t actor=0U;actor<4U;++actor)
        std::copy_n(original_save.begin()+
            layout.context_offset+actor*4U,4U,
            result.container_ui_contexts[actor].begin());
    return result;
}

std::vector<std::uint8_t> InventorySaveCodec::encode(
    const InventorySaveSnapshot& snapshot) {
    const auto layout=validate_layout(
        snapshot.preserved_save_bytes);
    validate_descriptors(snapshot.inventory);
    if(layout.container_offset!=snapshot.container_bank_offset ||
       layout.effects!=snapshot.transient_effect_count)
        throw std::runtime_error(
            "SAVE snapshot offsets differ from original backing image");

    auto result=snapshot.preserved_save_bytes;
    std::copy(snapshot.persistent_event_flags.begin(),
        snapshot.persistent_event_flags.end(),
        result.begin()+0x32U);
    std::copy(snapshot.party.bytes.begin(),
        snapshot.party.bytes.end(),
        result.begin()+kPartyStart);
    write16(result,0x74U,
        static_cast<std::uint16_t>(snapshot.inventory.held.object_id));
    result[0x76U]=snapshot.inventory.held.known;

    auto at=layout.container_offset;
    for(std::size_t i=0U;
        i<InventoryRuntimeState::kSerializedContainerCount;++i) {
        const auto& container=snapshot.inventory.containers[i];
        for(std::size_t slot=0U;slot<container.slot_count;++slot) {
            write16(result,at,static_cast<std::uint16_t>(
                container.items[slot]));
            at+=2U;
        }
        result[at++]=container.per_slot_state_bits;
    }
    if(at!=layout.container_offset+kPersistentContainerBytes)
        throw std::runtime_error(
            "SAVE container bank encode mismatch");

    for(std::size_t actor=0U;actor<4U;++actor)
        std::copy(
            snapshot.container_ui_contexts[actor].begin(),
            snapshot.container_ui_contexts[actor].end(),
            result.begin()+layout.context_offset+actor*4U);
    return result;
}

} // namespace rloft
