#include "rloft/exploration_world.hpp"

#include <algorithm>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

std::uint16_t u16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8U);
}

int fixed_integer(std::uint32_t value) noexcept {
    return static_cast<std::int32_t>(value) >> 16;
}

std::uint8_t floor_class(const LevelState& level) noexcept {
    return level.environment_profile == 0xFF ? 0x2FU : 0x2EU;
}

bool same_cell(const std::optional<CellPosition>& a, CellPosition b) noexcept {
    return a.has_value() && *a == b;
}

}  // namespace

CellPosition ExplorationWorldModel::party_cell(
    const ExplorationSessionState& session) noexcept {
    // SAVE/CODE.2 coordinate A is the x/column fixed-point coordinate and
    // coordinate B is y/row. Family-A cached-cell reconstruction independently
    // proves index = y_cell * map_width + x_cell.
    return CellPosition{
        fixed_integer(session.coordinate_a),
        fixed_integer(session.coordinate_b),
    };
}

bool ExplorationWorldModel::in_bounds(
    const LevelState& level, CellPosition cell) noexcept {
    return cell.column >= 0 && cell.row >= 0 &&
           cell.column < level.width && cell.row < level.height;
}

std::size_t ExplorationWorldModel::cell_index(
    const LevelState& level, CellPosition cell) {
    if (!in_bounds(level, cell)) {
        throw std::out_of_range("level cell coordinate");
    }
    return static_cast<std::size_t>(cell.row) *
               static_cast<std::size_t>(level.width) +
           static_cast<std::size_t>(cell.column);
}

std::uint16_t ExplorationWorldModel::cell_word(
    const LevelState& level, CellPosition cell) {
    const auto index = cell_index(level, cell);
    if (index >= level.cell_words.size()) {
        throw std::runtime_error("level cell array does not match dimensions");
    }
    return level.cell_words[index];
}

void ExplorationWorldModel::clear_current_visibility(LevelState& level) {
    for (auto& word : level.cell_words) {
        word = static_cast<std::uint16_t>(word & ~kCurrentVisible);
    }
}

void ExplorationWorldModel::mark_visible_and_explored(
    LevelState& level, CellPosition cell) {
    const auto index = cell_index(level, cell);
    if (index >= level.cell_words.size()) {
        throw std::runtime_error("level cell array does not match dimensions");
    }
    level.cell_words[index] =
        static_cast<std::uint16_t>(level.cell_words[index] | kVisibleAndExplored);
}

const CellControlRecord* ExplorationWorldModel::find_record(
    const std::vector<CellControlRecord>& records,
    CellPosition cell) noexcept {
    const auto it = std::find_if(
        records.begin(), records.end(),
        [cell](const CellControlRecord& record) {
            return record.column() == cell.column && record.row() == cell.row;
        });
    return it == records.end() ? nullptr : &*it;
}

std::uint8_t ExplorationWorldModel::classify_automap_cell(
    const LevelState& level,
    CellPosition cell,
    std::optional<CellPosition> party) {
    const auto index = cell_index(level, cell);
    if (index >= level.cell_words.size()) {
        throw std::runtime_error("level cell array does not match dimensions");
    }

    if (same_cell(party, cell)) {
        return floor_class(level);
    }

    const auto word = level.cell_words[index];
    const auto code = static_cast<std::uint8_t>(word & 0x003FU);
    const auto table_at = static_cast<std::size_t>(0x00A) +
                          static_cast<std::size_t>(code) * 2U;
    if (table_at + 1U >= level.header.size()) {
        throw std::runtime_error("level primary resource table is truncated");
    }
    const auto primary_resource = u16(level.header.data() + table_at);
    const bool explored = (word & kExplored) != 0;

    if (explored) {
        if (code == 0 || primary_resource == 0x0093U) {
            return floor_class(level);
        }
        if ((word & 0x0800U) != 0) {
            return 0x05U;
        }
        if ((word & 0x0080U) == 0) {
            return 0x01U;
        }

        const bool has_any_record =
            find_record(level.group1, cell) != nullptr ||
            find_record(level.group2, cell) != nullptr;
        if (!has_any_record) {
            return 0x03U;
        }

        const auto* dynamic = find_record(level.group2, cell);
        const std::uint16_t dynamic_state =
            dynamic == nullptr
                ? 0x8000U
                : static_cast<std::uint16_t>(
                      static_cast<std::uint16_t>(dynamic->raw[6]) << 12U);
        return dynamic_state == 0x8000U ? 0x04U : 0x02U;
    }

    // This 0x0093 shortcut is present in certified CODE.2 for unexplored
    // cells. Keep it despite the separately documented LEV022 cache anomaly.
    if (code != 0 && primary_resource == 0x0093U) {
        return floor_class(level);
    }

    if (code != 0) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                const CellPosition neighbor{cell.column + dx, cell.row + dy};
                if (!in_bounds(level, neighbor)) continue;
                const auto neighbor_word = cell_word(level, neighbor);
                if ((neighbor_word & kExplored) != 0 &&
                    (neighbor_word & 0x003FU) == 0) {
                    return 0x01U;
                }
            }
        }
    }

    return 0xFFU;
}

void ExplorationWorldModel::rebuild_automap_cache(
    LevelState& level,
    std::optional<CellPosition> party) {
    const auto expected =
        static_cast<std::size_t>(level.width) *
        static_cast<std::size_t>(level.height);
    if (level.width <= 0 || level.height <= 0 ||
        level.cell_words.size() != expected) {
        throw std::runtime_error("invalid level geometry for automap rebuild");
    }

    level.automap_cache.resize(expected);
    for (int row = 0; row < level.height; ++row) {
        for (int column = 0; column < level.width; ++column) {
            const CellPosition cell{column, row};
            level.automap_cache[cell_index(level, cell)] =
                classify_automap_cell(level, cell, party);
        }
    }
}

}  // namespace rloft
