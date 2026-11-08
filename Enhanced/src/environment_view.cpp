#include "rloft/environment_view.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>

namespace rloft {
namespace {

std::uint16_t read_u16(
    const std::array<std::uint8_t, 0x2A4>& header,
    std::size_t offset) {
    if (offset + 1U >= header.size()) {
        throw std::out_of_range("level renderer lookup exceeds header");
    }
    return static_cast<std::uint16_t>(header[offset]) |
           (static_cast<std::uint16_t>(header[offset + 1U]) << 8U);
}

template <std::size_t N>
void read_bank(
    const std::array<std::uint8_t, 0x2A4>& header,
    std::size_t start,
    std::array<std::uint16_t, N>& out) {
    for (std::size_t i = 0; i < N; ++i) {
        out[i] = read_u16(header, start + i * 2U);
    }
}

}  // namespace

LevelResourceBanks EnvironmentViewModel::resource_banks(
    const LevelState& level) {
    LevelResourceBanks out{};
    read_bank(level.header, 0x00AU, out.primary_a);
    read_bank(level.header, 0x08AU, out.alternate_a);
    read_bank(level.header, 0x10AU, out.primary_b);
    read_bank(level.header, 0x18AU, out.alternate_b);
    read_bank(level.header, 0x20AU, out.automap_style);
    return out;
}

const CellControlRecord* EnvironmentViewModel::find_group1_record(
    const LevelState& level,
    CellPosition cell) noexcept {
    const auto it = std::find_if(
        level.group1.begin(),
        level.group1.end(),
        [cell](const CellControlRecord& record) {
            return record.column() == cell.column &&
                   record.row() == cell.row;
        });
    return it == level.group1.end() ? nullptr : &*it;
}

CellVisualInputs EnvironmentViewModel::cell_visual_inputs(
    const LevelState& level,
    CellPosition cell) {
    const auto word = ExplorationWorldModel::cell_word(level, cell);
    const auto code = static_cast<std::uint8_t>(word & 0x003FU);
    const auto banks = resource_banks(level);

    CellVisualInputs out{};
    out.cell = cell;
    out.raw_word = word;
    out.cell_code = code;
    out.primary_a = banks.primary_a[code];
    out.alternate_a = banks.alternate_a[code];
    out.primary_b = banks.primary_b[code];
    out.alternate_b = banks.alternate_b[code];
    out.automap_style = banks.automap_style[code];

    out.special_geometry_center = (word & 0x0080U) != 0;
    out.special_geometry_frame = (word & 0x0100U) != 0;
    out.extra_render_prepass = (word & 0x0200U) != 0;
    out.hidden_passage_topology = (word & 0x0800U) != 0;
    out.record_state_variant_enabled = (word & 0x1000U) != 0;
    out.shade_decrement =
        static_cast<std::uint8_t>((word & 0xC000U) >> 14U);

    if (out.record_state_variant_enabled) {
        if (const auto* record = find_group1_record(level, cell)) {
            out.record_state_variant =
                static_cast<std::uint8_t>(record->control() & 0x03U);
        }
    }

    return out;
}

}  // namespace rloft
