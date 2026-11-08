#include "rloft/placed_object_scene.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

void put16(
    std::vector<std::uint8_t>& bytes,
    std::size_t offset,
    std::uint16_t value) {
    bytes[offset] =
        static_cast<std::uint8_t>(value);
    bytes[offset + 1U] =
        static_cast<std::uint8_t>(
            value >> 8U);
}

rloft::PlacedSceneCatalog catalog() {
    constexpr std::size_t count = 65U;
    std::vector<std::uint8_t> init(
        0x26U + count * 7U, 0);
    put16(init, 0x24U, count);

    const auto at =
        0x26U + 62U * 7U;
    put16(init, at + 0U, 100U);
    init[at + 4U] = 16U;
    init[at + 5U] = 1U;
    init[at + 6U] = 0U;

    return rloft::PlacedSceneCatalog::parse_init(
        init);
}

}  // namespace

int main() {
    rloft::RendererTables tables{};
    tables.sine_q16_signmag.fill(0);
    tables.sine_q16_signmag[90] =
        0x00008000U;
    tables.constants[2] = 160U;
    tables.constants[5] = 0U;
    tables.constants[6] = 100U;
    tables.constants[7] = 0x00100000U;

    rloft::ExplorationSessionState session{};
    session.facing = 0;
    session.level.width = 1;
    session.level.height = 1;
    session.level.cell_words = {0x0040U};

    rloft::LevelWorldState world{};
    rloft::LiveObjectRecord object{};
    object.y_position_16_16 = 0x00020000U;
    object.x_position_16_16 = 0U;
    object.cached_cell_index = 0U;
    object.object_type_id = 62U;
    object.packed_runtime_state = 0U;
    world.placed_objects.push_back(object);

    rloft::SceneSourceBanks banks{};
    banks.base = {0U, 2169U, 3316U, 4158U};

    const auto scene =
        rloft::PlacedObjectSceneModel::build(
            world,
            session,
            catalog(),
            tables,
            banks,
            1U);

    assert(scene.size() == 1U);
    assert(scene[0].screen_x == 160U);
    assert(scene[0].projected_extent == 4681U);
    // class-1 type 62 uses the 0x3C..0x43 stride-16 mirror: 100 -> 107.
    assert(scene[0].resource_id == 2276U);
    assert(scene[0].source_index == 0U);
    assert(scene[0].source_class == 1U);
    assert(scene[0].source_type_id == 62U);

    session.level.cell_words[0] = 0U;
    assert(
        rloft::PlacedObjectSceneModel::build(
            world,
            session,
            catalog(),
            tables,
            banks,
            1U).empty());

    return 0;
}
