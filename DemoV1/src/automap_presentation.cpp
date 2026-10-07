#include "rloft/automap_presentation.hpp"

#include <array>
#include <algorithm>
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

namespace {

std::uint16_t read_u16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8U);
}

std::int16_t read_s16(const std::uint8_t* p) noexcept {
    return static_cast<std::int16_t>(read_u16(p));
}

std::int16_t family_b_count(const LevelState& level) noexcept {
    return read_s16(level.header.data() + 0x28E);
}

}  // namespace

std::vector<AutomapExportModel::PlacedRecord>
AutomapExportModel::placed_records(
    const LevelState& level) {
    const auto raw_count = family_b_count(level);
    if (raw_count < 0) {
        throw std::runtime_error(
            "negative family-B count in automap exporter");
    }
    const auto count = static_cast<std::size_t>(raw_count);

    std::vector<PlacedRecord> out;
    out.reserve(count);

    if (level.storage == LevelStorage::ArchiveCompact) {
        constexpr std::size_t kStride = 11U;
        if (level.family_b_raw.size() != count * kStride) {
            throw std::runtime_error(
                "compact family-B bytes disagree with header count");
        }

        for (std::size_t i = 0; i < count; ++i) {
            const auto* record =
                level.family_b_raw.data() + i * kStride;
            out.push_back(PlacedRecord{
                read_s16(record + 2U),
                read_s16(record + 6U),
                read_u16(record + 0U),
            });
        }
        return out;
    }

    constexpr std::size_t kPrefix = 2U;
    constexpr std::size_t kStride = 22U;
    if (level.family_b_raw.size() !=
        kPrefix + count * kStride) {
        throw std::runtime_error(
            "direct family-B bytes disagree with header count");
    }
    if (read_s16(level.family_b_raw.data()) != raw_count) {
        throw std::runtime_error(
            "direct family-B prefix disagrees with header count");
    }

    for (std::size_t i = 0; i < count; ++i) {
        const auto* record =
            level.family_b_raw.data() +
            kPrefix + i * kStride;

        // Runtime/direct layout:
        // +0 y 16.16, +4 x 16.16, +0E object type.
        // 00700 compares the signed high words at +2/+6.
        out.push_back(PlacedRecord{
            read_s16(record + 6U),
            read_s16(record + 2U),
            read_u16(record + 0x0EU),
        });
    }
    return out;
}

std::vector<std::uint8_t>
AutomapExportModel::placed_overlay(
    const LevelState& level,
    int column_index) {
    if (column_index < 0 ||
        column_index >= level.width ||
        level.height <= 0) {
        throw std::out_of_range(
            "automap export column");
    }

    std::vector<std::uint8_t> out(
        static_cast<std::size_t>(level.height),
        0x20U);
    const auto records = placed_records(level);

    for (int row = 0; row < level.height; ++row) {
        const auto cell =
            ExplorationWorldModel::cell_index(
                level, {column_index, row});
        if (cell >= level.automap_cache.size()) {
            throw std::runtime_error(
                "automap cache does not match level dimensions");
        }
        if (level.automap_cache[cell] == 0xFFU) {
            continue;
        }

        for (const auto& record : records) {
            if (record.column != column_index ||
                record.row != row) {
                continue;
            }
            // CODE.2 026E8 writes each match to the same byte; later family-B
            // records therefore win at a shared cell.
            out[static_cast<std::size_t>(row)] =
                record.type >= 0x3CU &&
                        record.type <= 0x41U
                    ? 0x2AU
                    : 0xFEU;
        }
    }
    return out;
}

AutomapExportColumn AutomapExportModel::column(
    const LevelState& level,
    int column_index) {
    if (column_index < 0 ||
        column_index >= level.width ||
        level.height <= 0) {
        throw std::out_of_range(
            "automap export column");
    }

    AutomapExportColumn out{};
    const auto height =
        static_cast<std::size_t>(level.height);
    out.base.assign(height, 0x20U);
    out.placed =
        placed_overlay(level, column_index);
    out.labels.assign(height, 0x20U);

    for (int row = 0; row < level.height; ++row) {
        out.base[static_cast<std::size_t>(row)] =
            AutomapPresentationModel::cp437_base_glyph(
                level, {column_index, row});
    }

    const auto annotations =
        AutomapAnnotationModel::parse(
            level.automap_annotations);

    // This cursor deliberately does not equal the current row once a label is
    // found. It is the exact 026E8 label_ptr progression.
    std::size_t label_cursor = 0;
    for (int row = 0; row < level.height; ++row) {
        const auto label =
            AutomapAnnotationModel::label_at(
                annotations,
                {column_index, row});
        if (label.has_value()) {
            if (label_cursor < height) {
                out.labels[label_cursor] =
                    static_cast<std::uint8_t>(
                        label->first);
            } else {
                out.label_write_overflow = true;
            }

            ++label_cursor;
            if (label_cursor < height) {
                out.labels[label_cursor] =
                    static_cast<std::uint8_t>(
                        label->second);
            } else {
                out.label_write_overflow = true;
            }

            ++label_cursor;
        }
        ++label_cursor;
    }

    out.merged = out.base;
    for (std::size_t i = 0; i < height; ++i) {
        if (out.placed[i] != 0x20U) {
            out.merged[i] = out.placed[i];
        }
        if (out.labels[i] != 0x20U) {
            out.merged[i] = out.labels[i];
        }
    }
    return out;
}

std::vector<AutomapExportColumn>
AutomapExportModel::columns(
    const LevelState& level) {
    if (level.width <= 0 || level.height <= 0) {
        throw std::runtime_error(
            "invalid level geometry for automap export");
    }

    std::vector<AutomapExportColumn> out;
    out.reserve(
        static_cast<std::size_t>(level.width));
    for (int column_index = 0;
         column_index < level.width;
         ++column_index) {
        out.push_back(
            column(level, column_index));
    }
    return out;
}

}  // namespace rloft
