#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/inventory_save_codec.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

void put16(std::vector<std::uint8_t>& dst,
           std::size_t at,std::uint16_t value) {
    dst[at]=static_cast<std::uint8_t>(value);
    dst[at+1U]=static_cast<std::uint8_t>(value>>8U);
}
void put32(std::vector<std::uint8_t>& dst,
           std::size_t at,std::uint32_t value) {
    for(unsigned i=0;i<4U;++i)
        dst[at+i]=static_cast<std::uint8_t>(value>>(i*8U));
}
std::uint16_t get16(const std::vector<std::uint8_t>& b,
                    std::size_t at) {
    return static_cast<std::uint16_t>(b[at]) |
        (static_cast<std::uint16_t>(b[at+1U])<<8U);
}

// Descriptor slot-count bytes copied from original CODE.2 object-2
// DGROUP:7330, at physical 0x7D254 + 10*descriptor.
// State 62 is zero-initialized, NOT persisted by original SAVE.
constexpr std::array<std::uint8_t,63> kOriginalSlotCounts{{
    4,8,8,8,6,6,8,6,8,8,4,8,8,6,8,4,8,4,8,8,8,
    8,8,8,6,8,8,4,6,8,8,8,8,8,8,8,8,8,4,8,8,8,
    4,8,8,6,6,8,8,8,4,8,8,8,6,8,4,6,8,8,6,8,0
}};

rloft::InventoryRuntimeState original_template() {
    rloft::InventoryRuntimeState state{};
    for(std::size_t i=0;i<63U;++i) {
        state.containers[i].slot_count=kOriginalSlotCounts[i];
        state.containers[i].items.assign(kOriginalSlotCounts[i],-1);
        state.containers[i].subtype=static_cast<std::uint8_t>(i%7U);
    }
    // This test sentinel must survive import but never be written
    // into the 62-record original on-disk format.
    state.containers[62].per_slot_state_bits=0xD3U;
    return state;
}

std::vector<std::uint8_t> source_fixture(std::uint32_t effects) {
    using C=rloft::InventorySaveCodec;
    std::vector<std::uint8_t> b(
        C::kZeroTransientSaveBytes+
            static_cast<std::size_t>(effects)*C::kTransientRecordBytes);
    for(std::size_t i=0;i<b.size();++i)
        b[i]=static_cast<std::uint8_t>((i*37U+19U)&0xFFU);
    put32(b,C::kTransientCountAt,effects);
    put16(b,0x74U,0xFFFFU);
    b[0x76U]=0U;
    const auto bank=0x9A7U+
        static_cast<std::size_t>(effects)*0x16U;
    std::size_t at=bank;
    for(std::size_t i=0;i<62U;++i) {
        for(std::size_t s=0;s<kOriginalSlotCounts[i];++s) {
            put16(b,at,s==0U
                ? static_cast<std::uint16_t>(i==61U?57U:i)
                : 0xFFFFU);
            at+=2U;
        }
        b[at++]=static_cast<std::uint8_t>(i&0xFFU);
    }
    assert(at==bank+0x3AAU);
    return b;
}

template<class Action>
bool fails(Action&& action) {
    try { action(); }
    catch(const std::runtime_error&) { return true; }
    return false;
}

void round_trip(std::uint32_t effects) {
    using C=rloft::InventorySaveCodec;
    const auto original=source_fixture(effects);
    const auto template_state=original_template();
    auto saved=C::decode(original,template_state);

    assert(saved.preserved_save_bytes==original);
    assert(saved.transient_effect_count==effects);
    assert(saved.container_bank_offset==
        0x9A7U+static_cast<std::size_t>(effects)*0x16U);
    assert(saved.inventory.held.object_id==-1);
    assert(saved.inventory.held.known==0U);
    assert(saved.inventory.containers[0].items[0]==0);
    assert(saved.inventory.containers[61].items[0]==57);
    assert(saved.inventory.containers[61].per_slot_state_bits==61U);
    assert(saved.inventory.containers[62].per_slot_state_bits==0xD3U);
    assert(saved.inventory.containers[62].slot_count==0U);
    assert(!saved.inventory.open_container.has_value());
    assert(!saved.inventory.mage_scribe_feedback.has_value());

    // Unchanged decode -> encode must reproduce EVERY source
    // byte, including packed effects, reserved actor regions,
    // charged-item counters, all 10 roster records, phase bytes.
    assert(C::encode(saved)==original);

    // Change known flags, Mage spellbook low nibble, held cursor,
    // four original fragment story bits, a nested container, and
    // the late quiver-shaped source descriptor 61.
    saved.party.bytes[0U*0x248U+0x6AU]=0U;
    saved.party.bytes[2U*0x248U+0x12BU]=0x05U;
    saved.persistent_event_flags[0U]=0x0FU;
    saved.persistent_event_flags[0xBAU/8U] |= 0x04U;
    saved.inventory.held={201,1U};
    saved.inventory.containers[0].items[1]=0x70;
    saved.inventory.containers[0].per_slot_state_bits=0x02U;
    saved.inventory.containers[61].items[0]=-1;
    saved.inventory.containers[61].per_slot_state_bits=0x80U;
    saved.inventory.containers[62].per_slot_state_bits=0x22U;
    saved.container_ui_contexts[1]={{1U,2U,3U,4U}};

    const auto encoded=C::encode(saved);
    assert(encoded.size()==original.size());
    assert(encoded[0x32U]==0x0FU);
    assert(encoded[0x32U+0xBAU/8U]==
        static_cast<std::uint8_t>(
            original[0x32U+0xBAU/8U]|0x04U));
    assert(encoded[0x77U+0x6AU]==0U);
    assert(encoded[0x77U+2U*0x248U+0x12BU]==0x05U);
    assert(get16(encoded,0x74U)==201U);
    assert(encoded[0x76U]==1U);
    const auto bank=saved.container_bank_offset;
    assert(get16(encoded,bank+2U)==0x70U);
    assert(encoded[bank+8U]==0x02U);
    const auto quiver=bank+0x3AAU-17U;
    assert(get16(encoded,quiver)==0xFFFFU);
    assert(encoded[quiver+16U]==0x80U);
    for(std::size_t i=0;i<4U;++i)
        assert(encoded[encoded.size()-16U+4U+i]==i+1U);

    // Source bytes not authored by the inventory codec must
    // not change, even at nonzero counted transient effects.
    for(std::size_t i=0;i<original.size();++i) {
        const bool owned=
            (i>=0x32U && i<0x52U) ||
            (i>=0x74U && i<=0x76U) ||
            (i>=0x77U && i<0x77U+C::kPartyBytes) ||
            (i>=bank && i<bank+C::kPersistentContainerBytes) ||
            (i>=original.size()-16U);
        if(!owned)assert(encoded[i]==original[i]);
    }

    // Decode the modified original bytes a second time; metadata
    // remains CODE.2-sourced and authoring changes survive.
    const auto restored=C::decode(encoded,template_state);
    assert(restored.party.bytes==saved.party.bytes);
    assert(restored.persistent_event_flags==
           saved.persistent_event_flags);
    assert(restored.inventory.held.object_id==201);
    assert(restored.inventory.held.known==1U);
    assert(restored.inventory.containers[0].items[1]==0x70);
    assert(restored.inventory.containers[61].items[0]==-1);
    assert(restored.inventory.containers[62].per_slot_state_bits==
           0xD3U); // unpersisted descriptor 62 stays on CODE.2
    assert(restored.container_ui_contexts==
           saved.container_ui_contexts);
    assert(C::encode(restored)==encoded);
}

} // namespace

int main() {
    // Original quick-start SAVE: 0x2448, no transient records.
    round_trip(0U);
    // Count moves ALL middle/trailing sections by 0x16*N.
    round_trip(3U);
    round_trip(rloft::InventorySaveCodec::kMaxTransientCount);

    using C=rloft::InventorySaveCodec;
    const auto baseline=original_template();
    auto valid=source_fixture(0U);

    auto too_short=valid;
    too_short.pop_back();
    assert(fails([&]{
        (void)C::decode(too_short,baseline);
    }));
    auto invalid_count=valid;
    put32(invalid_count,C::kTransientCountAt,0x97U);
    assert(fails([&]{
        (void)C::decode(invalid_count,baseline);
    }));
    auto malformed=baseline;
    malformed.containers[61].items.pop_back();
    assert(fails([&]{
        (void)C::decode(valid,malformed);
    }));
    auto nonoriginal=baseline;
    nonoriginal.containers[1].slot_count=9U;
    nonoriginal.containers[1].items.push_back(-1);
    assert(fails([&]{
        (void)C::decode(valid,nonoriginal);
    }));
    auto loaded=C::decode(valid,baseline);
    loaded.transient_effect_count=1U;
    assert(fails([&]{
        (void)C::encode(loaded);
    }));
    loaded.transient_effect_count=0U;
    loaded.inventory.containers[0].items.clear();
    assert(fails([&]{
        (void)C::encode(loaded);
    }));
}
