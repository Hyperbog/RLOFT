#include "rloft/automap_presentation.hpp"

#include <array>
#include <cstddef>
#include <stdexcept>

namespace rloft {
namespace {

// CODE.2 data object 0x5FF0: dword table indexed by the four-bit
// non-floor-neighbor mask. Only the low byte is nonzero in the shipped table.
constexpr std::array<std::uint8_t, 16> kConnectivity{{
    0x06, 0x0F, 0x0E, 0x05,
    0x0C, 0x04, 0x00, 0x0A,
    0x0D, 0x01, 0x03, 0x09,
    0x02, 0x07, 0x08, 0x0B,
}};

// CODE.2 data bytes reached through [0x6011 + resolved], resolved 0x20..0x33.
constexpr std::array<std::uint8_t, 20> kCp437{{
    0xDF, 0xDC, 0xDD, 0xDE,
    0xDF, 0xDC, 0xDD, 0xDE,
    0xCD, 0xCD, 0xBA, 0xBA,
    0xFA, 0xFA, 0xFA, 0xFA,
    0xE9, 0xB0, 0x09, 0xB2,
}};

bool is_floor(std::uint8_t value) noexcept {
    return value == 0x2EU || value == 0x2FU;
}

}  // namespace

std::uint8_t AutomapPresentationModel::cache_at(
    const LevelState& level,
    CellPosition cell) {
    const auto index = ExplorationWorldModel::cell_index(level, cell);
    if (index >= level.automap_cache.size()) {
        throw std::runtime_error(
            "automap cache does not match level dimensions");
    }
    return level.automap_cache[index];
}

std::uint8_t AutomapPresentationModel::resolve_cell(
    const LevelState& level,
    CellPosition cell) {
    const auto current = cache_at(level, cell);
    if (current == 0xFFU || current >= 0x20U) {
        return current;
    }

    std::uint8_t wall_mask = 0;
    std::uint8_t floor_mask = 0;

    const auto add_neighbor = [&](CellPosition neighbor,
                                  std::uint8_t bit) {
        if (!ExplorationWorldModel::in_bounds(level, neighbor)) {
            return;
        }
        const auto value = cache_at(level, neighbor);
        if (value < 0x20U) {
            wall_mask = static_cast<std::uint8_t>(wall_mask | bit);
        }
        if (is_floor(value)) {
            floor_mask = static_cast<std::uint8_t>(floor_mask | bit);
        }
    };

    // Exact CODE.2 009B0 mask assignment:
    // left=1, up=2, down=4, right=8.
    add_neighbor({cell.column - 1, cell.row}, 0x01U);
    add_neighbor({cell.column + 1, cell.row}, 0x08U);
    add_neighbor({cell.column, cell.row - 1}, 0x02U);
    add_neighbor({cell.column, cell.row + 1}, 0x04U);

    switch (current) {
        case 0x01:
            return kConnectivity[wall_mask];

        case 0x02:
            if (wall_mask == 0x06U) return 0x29U;
            if (wall_mask == 0x09U) return 0x2BU;
            return wall_mask;

        case 0x03:
            if (floor_mask == 0x01U) return 0x21U;
            if (floor_mask == 0x02U) return 0x23U;
            if (floor_mask == 0x04U) return 0x22U;
            if (floor_mask == 0x08U) return 0x20U;
            if ((floor_mask & 0x09U) != 0) return 0x20U;
            if ((floor_mask & 0x06U) != 0) return 0x22U;
            return wall_mask;

        case 0x04:
            if (wall_mask == 0x06U) return 0x28U;
            if (wall_mask == 0x09U) return 0x2AU;
            return wall_mask;

        case 0x05:
            if ((floor_mask & 0x06U) == 0x06U ||
                (floor_mask & 0x09U) == 0x09U) {
                return 0x33U;
            }
            return kConnectivity[wall_mask];

        default:
            // 009B0 leaves EAX as the raw wall mask for other sub-0x20
            // values. The generated cache in the supplied build uses 1..5.
            return wall_mask;
    }
}

std::uint8_t AutomapPresentationModel::cp437_base_glyph(
    const LevelState& level,
    CellPosition cell) {
    const auto cached = cache_at(level, cell);
    if (cached == 0xFFU) {
        return 0x20U;
    }

    const auto resolved =
        cached < 0x20U ? resolve_cell(level, cell) : cached;
    if (resolved >= 0x20U && resolved <= 0x33U) {
        return kCp437[
            static_cast<std::size_t>(resolved - 0x20U)];
    }

    if (cached < 0x20U) {
        // CODE.2 data byte 0x6030.
        return 0xDBU;
    }

    return 0x20U;
}

std::vector<std::uint8_t> AutomapPresentationModel::cp437_base_map(
    const LevelState& level) {
    const auto expected =
        static_cast<std::size_t>(level.width) *
        static_cast<std::size_t>(level.height);
    if (level.width <= 0 || level.height <= 0 ||
        level.automap_cache.size() != expected) {
        throw std::runtime_error(
            "invalid level geometry for automap presentation");
    }

    std::vector<std::uint8_t> out(expected, 0x20U);
    for (int row = 0; row < level.height; ++row) {
        for (int column = 0; column < level.width; ++column) {
            const CellPosition cell{column, row};
            out[ExplorationWorldModel::cell_index(level, cell)] =
                cp437_base_glyph(level, cell);
        }
    }
    return out;
}

}  // namespace rloft
