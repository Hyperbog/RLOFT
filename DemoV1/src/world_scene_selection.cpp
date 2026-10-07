#include "rloft/world_scene_selection.hpp"

#include "rloft/scene_shape_bounds.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <vector>

namespace rloft {

bool WorldSceneSelectionModel::class1_type_is_interactive(
    std::uint16_t object_type_id) noexcept {
    // CODE.2 2AC80 is exactly four equality tests.
    return object_type_id == 0x52U ||
           object_type_id == 0x6EU ||
           object_type_id == 0x71U ||
           object_type_id == 0x91U;
}

std::optional<WorldSceneTarget>
WorldSceneSelectionModel::target_at(
    const std::vector<WorldSceneRecord>& records,
    int row,
    int column,
    std::uint8_t action_flags,
    const GeometryLookup& geometry,
    const Class1Eligibility& class1_eligible) {
    if (!geometry) {
        throw std::invalid_argument(
            "world scene selection requires RES3 geometry lookup");
    }
    if (records.empty()) {
        return std::nullopt;
    }

    // 223F4 initializes ESI to scene_count-1 and resolves the record whose
    // +0x0A rank equals ESI, then decrements the rank only after rejection.
    for (std::size_t reverse_rank = records.size();
         reverse_rank > 0U;
         --reverse_rank) {
        const auto rank =
            static_cast<std::uint16_t>(
                reverse_rank - 1U);

        std::size_t record_index =
            records.size();
        for (std::size_t i = 0;
             i < records.size();
             ++i) {
            if (records[i].render_order == rank) {
                record_index = i;
                break;
            }
        }
        if (record_index == records.size()) {
            continue;
        }

        const auto& record =
            records[record_index];

        if (record.source_class == 3U) {
            continue;
        }
        if (record.source_class == 1U) {
            // 2246C..2248A resolves the live family-B type and calls 2AC80
            // before 21BF0 or any rectangle comparisons.
            if (!class1_eligible ||
                !class1_eligible(record.source_index)) {
                continue;
            }
        } else if (record.source_class == 2U) {
            // A class-2 rectangle can be visually topmost yet ignored when
            // flags bit 0 is clear; 223F4 then continues to the next rank.
            if ((action_flags & 0x01U) == 0U) {
                continue;
            }
        } else if (record.source_class != 0U) {
            throw std::logic_error(
                "unsupported interactive scene source class");
        }

        const auto bounds =
            SceneShapeBoundsModel::interaction_type7(
                record.screen_x,
                record.projected_extent,
                geometry(record.resource_id));
        if (!bounds.valid) {
            continue;
        }

        // 224AA..224E0 performs four inclusive unsigned-word rectangle
        // comparisons with no additional hit padding.
        if (row >= static_cast<int>(bounds.top) &&
            row <= static_cast<int>(bounds.bottom) &&
            column >= static_cast<int>(bounds.left) &&
            column <= static_cast<int>(bounds.right)) {
            return WorldSceneTarget{
                record_index,
                record.source_class,
                record.source_index,
            };
        }
    }

    return std::nullopt;
}

}  // namespace rloft
