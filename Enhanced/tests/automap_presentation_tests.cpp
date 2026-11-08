#include "rloft/automap_presentation.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

rloft::LevelState level3() {
    rloft::LevelState level{};
    level.width = 3;
    level.height = 3;
    level.cell_words.assign(9, 0);
    level.automap_cache.assign(9, 0xFF);
    return level;
}

void put16(
    std::vector<std::uint8_t>& bytes,
    std::size_t at,
    std::uint16_t value) {
    bytes[at] = static_cast<std::uint8_t>(value);
    bytes[at + 1U] =
        static_cast<std::uint8_t>(value >> 8U);
}

void put_header16(
    rloft::LevelState& level,
    std::size_t at,
    std::uint16_t value) {
    level.header[at] =
        static_cast<std::uint8_t>(value);
    level.header[at + 1U] =
        static_cast<std::uint8_t>(value >> 8U);
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

    // 026E8 family-B placed-object plane: archive compact and direct-save
    // encodings must yield the same runtime-visible overlay.
    rloft::LevelState archive{};
    archive.storage = rloft::LevelStorage::ArchiveCompact;
    archive.width = 2;
    archive.height = 3;
    archive.cell_words.assign(6, 0);
    archive.automap_cache.assign(6, 0x2E);
    put_header16(archive, 0x28E, 3);
    archive.family_b_raw.assign(3U * 11U, 0);

    auto compact = [&](std::size_t index,
                       std::uint16_t type,
                       std::int16_t x,
                       std::int16_t y) {
        const auto at = index * 11U;
        put16(archive.family_b_raw, at + 0U, type);
        put16(
            archive.family_b_raw,
            at + 2U,
            static_cast<std::uint16_t>(x));
        put16(
            archive.family_b_raw,
            at + 6U,
            static_cast<std::uint16_t>(y));
    };
    compact(0, 0x3C, 1, 1);
    compact(1, 0x10, 1, 1); // later record overwrites '*' with FE
    compact(2, 0x41, 0, 0);

    auto placed =
        rloft::AutomapExportModel::placed_overlay(
            archive, 1);
    assert(placed.size() == 3);
    assert(placed[0] == 0x20);
    assert(placed[1] == 0xFE);
    assert(placed[2] == 0x20);

    auto direct = archive;
    direct.storage = rloft::LevelStorage::DirectSave;
    direct.family_b_raw.assign(2U + 3U * 22U, 0);
    put16(direct.family_b_raw, 0, 3);
    auto expanded = [&](std::size_t index,
                        std::uint16_t type,
                        std::int16_t x,
                        std::int16_t y) {
        const auto at = 2U + index * 22U;
        // High words of y/x 16.16 at +2/+6.
        put16(
            direct.family_b_raw,
            at + 2U,
            static_cast<std::uint16_t>(y));
        put16(
            direct.family_b_raw,
            at + 6U,
            static_cast<std::uint16_t>(x));
        put16(direct.family_b_raw, at + 0x0EU, type);
    };
    expanded(0, 0x3C, 1, 1);
    expanded(1, 0x10, 1, 1);
    expanded(2, 0x41, 0, 0);

    const auto direct_placed =
        rloft::AutomapExportModel::placed_overlay(
            direct, 1);
    assert(direct_placed == placed);

    // Unexplored cells suppress placed-object symbols even when records exist.
    direct.automap_cache[
        rloft::ExplorationWorldModel::cell_index(
            direct, {0, 0})] = 0xFF;
    const auto hidden =
        rloft::AutomapExportModel::placed_overlay(
            direct, 0);
    assert(hidden[0] == 0x20);

    // Preserve 026E8's label_ptr quirk. With labels at rows 1 and 2, the
    // first "A " consumes output positions 1/2 then skips to 4; "B " is
    // therefore emitted at positions 4/5 rather than at its source row.
    rloft::LevelState annotated{};
    annotated.width = 1;
    annotated.height = 6;
    annotated.cell_words.assign(6, 0);
    annotated.automap_cache.assign(6, 0x2E);
    put_header16(annotated, 0x28E, 0);

    auto append_annotation = [&](std::int16_t raw_x,
                                 std::int16_t raw_y,
                                 std::uint8_t payload) {
        const auto at = annotated.automap_annotations.size();
        annotated.automap_annotations.resize(at + 7U);
        put16(
            annotated.automap_annotations,
            at + 0U,
            static_cast<std::uint16_t>(raw_x));
        put16(
            annotated.automap_annotations,
            at + 2U,
            static_cast<std::uint16_t>(raw_y));
        put16(annotated.automap_annotations, at + 4U, 1);
        annotated.automap_annotations[at + 6U] = payload;
    };
    append_annotation(0, 5, 'x');  // map row 1 -> A
    append_annotation(0, 10, 'y'); // map row 2 -> B

    const auto exported =
        rloft::AutomapExportModel::column(
            annotated, 0);
    assert(exported.base.size() == 6);
    assert(exported.placed.size() == 6);
    assert(exported.labels.size() == 6);
    assert(exported.merged.size() == 6);
    assert(exported.labels[0] == 0x20);
    assert(exported.labels[1] == 'A');
    assert(exported.labels[2] == 0x20);
    assert(exported.labels[3] == 0x20);
    assert(exported.labels[4] == 'B');
    assert(exported.labels[5] == 0x20);
    assert(!exported.label_write_overflow);
    assert(exported.merged[1] == 'A');
    assert(exported.merged[2] == 0xFA);
    assert(exported.merged[4] == 'B');

    const auto columns =
        rloft::AutomapExportModel::columns(
            annotated);
    assert(columns.size() == 1);
    assert(columns[0].merged == exported.merged);
}
