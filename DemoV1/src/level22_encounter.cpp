#include "rloft/level22_encounter.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

std::uint32_t normalized_drop_angle(
    std::uint32_t screen_seed,
    std::uint32_t facing) noexcept {
    const auto delta =
        static_cast<std::int32_t>(screen_seed) -
        0xA0;
    const auto quotient = delta / 3;
    return ExplorationMotionModel::normalize_heading(
        static_cast<std::int32_t>(
            facing -
            static_cast<std::uint32_t>(quotient)));
}

void spawn_drop(
    LevelWorldState& world,
    const Level22Drop& drop,
    std::uint32_t base_x,
    std::uint32_t base_y,
    std::uint32_t facing,
    int map_width,
    const RendererTables& tables,
    const Level22EncounterModel::PlacementProbe& can_place) {
    const auto angle =
        normalized_drop_angle(
            drop.screen_seed,
            facing);
    const auto delta =
        ExplorationMotionModel::direction_vector(
            0x00018000U,
            angle,
            false,
            tables);

    auto x = base_x + delta.delta_x;
    auto y = base_y + delta.delta_y;

    if (can_place &&
        !can_place(y, x, 0x190U)) {
        x = base_x;
        y = base_y;
    }

    const auto high_y =
        static_cast<std::int16_t>(
            static_cast<std::uint16_t>(
                y >> 16U));
    const auto high_x =
        static_cast<std::int16_t>(
            static_cast<std::uint16_t>(
                x >> 16U));

    const auto cell =
        static_cast<std::int32_t>(map_width) *
            static_cast<std::int32_t>(high_y) +
        static_cast<std::int32_t>(high_x);
    if (cell < 0 || cell > 0xFFFF) {
        throw std::runtime_error(
            "Assassin drop cell exceeds live record range");
    }

    if (world.loose_items.size() == 0x12CU) {
        // CODE.2 2195C discards the oldest record at the 300-record ceiling.
        world.loose_items.erase(
            world.loose_items.begin());
    }

    LiveObjectRecord item{};
    item.y_position_16_16 = y;
    item.x_position_16_16 = x;
    item.cached_cell_index =
        static_cast<std::uint16_t>(cell);
    item.object_type_id =
        drop.object_id;

    // 2195C preserves unrelated packed bits from the reused slot, then
    // clears 0x05F8 and writes known-state bit 10 from flag_source&1.
    // Assassin death passes flag_source=0. A newly appended native record has
    // no stale-slot residue, so all retained bits begin zero.
    item.packed_runtime_state = 0;

    world.loose_items.push_back(item);
}

}  // namespace

NpcDamageResult Level22EncounterModel::damage_assassin(
    LevelWorldState& world,
    std::size_t npc_index,
    std::uint16_t amount,
    std::uint16_t reaction_mode,
    std::uint32_t party_facing,
    const NpcCatalog& npcs,
    const RendererTables& tables,
    const PlacementProbe& can_place) {
    if (npc_index >= world.npcs.size()) {
        throw std::out_of_range(
            "Level 22 Assassin index");
    }

    auto& live = world.npcs[npc_index];
    if (live.npc_type_id !=
        Level22Story::kAssassinNpcType) {
        throw std::logic_error(
            "Level 22 damage target is not the Assassin");
    }

    const auto& def =
        npcs.at(live.npc_type_id);

    NpcDamageResult result{};
    result.damaged = true;

    if (def.damage_reaction_animation_flag != 0U) {
        live.packed_runtime_state =
            static_cast<std::uint16_t>(
                live.packed_runtime_state | 0x0006U);
    }
    live.action_state_countdown =
        reaction_mode == 0U ? 2U : 1U;

    live.current_hp =
        static_cast<std::int16_t>(
            static_cast<std::uint16_t>(
                live.current_hp) -
            amount);

    if (live.current_hp > 0) {
        result.sound_kind =
            NpcSoundKind::DamageReaction;
        result.sound_id =
            def.damage_sound_id;
        return result;
    }

    result.died = true;
    result.sound_kind = NpcSoundKind::Death;
    result.sound_id = def.death_sound_id;
    result.experience_award =
        def.experience_award;

    for (const auto& drop :
         Level22Story::assassin_death_drops()) {
        spawn_drop(
            world,
            drop,
            live.x_position_16_16,
            live.y_position_16_16,
            party_facing,
            30,
            tables,
            can_place);
        ++result.spawned_loose_items;
    }

    // 0E284 marks byte +0B bit 1 after the special respawn scan fails. Level
    // 22 has no Assassin respawn entry, so this is runtime_flags bit 25.
    live.runtime_flags |= 0x02000000U;
    return result;
}

LooseItemPickupResult
Level22EncounterModel::pickup_loose_item(
    LevelWorldState& world,
    std::size_t item_index,
    InventoryRuntimeState& inventory,
    PartyImage& party,
    const ObjectCatalog& objects) {
    if (item_index >=
        world.loose_items.size()) {
        throw std::out_of_range(
            "loose-item pickup index");
    }

    const auto item =
        world.loose_items[item_index];

    LooseItemPickupResult result{};
    result.object_id =
        static_cast<std::int16_t>(
            item.object_type_id);
    result.known =
        static_cast<std::uint8_t>(
            (item.packed_runtime_state >>
             10U) & 1U);

    // CODE.2 21704 calls 1A344 before compacting family C.
    inventory.held.object_id =
        result.object_id;
    inventory.held.known =
        result.known;

    world.loose_items.erase(
        world.loose_items.begin() +
        static_cast<std::ptrdiff_t>(
            item_index));

    result.level22_mist_sequence =
        Level22Story::pickup_completes_set(
            item.object_type_id,
            [&](std::uint16_t wanted) {
                return inventory.has_object(
                    party,
                    objects,
                    static_cast<std::int16_t>(
                        wanted));
            });

    return result;
}

}  // namespace rloft
