#include "rloft/loose_item_scene.hpp"

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

rloft::ObjectCatalog objects() {
    std::vector<std::uint8_t> bytes(
        4U + 0x89U * 0x41U, 0);
    put32(bytes, 0, 0x89U);
    const auto r =
        4U + 0x88U * 0x41U;
    put16(bytes, r + 0x28U, 694U);
    bytes[r + 0x2AU] = 8U;
    bytes[r + 0x2BU] = 0U;
    return rloft::ObjectCatalog::parse(bytes);
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
    rloft::LiveObjectRecord item{};
    item.y_position_16_16 = 0x00020000U;
    item.x_position_16_16 = 0U;
    item.cached_cell_index = 0U;
    item.object_type_id = 0x88U;
    item.packed_runtime_state = 0U;
    world.loose_items.push_back(item);

    rloft::SceneSourceBanks banks{};
    banks.base = {0U, 2169U, 3316U, 4158U};

    const auto scene =
        rloft::LooseItemSceneModel::build(
            world,
            session,
            objects(),
            tables,
            banks,
            1U);
    assert(scene.size() == 1U);
    assert(scene[0].screen_x == 160U);
    assert(scene[0].projected_extent == 4681U);
    assert(scene[0].resource_id == 4010U);
    assert(scene[0].source_index == 0U);
    assert(scene[0].source_class == 2U);
    assert(scene[0].render_scale_lookup_bias == 0U);

    session.level.cell_words[0] = 0U;
    assert(
        rloft::LooseItemSceneModel::build(
            world,
            session,
            objects(),
            tables,
            banks,
            1U).empty());

    return 0;
}
