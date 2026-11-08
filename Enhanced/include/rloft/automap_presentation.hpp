#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

#include "rloft/automap_annotations.hpp"

#include "rloft/exploration_state.hpp"
#include "rloft/exploration_world.hpp"

namespace rloft {

class AutomapPresentationModel final {
public:
    // CODE.2 009B0: resolve generated cache classes into connected-map
    // display values. Values >= 0x20 are already resolved.
    [[nodiscard]] static std::uint8_t resolve_cell(
        const LevelState& level,
        CellPosition cell);

    // CODE.2 026E8..02A33 base export layer. This intentionally excludes
    // family-B item overlays and annotation labels; those are separate layers.
    [[nodiscard]] static std::uint8_t cp437_base_glyph(
        const LevelState& level,
        CellPosition cell);

    [[nodiscard]] static std::vector<std::uint8_t> cp437_base_map(
        const LevelState& level);

private:
    [[nodiscard]] static std::uint8_t cache_at(
        const LevelState& level,
        CellPosition cell);
};

struct AutomapExportColumn final {
    std::vector<std::uint8_t> base;
    std::vector<std::uint8_t> placed;
    std::vector<std::uint8_t> labels;
    std::vector<std::uint8_t> merged;

    // The original exporter allocates exactly map_height bytes for labels but
    // advances its destination cursor by three positions when a two-byte label
    // is found. Preserve that cursor behavior while surfacing unsafe writes
    // instead of reproducing DOS heap corruption in native code.
    bool label_write_overflow{};
};

class AutomapExportModel final {
public:
    // CODE.2 026E8 block-mode planes. The outer vector follows the executable:
    // one height-byte output slice for each map column.
    [[nodiscard]] static std::vector<AutomapExportColumn> columns(
        const LevelState& level);

    [[nodiscard]] static std::vector<std::uint8_t> placed_overlay(
        const LevelState& level,
        int column);

    [[nodiscard]] static AutomapExportColumn column(
        const LevelState& level,
        int column);

private:
    struct PlacedRecord final {
        std::int16_t column{};
        std::int16_t row{};
        std::uint16_t type{};
    };

    [[nodiscard]] static std::vector<PlacedRecord> placed_records(
        const LevelState& level);
};

}  // namespace rloft
