#include "rloft/level22_encounter.hpp"
#include "rloft/combat_npc_death_followon.hpp"

#ifdef NDEBUG
#undef NDEBUG // Keep assertions active in Release CTest.
#endif

#include <cassert>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <stdexcept>

namespace {

void put16(
    std::vector<std::uint8_t>& b,
    std::size_t o,
    std::uint16_t v) {
    b[o] = static_cast<std::uint8_t>(v);
    b[o + 1U] =
        static_cast<std::uint8_t>(v >> 8U);
}
void put32(
    std::vector<std::uint8_t>& b,
    std::size_t o,
    std::uint32_t v) {
    for (int i = 0; i < 4; ++i) {
        b[o + static_cast<std::size_t>(i)] =
            static_cast<std::uint8_t>(
                v >> (i * 8));
    }
}
void set_item(
    rloft::PartyImage& party,
    std::size_t slot,
    std::int16_t value) {
    const auto at =
        0x40U + slot * 2U;
    const auto raw =
        static_cast<std::uint16_t>(value);
    party.bytes[at] =
        static_cast<std::uint8_t>(raw);
    party.bytes[at + 1U] =
        static_cast<std::uint8_t>(
            raw >> 8U);
}

rloft::NpcCatalog npc_catalog() {
    std::vector<std::uint8_t> bytes(
        4U + 10U * 0x45U, 0);
    put32(bytes, 0, 10);
    const auto r =
        4U + 9U * 0x45U;
    bytes[r] = 'A';
    bytes[r + 0x1C] = 1;
    bytes[r + 0x21] = 5;
    bytes[r + 0x22] = 6;
    put16(bytes, r + 0x23, 200);
    put16(bytes, r + 0x41, 90);
    put16(bytes, r + 0x43, 22);
    return rloft::NpcCatalog::parse(
        bytes);
}

rloft::ObjectCatalog object_catalog() {
    std::vector<std::uint8_t> bytes(
        4U + 0xEAU * 0x41U, 0);
    put32(bytes, 0, 0xEA);
    return rloft::ObjectCatalog::parse(
        bytes);
}

}  // namespace

int main() {
    // Ensure the newly source-driven E284 four-drop plan and the
    // previously reconstructed Level22 quest/pickup definitions
    // continue to agree in both object IDs AND screen seeds.
    const auto original_death =
        rloft::CombatNpcDeathFollowOnModel::inspect(
            9U,0,90U,22U,200U,0U,0U);
    assert(original_death.drop_count==4U);
    assert(original_death.set_story_dword_bf44);
    for(std::size_t i=0U;i<4U;++i) {
        const auto legacy_drop=
            rloft::Level22Story::assassin_death_drops()[i];
        assert(original_death.drops[i].object_id==
               legacy_drop.object_id);
        assert(original_death.drops[i].screen_seed==
               legacy_drop.screen_seed);
    }

    rloft::RendererTables tables{};
    // Synthetic Q16 unit factors sufficient for heading 0 test.
    tables.sine_q16_signmag.fill(0);
    tables.sine_q16_signmag[0] = 0x00010000U;
    tables.sine_q16_signmag[90] = 0;

    rloft::LevelWorldState world{};
    rloft::LiveNpcRecord assassin{};
    assassin.npc_type_id = 9;
    assassin.x_position_16_16 =
        0x000D8000U;
    assassin.y_position_16_16 =
        0x00078000U;
    assassin.current_hp = 5;
    assassin.packed_runtime_state = 0x8000U; // preserve unrelated original packed state
    assassin.runtime_flags = 0xE0000001U; // pre-existing byte+0B flags
    world.npcs.push_back(assassin);

    // 0x258AA is not an uninitialized flag: it is the three SIGNED
    // NPC filters copied from original level header +0x292..0x297.
    rloft::LevelState level{};
    level.width=30;
    level.height=15;
    for(std::size_t i=0;i<3U;++i) {
        level.header[0x292U+i*2U]=0xFFU;
        level.header[0x293U+i*2U]=0xFFU;
    }
    assert((level.respawn_npc_types()==
        std::array<std::int16_t,3>{{-1,-1,-1}}));

    // A modified header that tracks type9 MUST NOT silently skip the
    // original RNG-consuming E0F8 call and mutate the level anyway.
    auto unsupported=level;
    unsupported.header[0x292U]=9U;
    unsupported.header[0x293U]=0U;
    assert(unsupported.respawn_npc_types()[0]==9);
    bool correctly_rejected=false;
    try {
        auto probe_world=world;
        (void)rloft::Level22EncounterModel::damage_assassin(
            probe_world,unsupported,0U,5U,0U,0U,
            npc_catalog(),tables,{});
    } catch(const std::logic_error&) {
        correctly_rejected=true;
    }
    assert(correctly_rejected);
    assert(world.npcs[0].current_hp==5);
    // A signed -1 tracker must also NOT match type 0xFFFF.
    level.header[0x294U]=0U;
    level.header[0x295U]=0U;
    assert(level.respawn_npc_types()[1]==0);
    level.header[0x294U]=0xFFU;
    level.header[0x295U]=0xFFU;

    // Nonlethal damage uses the reaction sound and reaction animation/count.
    auto damage =
        rloft::Level22EncounterModel::damage_assassin(
            world,
            level,
            0,
            2,
            0,
            0,
            npc_catalog(),
            tables,
            [](std::uint32_t,
               std::uint32_t,
               std::uint32_t) {
                return false;
            });
    assert(damage.damaged);
    assert(!damage.died);
    assert(damage.sound_kind ==
           rloft::NpcSoundKind::DamageReaction);
    assert(damage.sound_id == 90);
    assert(world.npcs[0].current_hp == 3);
    assert(world.npcs[0].action_state_countdown == 2);
    assert((world.npcs[0].packed_runtime_state & 6U) == 6U);
    assert((world.npcs[0].packed_runtime_state & 0x8000U) != 0U);
    assert(world.npcs[0].runtime_flags == 0xD0000001U);

    // Lethal damage spawns all four story items; failed placement probes make
    // each fall back to the corpse coordinates.
    damage =
        rloft::Level22EncounterModel::damage_assassin(
            world,
            level,
            0,
            3,
            1,
            0,
            npc_catalog(),
            tables,
            [](std::uint32_t,
               std::uint32_t,
               std::uint32_t radius) {
                assert(radius == 0x190U);
                return false;
            });
    assert(damage.died);
    assert(world.npcs[0].action_state_countdown == 1U);
    assert((world.npcs[0].packed_runtime_state & 0x8000U) != 0U);
    assert(damage.sound_kind ==
           rloft::NpcSoundKind::Death);
    assert(damage.sound_id == 22);
    assert(damage.experience_award == 200);
    assert(damage.spawned_loose_items == 4);
    assert((world.npcs[0].runtime_flags & 0x02000000U) != 0U);
    assert(world.npcs[0].runtime_flags == 0xD2000001U);
    assert(world.loose_items.size() == 4);
    assert(world.loose_items[0].object_type_id == 0x88);
    assert(world.loose_items[1].object_type_id == 0x87);
    assert(world.loose_items[2].object_type_id == 0xE9);
    assert(world.loose_items[3].object_type_id == 0x74);
    for (const auto& item : world.loose_items) {
        assert(item.x_position_16_16 == 0x000D8000U);
        assert(item.y_position_16_16 == 0x00078000U);
        assert(item.packed_runtime_state == 0);
    }

    auto objects = object_catalog();
    rloft::InventoryRuntimeState inventory{};
    rloft::PartyImage party{};
    party.bytes.fill(0xFF);
    party.bytes[0x10] = 0;
    party.bytes[0x11] = 0;

    // Put three quest objects in inventory. Picking up the fourth to the
    // cursor completes the exact CODE.2 possession predicate.
    set_item(party, 0, 0x87);
    set_item(party, 1, 0x88);
    set_item(party, 2, 0x74);

    std::size_t parchment = 0;
    while (world.loose_items[parchment].object_type_id != 0xE9) {
        ++parchment;
    }

    const auto pickup =
        rloft::Level22EncounterModel::pickup_loose_item(
            world,
            parchment,
            inventory,
            party,
            objects);
    assert(pickup.object_id == 0xE9);
    assert(pickup.known == 0);
    assert(pickup.level22_mist_sequence);
    assert(inventory.held.object_id == 0xE9);

    // New end-to-end native interaction slice: 21704 takes one of
    // the Assassin's four dropped objects into the held cursor, and
    // 117A0 places it in the first free ordinary inventory slot.
    // The fourth pickup must still trigger 223F4's exact set
    // predicate with the first three already owned by the party.
    auto fresh_world = rloft::LevelWorldState{};
    // Re-create all four exact source object types in a separate
    // staged world; the earlier test already consumed the parchment.
    for (const auto drop : rloft::Level22Story::assassin_death_drops()) {
        rloft::LiveObjectRecord item{};
        item.object_type_id = drop.object_id;
        fresh_world.loose_items.push_back(item);
    }
    rloft::InventoryRuntimeState staged_inventory{};
    rloft::PartyImage staged_party{};
    staged_party.bytes.fill(0xFFU);
    staged_party.bytes[0x10U]=0U;
    staged_party.bytes[0x11U]=0U;
    for (std::size_t i=0U;i<4U;++i) {
        assert(staged_inventory.held.object_id==-1);
        const auto result =
            rloft::Level22EncounterModel::pickup_loose_item(
                fresh_world,0U,staged_inventory,staged_party,objects);
        assert(result.object_id ==
            static_cast<std::int16_t>(
                rloft::Level22Story::assassin_death_drops()[i].object_id));
        assert(result.level22_mist_sequence==(i==3U));
        if(i<3U) {
            assert(staged_inventory.swap_ordinary_slot(
                       staged_party,objects,0U,i)==
                   rloft::InventoryOrdinarySwapStatus::Swapped);
            assert(staged_inventory.held.object_id==-1);
        }
    }
    assert(fresh_world.loose_items.empty());
    assert(staged_inventory.held.object_id==0x74);
    for(const auto relic : rloft::Level22Story::quest_objects()) {
        assert(staged_inventory.has_object(
            staged_party,objects,
            static_cast<std::int16_t>(relic)));
    }

    // CODE.2 211D8 alternate real interaction: after each world
    // pickup the actor-panel flag08 path quick-stashes the held item
    // into the first free ordinary slot, without opening the modal.
    auto quick_world = rloft::LevelWorldState{};
    for(const auto drop:rloft::Level22Story::assassin_death_drops()) {
        rloft::LiveObjectRecord item{};
        item.object_type_id=drop.object_id;
        quick_world.loose_items.push_back(item);
    }
    rloft::InventoryRuntimeState quick_inventory{};
    rloft::PartyImage quick_party{};
    quick_party.bytes.fill(0xFFU);
    quick_party.bytes[0x10U]=0U;
    quick_party.bytes[0x11U]=0U;
    // This synthetic fresh actor has no initially identified items.
    // 211D8 itself must leave these four bytes unchanged on stash.
    for(std::size_t b=0U;b<4U;++b)
        quick_party.bytes[0x12BU+b]=0U;
    for(std::size_t i=0U;i<4U;++i) {
        assert(quick_inventory.held.object_id==-1);
        const auto got=rloft::Level22EncounterModel::pickup_loose_item(
            quick_world,0U,quick_inventory,quick_party,objects);
        assert(got.level22_mist_sequence==(i==3U));
        if(i<3U) {
            assert(quick_inventory.quick_stash_held(
                       quick_party,objects,0U)==
                   rloft::InventoryQuickStashStatus::Stashed);
            assert(quick_inventory.held.object_id==-1);
            assert(quick_party.bytes[0x12BU]==0U);
        }
    }
    assert(quick_world.loose_items.empty());
    assert(quick_inventory.held.object_id==0x74);
    for(const auto relic:rloft::Level22Story::quest_objects()) {
        assert(quick_inventory.has_object(
            quick_party,objects,
            static_cast<std::int16_t>(relic)));
    }
}
