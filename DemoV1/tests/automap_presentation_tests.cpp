#include "rloft/automap_presentation.hpp"

#include <cassert>

namespace {

rloft::LevelState level3() {
    rloft::LevelState level{};
    level.width = 3;
    level.height = 3;
    level.cell_words.assign(9, 0);
    level.automap_cache.assign(9, 0xFF);
    return level;
}

void set(rloft::LevelState& level,
         int x,
         int y,
         std::uint8_t value) {
    level.automap_cache[
        rloft::ExplorationWorldModel::cell_index(level, {x, y})] = value;
}

}  // namespace

int main() {
    using rloft::AutomapPresentationModel;

    auto level = level3();

    // Unexplored is a literal space in the original exporter.
    assert(AutomapPresentationModel::cp437_base_glyph(
               level, {1, 1}) == 0x20);

    // Class 1 uses the exact 16-entry connectivity table. Left-only gives
    // table[1] = 0x0F, which exports through the generic wall fallback 0xDB.
    set(level, 1, 1, 0x01);
    set(level, 0, 1, 0x01);
    assert(AutomapPresentationModel::resolve_cell(
               level, {1, 1}) == 0x0F);
    assert(AutomapPresentationModel::cp437_base_glyph(
               level, {1, 1}) == 0xDB);

    // Class 2 aligned on up+down becomes resolved 0x29 -> horizontal door.
    level = level3();
    set(level, 1, 1, 0x02);
    set(level, 1, 0, 0x01);
    set(level, 1, 2, 0x01);
    assert(AutomapPresentationModel::resolve_cell(
               level, {1, 1}) == 0x29);
    assert(AutomapPresentationModel::cp437_base_glyph(
               level, {1, 1}) == 0xCD);

    // Class 4 aligned left+right becomes 0x2A -> vertical door.
    level = level3();
    set(level, 1, 1, 0x04);
    set(level, 0, 1, 0x01);
    set(level, 2, 1, 0x01);
    assert(AutomapPresentationModel::resolve_cell(
               level, {1, 1}) == 0x2A);
    assert(AutomapPresentationModel::cp437_base_glyph(
               level, {1, 1}) == 0xBA);

    // Class 3 floor direction selects inset orientation.
    level = level3();
    set(level, 1, 1, 0x03);
    set(level, 1, 0, 0x2E);
    assert(AutomapPresentationModel::resolve_cell(
               level, {1, 1}) == 0x23);
    assert(AutomapPresentationModel::cp437_base_glyph(
               level, {1, 1}) == 0xDE);

    // Class 5 with opposite floor neighbors resolves to secret marker 0x33.
    level = level3();
    set(level, 1, 1, 0x05);
    set(level, 1, 0, 0x2E);
    set(level, 1, 2, 0x2F);
    assert(AutomapPresentationModel::resolve_cell(
               level, {1, 1}) == 0x33);
    assert(AutomapPresentationModel::cp437_base_glyph(
               level, {1, 1}) == 0xB2);

    // Already-resolved floor class bypasses the connector.
    set(level, 0, 0, 0x2E);
    assert(AutomapPresentationModel::resolve_cell(
               level, {0, 0}) == 0x2E);
    assert(AutomapPresentationModel::cp437_base_glyph(
               level, {0, 0}) == 0xFA);

    const auto map = AutomapPresentationModel::cp437_base_map(level);
    assert(map.size() == 9);
}
