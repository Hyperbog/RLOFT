#include "rloft/level22_encounter.hpp"
#include "rloft/combat_npc_injury_prelude.hpp"
#include "rloft/combat_npc_death_followon.hpp"

#include <array>
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
    const LevelState& level,
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

    // Original level-header bytes +292/+294/+296 are the signed
    // 258AA NPC-type tracker entries read at E284:E50F. Both archive
    // level 22 and direct LEV022 have (-1,-1,-1), so type9 does NOT
    // call E0F8. Reject other/modified states *before any mutation*:
    // an E0F8 call would reseed RNG, possibly respawn, and shift the
    // following death/award timeline; we cannot treat it as false.
    if (level.width!=30 || level.height!=15) {
        throw std::logic_error("Level22 Assassin requires Level22 geometry");
    }
    const auto respawn_types=level.respawn_npc_types();
    for (const auto tracked:respawn_types) {
        if (static_cast<std::int32_t>(tracked)==
            static_cast<std::int32_t>(live.npc_type_id)) {
            throw std::logic_error(
                "E284 matched E0F8 respawn requires original IRQ/RNG state");
        }
    }

    const auto& def =
        npcs.at(live.npc_type_id);

    NpcDamageResult result{};
    result.damaged = true;

    // CODE.2 E284..E306 is now shared with the full
    // original NPC injury prelude. For the Level 22 Assassin
    // (NPC type 9), this preserves the existing HP deduction,
    // definition-controlled 06h reaction bits, and 2/1 countdown.
    // Type-specific death, sounds and four quest drops below are
    // still handled by the established Level 22 encounter path.
    const auto injury = CombatNpcInjuryPreludeModel::inspect(
        CombatNpcInjuryPreludeInputs{
            live.npc_type_id,
            live.current_hp,
            live.packed_runtime_state,
            def.damage_reaction_animation_flag,
            reaction_mode,
            static_cast<std::int32_t>(amount),
            live.runtime_flags
        });
    live.packed_runtime_state = injury.next_packed_state_14;
    live.action_state_countdown = injury.next_action_countdown_10;
    live.current_hp = injury.next_hp;
    // E306..E31F runs for nonlethal and lethal injury alike.
    live.runtime_flags = injury.next_runtime_flags;

    // Original E284:E31F..E577 first separates living damage
    // presentation from death-specific branches. For this
    // bounded Level22 type9 specialization the planner also
    // provides exact 3195C drop item/seed arguments.
    // Original 258BC/258C0 presentation resource bases are
    // managed by the outer runtime, not guessed here.
    const auto followon=CombatNpcDeathFollowOnModel::inspect(
        live.npc_type_id,
        live.current_hp,
        def.damage_sound_id,
        def.death_sound_id,
        def.experience_award,
        0U,0U);

    if (!followon.is_death) {
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

    // E3C9..E443 issues four 3195C calls with exact IDs and
    // screen seeds. Use the shared original death dispatcher,
    // not a second modern hardcoded four-item loop.
    if (followon.special!=
            CombatNpcDeathSpecialBranch::Type9FourQuestDrops ||
        !followon.set_story_dword_bf44 ||
        followon.drop_count!=4U) {
        throw std::logic_error(
            "CODE.2 E284 type9 death branch not verified");
    }
    for (std::size_t i=0U;i<followon.drop_count;++i) {
        const auto& original_drop=followon.drops[i];
        const Level22Drop drop{
            original_drop.screen_seed,
            original_drop.object_id
        };
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

    // E50F..E539: NONE of the actual level-header types match the
    // level22 Assassin. The original signed three-word scan therefore
    // consumes no RNG, invokes no E0F8 and marks the corpse. This is
    // no longer based on a guessed level-22 special case.
    constexpr std::array<bool,3U> no_calls{{false,false,false}};
    live.runtime_flags=CombatNpcDeathFollowOnModel::death_runtime_flags(
        live.npc_type_id,live.runtime_flags,respawn_types,no_calls);
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
