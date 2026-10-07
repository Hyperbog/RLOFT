#include "rloft/npc_scene.hpp"
#include "rloft/scene_shape_bounds.hpp"

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

rloft::NpcCatalog assassin_catalog() {
    std::vector<std::uint8_t> bytes(
        4U + 10U * 0x45U, 0);
    put32(bytes, 0, 10);

    const auto r = 4U + 9U * 0x45U;
    const char* name = "ASSASSIN";
    for (std::size_t i = 0; name[i]; ++i) {
        bytes[r + i] =
            static_cast<std::uint8_t>(
                name[i]);
    }

    // Certified supplied NPCS.DAT record 9.
    put16(bytes, r + 0x14, 315);
    put16(bytes, r + 0x16, 0x8000);
    bytes[r + 0x18] = 8;
    bytes[r + 0x19] = 4;
    bytes[r + 0x1A] = 3;
    bytes[r + 0x1C] = 1;
    bytes[r + 0x1D] = 4;
    bytes[r + 0x1E] = 1;
    bytes[r + 0x1F] = 6;
    bytes[r + 0x20] = 0;
    bytes[r + 0x21] = 5;
    bytes[r + 0x22] = 6;
    put16(bytes, r + 0x23, 200);
    put16(bytes, r + 0x27, 0);
    put32(bytes, r + 0x31, 0x00000017U);
    put32(bytes, r + 0x35, 0x10000000U);
    put16(bytes, r + 0x3D, 88);
    put16(bytes, r + 0x3F, 89);
    put16(bytes, r + 0x41, 90);
    put16(bytes, r + 0x43, 22);

    return rloft::NpcCatalog::parse(bytes);
}

rloft::RendererTables renderer() {
    rloft::RendererTables tables{};
    tables.sine_q16_signmag.fill(0);
    tables.sine_q16_signmag[90] =
        0x00008000U;
    tables.constants[2] = 160U;        // C6D0.
    tables.constants[5] = 0U;          // Camera-origin offset.
    tables.constants[6] = 100U;        // C6CC.
    tables.constants[7] = 0x00100000U; // 255E0.
    return tables;
}

rloft::ExplorationSessionState session() {
    rloft::ExplorationSessionState s{};
    s.facing = 0;
    s.level.width = 1;
    s.level.height = 1;
    s.level.cell_words = {0x0040U};
    return s;
}

rloft::LevelWorldState world() {
    rloft::LevelWorldState w{};
    rloft::LiveNpcRecord assassin{};
    assassin.y_position_16_16 =
        0x00020000U;
    assassin.x_position_16_16 = 0U;
    assassin.runtime_flags =
        0x10000000U;
    assassin.cached_cell_index = 0;
    assassin.npc_type_id = 9;
    assassin.current_hp = 15;
    assassin.packed_runtime_state =
        0x0030U; // facing 6, action state 0.
    w.npcs.push_back(assassin);
    return w;
}

}  // namespace

int main() {
    auto s = session();
    auto w = world();
    auto tables = renderer();
    const auto catalog = assassin_catalog();
    rloft::SceneSourceBanks banks{};
    banks.base = {0U, 2169U, 3316U, 4158U};

    auto scene =
        rloft::NpcSceneModel::build(
            w, s, catalog, tables, banks, 1U);
    assert(scene.size() == 1U);
    assert(scene[0].screen_x == 160U);
    assert(scene[0].projected_extent == 4681U);
    assert(scene[0].resource_id == 319U);
    assert(scene[0].source_index == 0U);
    assert(scene[0].source_class == 0U);
    assert(scene[0].render_scale_lookup_bias == 0U);

    // Damage/reaction state 1 selects the exact alternate block and
    // relative-orientation contribution.
    w.npcs[0].packed_runtime_state =
        0x0032U;
    scene =
        rloft::NpcSceneModel::build(
            w, s, catalog, tables, banks, 1U);
    assert(scene.size() == 1U);
    assert(scene[0].resource_id == 348U);

    // State 3 locally seeds CODE.2's LCG from the NPC index and bypasses
    // 2B2F4 orientation. Index zero produces random bit zero.
    w.npcs[0].packed_runtime_state =
        0x0036U;
    scene =
        rloft::NpcSceneModel::build(
            w, s, catalog, tables, banks, 1U);
    assert(scene.size() == 1U);
    assert(scene[0].resource_id == 350U);

    // Current visibility is mandatory.
    s.level.cell_words[0] = 0U;
    scene =
        rloft::NpcSceneModel::build(
            w, s, catalog, tables, banks, 1U);
    assert(scene.empty());

    // The encounter death/suppression bit removes the NPC from scene output.
    s.level.cell_words[0] = 0x0040U;
    w.npcs[0].runtime_flags |=
        0x02000000U;
    scene =
        rloft::NpcSceneModel::build(
            w, s, catalog, tables, banks, 1U);
    assert(scene.empty());

    // The encoded comparison is projected_extent <= C02D.
    w.npcs[0].runtime_flags =
        0x10000000U;
    w.npcs[0].packed_runtime_state =
        0x0030U;
    tables.constants[7] = 22400U;
    scene =
        rloft::NpcSceneModel::build(
            w, s, catalog, tables, banks, 100U);
    assert(scene.empty());

    // CODE.2 21BF0 uses the selected type-7 geometry to construct the
    // clickable screen rectangle. With a 400-unit projected extent, the
    // Assassin base shape produces this exact clipped VGA-space box.
    const rloft::Type7Geometry assassin_shape{
        64U, 15, 51, 27, 128
    };
    const auto hit =
        rloft::SceneShapeBoundsModel::npc_type7(
            128U,
            400U,
            assassin_shape);
    assert(hit.valid);
    assert(hit.top == 115U);
    assert(hit.bottom == 254U);
    assert(hit.left == 137U);
    assert(hit.right == 187U);
    assert(hit.contains(115, 137));
    assert(hit.contains(254, 187));
    assert(!hit.contains(114, 137));
    assert(!hit.contains(115, 188));

    const auto interaction =
        rloft::SceneShapeBoundsModel::interaction_type7(
            128U,
            400U,
            assassin_shape);
    assert(interaction.valid);
    assert(interaction.top == 115U);
    assert(interaction.bottom == 254U);
    assert(interaction.left == 105U);
    assert(interaction.right == 155U);

    const auto emitter_too_small =
        rloft::SceneShapeBoundsModel::emitter_type7(
            128U,
            319U,
            assassin_shape);
    assert(!emitter_too_small.valid);

    const auto interaction_small =
        rloft::SceneShapeBoundsModel::interaction_type7(
            128U,
            319U,
            assassin_shape);
    assert(interaction_small.valid);

    return 0;
}
