#include "rloft/level_world_state.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

void put16(std::vector<std::uint8_t>& b,
           std::size_t o,
           std::uint16_t v) {
    b[o] = static_cast<std::uint8_t>(v);
    b[o + 1U] =
        static_cast<std::uint8_t>(v >> 8U);
}
void put32(std::vector<std::uint8_t>& b,
           std::size_t o,
           std::uint32_t v) {
    for (int i = 0; i < 4; ++i) {
        b[o + static_cast<std::size_t>(i)] =
            static_cast<std::uint8_t>(v >> (i * 8));
    }
}

rloft::NpcCatalog catalog() {
    std::vector<std::uint8_t> bytes(
        4U + 10U * 0x45U, 0);
    put32(bytes, 0, 10);
    const auto r = 4U + 9U * 0x45U;
    bytes[r + 0] = 'A';
    bytes[r + 1] = 0;
    bytes[r + 0x1D] = 4;
    put32(bytes, r + 0x35, 0x10000000U);
    return rloft::NpcCatalog::parse(bytes);
}

}  // namespace

int main() {
    rloft::LevelState archive{};
    archive.storage =
        rloft::LevelStorage::ArchiveCompact;
    archive.width = 30;
    archive.height = 15;

    archive.family_a_raw.resize(11);
    put16(archive.family_a_raw, 0, 9);
    put16(archive.family_a_raw, 2, 13);
    put16(archive.family_a_raw, 4, 0x7C00);
    put16(archive.family_a_raw, 6, 7);
    put16(archive.family_a_raw, 8, 0x7800);
    archive.family_a_raw[10] = 2;

    archive.family_b_raw.resize(11);
    put16(archive.family_b_raw, 0, 0x4E);
    put16(archive.family_b_raw, 2, 5);
    put16(archive.family_b_raw, 4, 0x1111);
    put16(archive.family_b_raw, 6, 6);
    put16(archive.family_b_raw, 8, 0x2222);
    archive.family_b_raw[10] = 7;

    archive.family_c_raw.resize(11);
    put16(archive.family_c_raw, 0, 0x54);
    put16(archive.family_c_raw, 2, 8);
    put16(archive.family_c_raw, 4, 0x3333);
    put16(archive.family_c_raw, 6, 9);
    put16(archive.family_c_raw, 8, 0x4444);

    int rolls = 0;
    const auto world =
        rloft::LevelWorldStateCodec::build(
            archive,
            catalog(),
            [&](std::uint8_t count,
                std::uint8_t sides) {
                ++rolls;
                assert(count == 4);
                assert(sides == 8);
                return static_cast<std::uint16_t>(15);
            });

    assert(rolls == 1);
    assert(world.npcs.size() == 1);
    const auto& a = world.npcs[0];
    assert(a.npc_type_id == 9);
    assert(a.x_position_16_16 == 0x000D7C00U);
    assert(a.y_position_16_16 == 0x00077800U);
    assert(a.cached_cell_index == 223);
    assert(a.runtime_flags == 0x10000000U);
    assert(a.current_hp == 15);
    assert(a.facing() == 2);

    assert(world.placed_objects.size() == 1);
    assert(world.placed_objects[0].object_type_id == 0x4E);
    assert(world.placed_objects[0].cached_cell_index == 185);
    assert(
        ((world.placed_objects[0].packed_runtime_state >> 3U) & 7U) ==
        7U);

    assert(world.loose_items.size() == 1);
    assert(world.loose_items[0].object_type_id == 0x58);
    assert(world.loose_items[0].cached_cell_index == 278);

    rloft::LevelState direct{};
    direct.storage =
        rloft::LevelStorage::DirectSave;
    direct.width = 30;
    direct.height = 15;

    direct.family_a_raw.resize(0x16);
    put32(direct.family_a_raw, 0x00, 0x00077800);
    put32(direct.family_a_raw, 0x04, 0x00127C00);
    put32(direct.family_a_raw, 0x08, 0x10000000);
    put16(direct.family_a_raw, 0x0C, 228);
    put16(direct.family_a_raw, 0x0E, 9);
    put16(direct.family_a_raw, 0x10, 4);
    put16(direct.family_a_raw, 0x12, 15);
    put16(direct.family_a_raw, 0x14, 0x0030);

    direct.family_b_raw.resize(2U + 0x16U, 0);
    put16(direct.family_b_raw, 0, 1);
    put16(direct.family_b_raw, 2U + 0x0E, 44);
    put16(direct.family_b_raw, 2U + 0x10, 0x1234);
    put16(direct.family_b_raw, 2U + 0x12, 0x5678);
    put16(direct.family_b_raw, 2U + 0x14, 0x9ABC);

    direct.family_c_raw.resize(0x16, 0);
    put16(direct.family_c_raw, 0x0E, 0xE9);
    put16(direct.family_c_raw, 0x10, 7);
    put16(direct.family_c_raw, 0x12, 8);
    put16(direct.family_c_raw, 0x14, 9);

    const auto restored =
        rloft::LevelWorldStateCodec::build(
            direct,
            catalog(),
            {});
    assert(restored.npcs[0].x_position_16_16 == 0x00127C00U);
    assert(restored.npcs[0].action_state_countdown == 4);
    assert(restored.npcs[0].current_hp == 15);
    assert(restored.npcs[0].facing() == 6);
    assert(restored.placed_objects[0].object_type_id == 44);
    assert(restored.placed_objects[0].state_word == 0x1234);
    assert(restored.placed_objects[0].auxiliary_word == 0x5678);
    assert(restored.placed_objects[0].packed_runtime_state == 0x9ABC);
    assert(restored.loose_items[0].object_type_id == 0xE9);
    assert(restored.loose_items[0].state_word == 7);
    assert(restored.loose_items[0].auxiliary_word == 8);
    assert(restored.loose_items[0].packed_runtime_state == 9);
}
