#include "rloft/type7_column_traversal.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {

Type7ColumnTraversalPlan Type7ColumnTraversalModel::build(
    const Type7SceneShape& shape,
    const RendererTables& tables,
    std::uint16_t absolute_res3_id,
    std::uint32_t projected_extent) {
    const auto& geometry = shape.geometry;
    const auto width =
        static_cast<std::size_t>(geometry.source_width);
    if ((width != 64U && width != 96U && width != 128U) ||
        geometry.x_min < 0 ||
        geometry.x_max < geometry.x_min ||
        static_cast<std::size_t>(geometry.x_max) >= width ||
        shape.columns.size() !=
            static_cast<std::size_t>(
                geometry.x_max - geometry.x_min + 1)) {
        throw std::invalid_argument(
            "invalid type-7 column traversal geometry");
    }

    Type7ColumnTraversalPlan out{};
    out.initial_scale = projected_extent >> 3U;

    // CODE.2 36D07 returns before reading SINTAB when size / 8 == 0.
    if (out.initial_scale == 0U) {
        return out;
    }
    if (tables.scale_block_count == 0U ||
        tables.scale_blocks.size() !=
            static_cast<std::size_t>(
                tables.scale_block_count)) {
        throw std::invalid_argument(
            "type-7 traversal requires complete SINTAB scale blocks");
    }

    // 36D13..36D23 clamps the incoming extent / 8 to C738.
    out.clamped_scale = std::min(
        out.initial_scale,
        tables.scale_block_count);

    // 36D23..36D59 distinguishes the resource-ID range 0x117..0x13A.
    // Keep the executable's unsigned 32-bit subtraction here. A wrapped
    // pointer index is not safe to access through a portable vector.
    std::uint32_t adjustment{};
    if (absolute_res3_id >= 0x117U &&
        absolute_res3_id < 0x13BU) {
        const auto doubled =
            out.clamped_scale * 2U;
        adjustment = doubled + (doubled >> 3U);
    } else {
        adjustment = out.clamped_scale >> 2U;
    }
    out.scale_pointer_index =
        out.clamped_scale - (adjustment >> 1U);

    // The original pointer array aliases entry 0 to block 0. Entries 1..N
    // reference physical blocks 0..N-1 (SINTAB loader 3838..3A63).
    if (out.scale_pointer_index >
        tables.scale_block_count) {
        throw std::out_of_range(
            "type-7 scale pointer leaves the SINTAB block table");
    }
    out.scale_block_index =
        out.scale_pointer_index == 0U
            ? 0U
            : static_cast<std::size_t>(
                out.scale_pointer_index - 1U);

    const auto& block =
        tables.scale_blocks.at(
            out.scale_block_index);
    const auto half = width / 2U;

    const auto append =
        [&out, &shape, &block](
            Type7ColumnSide side,
            std::size_t x) {
            const auto at =
                x - static_cast<std::size_t>(
                    shape.geometry.x_min);
            const auto& column =
                shape.columns.at(at);
            if (column.source_x !=
                static_cast<std::int16_t>(x)) {
                throw std::invalid_argument(
                    "type-7 column order is inconsistent");
            }
            out.columns.push_back(
                Type7ColumnStep{
                    side,
                    static_cast<std::uint16_t>(x),
                    at,
                    column.list_offset,
                    block.scale_step.at(x),
                });
        };

    // CODE.2 36EBD..370AE starts at the source center minus one and
    // traverses the left-hand pointer table in descending source-X order.
    if (static_cast<std::size_t>(geometry.x_min) < half) {
        for (std::size_t x =
                 std::min(
                     half - 1U,
                     static_cast<std::size_t>(
                         geometry.x_max)) + 1U;
             x-- >
                 static_cast<std::size_t>(
                     geometry.x_min);) {
            append(Type7ColumnSide::Left, x);
        }
    }

    // 370B6..372C3 restarts at the center (or the stored x_min when it lies
    // to the right) and advances source-X in ascending pointer-table order.
    for (std::size_t x =
             std::max(
                 half,
                 static_cast<std::size_t>(
                     geometry.x_min));
         x <= static_cast<std::size_t>(
             geometry.x_max);
         ++x) {
        append(Type7ColumnSide::Right, x);
    }

    return out;
}

}  // namespace rloft
