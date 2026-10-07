#include "rloft/level22_encounter.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

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
    world.npcs.push_back(assassin);

    // Nonlethal damage uses the reaction sound and reaction animation/count.
    auto damage =
        rloft::Level22EncounterModel::damage_assassin(
            world,
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

    // Lethal damage spawns all four story items; failed placement probes make
    // each fall back to the corpse coordinates.
    damage =
        rloft::Level22EncounterModel::damage_assassin(
            world,
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
    assert(damage.sound_kind ==
           rloft::NpcSoundKind::Death);
    assert(damage.sound_id == 22);
    assert(damage.experience_award == 200);
    assert(damage.spawned_loose_items == 4);
    assert((world.npcs[0].runtime_flags & 0x02000000U) != 0U);
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
}
