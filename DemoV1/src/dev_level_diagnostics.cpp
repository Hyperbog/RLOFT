#include "rloft/dev_level_diagnostics.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace rloft {
namespace {

// Do not hash C++ structures or platform-native size_t memory:
// alignment, host endian order and padding would invalidate
// cross-run comparisons and hide source-byte regressions.
class StableFNV64 final {
public:
    void byte(std::uint8_t value) noexcept {
        value_ ^= value;
        value_ *= UINT64_C(1099511628211);
    }
    void word(std::uint16_t value) noexcept {
        byte(static_cast<std::uint8_t>(value));
        byte(static_cast<std::uint8_t>(value>>8U));
    }
    void dword(std::uint32_t value) noexcept {
        word(static_cast<std::uint16_t>(value));
        word(static_cast<std::uint16_t>(value>>16U));
    }
    void size(std::size_t value) noexcept {
        const auto x=static_cast<std::uint64_t>(value);
        dword(static_cast<std::uint32_t>(x));
        dword(static_cast<std::uint32_t>(x>>32U));
    }
    [[nodiscard]] std::uint64_t result() const noexcept {
        return value_;
    }

private:
    std::uint64_t value_{UINT64_C(14695981039346656037)};
};

void hash_live_object(
    StableFNV64& h,const LiveObjectRecord& item) noexcept {
    h.dword(item.y_position_16_16);
    h.dword(item.x_position_16_16);
    h.dword(item.runtime_flags);
    h.word(item.cached_cell_index);
    h.word(item.object_type_id);
    h.word(item.state_word);
    h.word(item.auxiliary_word);
    h.word(item.packed_runtime_state);
}

void hash_npc(
    StableFNV64& h,const LiveNpcRecord& npc) noexcept {
    h.dword(npc.y_position_16_16);
    h.dword(npc.x_position_16_16);
    h.dword(npc.runtime_flags);
    h.word(npc.cached_cell_index);
    h.word(npc.npc_type_id);
    h.word(npc.action_state_countdown);
    h.word(static_cast<std::uint16_t>(npc.current_hp));
    h.word(npc.packed_runtime_state);
}

} // namespace

std::string DevLevelSnapshot::digest8(std::uint64_t value) {
    constexpr char digits[]="0123456789ABCDEF";
    std::string result(8U,'0');
    // Eight hex digits are a small caption-friendly projection of
    // the full 64-bit per-domain diagnostics, not the full digest.
    for(std::size_t i=0U;i<result.size();++i) {
        result[result.size()-1U-i]=
            digits[static_cast<unsigned>(value&0xFU)];
        value>>=4U;
    }
    return result;
}

DevLevelSnapshot DevLevelDiagnostics::capture(
    const ExplorationSessionState& session,
    const LevelWorldState& world,
    const InventoryRuntimeState& inventory) noexcept {
    DevLevelSnapshot output{};
    output.level_id=session.level_id;
    output.coordinate_a=session.coordinate_a;
    output.coordinate_b=session.coordinate_b;
    output.facing=session.facing;
    output.time_of_day=session.time_of_day_16_16_hours;
    output.npc_count=world.npcs.size();
    output.placed_object_count=world.placed_objects.size();
    output.loose_item_count=world.loose_items.size();

    StableFNV64 camera{};
    camera.dword(session.level_id);
    camera.dword(session.coordinate_a);
    camera.dword(session.coordinate_b);
    camera.dword(session.facing);
    camera.dword(session.time_of_day_16_16_hours);
    output.camera_hash=camera.result();

    StableFNV64 party{};
    for(const auto value:session.party.bytes)party.byte(value);
    output.party_hash=party.result();
    for(std::size_t actor=0U;actor<PartyImage::kActorCount;++actor)
        if(session.party.active(actor))++output.active_actors;

    StableFNV64 story{};
    for(const auto value:session.persistent_event_flags) {
        story.byte(value);
        for(std::uint8_t mask=1U;mask!=0U;
            mask=static_cast<std::uint8_t>(mask<<1U))
            if((value&mask)!=0U)++output.set_story_flags;
    }
    output.flags_hash=story.result();

    StableFNV64 inventory_hash{};
    inventory_hash.word(
        static_cast<std::uint16_t>(inventory.held.object_id));
    inventory_hash.byte(inventory.held.known);
    inventory_hash.size(inventory.containers.size());
    for(const auto& container:inventory.containers) {
        inventory_hash.byte(container.slot_count);
        inventory_hash.byte(container.ui_columns);
        inventory_hash.byte(container.capacity_pounds);
        inventory_hash.byte(container.ui_x_offset);
        inventory_hash.byte(container.subtype);
        inventory_hash.byte(container.per_slot_state_bits);
        inventory_hash.size(container.items.size());
        for(const auto id:container.items)
            inventory_hash.word(static_cast<std::uint16_t>(id));
    }
    for(const auto& context:inventory.party_container_ui_contexts) {
        for(const auto value:context.raw)inventory_hash.byte(value);
        if(context.fully_open())++output.active_container_views;
    }
    inventory_hash.byte(inventory.open_container ? 1U : 0U);
    if(inventory.open_container) {
        inventory_hash.size(inventory.open_container->actor);
        inventory_hash.size(inventory.open_container->hand);
        inventory_hash.word(static_cast<std::uint16_t>(
            inventory.open_container->object_id));
        inventory_hash.size(
            inventory.open_container->descriptor_index);
    }
    output.inventory_hash=inventory_hash.result();

    StableFNV64 source_world{};
    source_world.dword(static_cast<std::uint32_t>(
        session.level.width));
    source_world.dword(static_cast<std::uint32_t>(
        session.level.height));
    source_world.size(session.level.cell_words.size());
    for(const auto word:session.level.cell_words)
        source_world.word(static_cast<std::uint16_t>(word));
    source_world.size(world.npcs.size());
    for(const auto& npc:world.npcs)hash_npc(source_world,npc);
    source_world.size(world.placed_objects.size());
    for(const auto& item:world.placed_objects)
        hash_live_object(source_world,item);
    source_world.size(world.loose_items.size());
    for(const auto& item:world.loose_items)
        hash_live_object(source_world,item);
    output.world_hash=source_world.result();
    return output;
}

DevLevelChange DevLevelDiagnostics::compare(
    const DevLevelSnapshot& before,
    const DevLevelSnapshot& after) noexcept {
    return {
        before.camera_hash!=after.camera_hash,
        before.party_hash!=after.party_hash,
        before.inventory_hash!=after.inventory_hash,
        before.world_hash!=after.world_hash,
        before.flags_hash!=after.flags_hash
    };
}

} // namespace rloft
