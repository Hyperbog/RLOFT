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
    std::uint32_t projected_extent,
    std::uint16_t render_scale_lookup_bias) {
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

    // CODE.2 36D23..36D59: the special-ID branch writes 10450 with
    // (scale*2) + ((scale*2)>>3), but 36D54 shifts EAX, not [10450].
    // The selected block index is scale - (scale>>3) in both branches.
    // Mixing those two variables underflows for ordinary special IDs.
    if (absolute_res3_id >= 0x117U &&
        absolute_res3_id < 0x13BU) {
        const auto doubled =
            out.clamped_scale * 2U;
        out.renderer_phase_adjustment =
            doubled + (doubled >> 3U);
    } else {
        out.renderer_phase_adjustment =
            out.clamped_scale >> 2U;
    }
    out.scale_pointer_index =
        out.clamped_scale - (out.clamped_scale >> 3U);

    // CODE.2 36CFA..36D8C uses the *unclamped* argument-3 size with its
    // low three bits masked. Argument 4 is added to the derived table index.
    // Above C730 the lookup result is zero; values above 0x808 are capped.
    out.lookup_index =
        ((projected_extent & 0xFFF8U) >> 1U) +
        static_cast<std::uint32_t>(
            render_scale_lookup_bias);
    if (out.lookup_index <= tables.runtime_lookup_max) {
        if (out.lookup_index >= tables.runtime_lookup.size()) {
            throw std::out_of_range(
                "type-7 renderer SINTAB lookup is truncated");
        }
        out.lookup_value =
            std::min<std::uint16_t>(
                tables.runtime_lookup[out.lookup_index],
                0x0808U);
    }

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


std::vector<Type7VisibleColumn>
Type7ColumnTraversalModel::visible_columns(
    const Type7ColumnTraversalPlan& plan,
    int screen_x,
    std::uint32_t raw_depth,
    const std::vector<ViewSweepRecord>& rays) {
    std::vector<Type7VisibleColumn> out;
    const auto depth =
        static_cast<std::int32_t>(raw_depth);
    int left_cursor = screen_x;
    int right_cursor = screen_x;

    for (const auto& column : plan.columns) {
        const auto run_width =
            static_cast<int>(column.scale_step);
        int first = 0;
        if (column.side == Type7ColumnSide::Left) {
            // CODE.2 36F2E decrements the destination pointer before
            // emitting the left-hand side. A step of zero emits nothing.
            left_cursor -= run_width;
            first = left_cursor;
        } else {
            // CODE.2 37140..372C3 emits right-hand spans starting at the
            // center and then advances to the next source column.
            first = right_cursor;
            right_cursor += run_width;
        }

        for (int offset = 0;
             offset < run_width;
             ++offset) {
            const auto x = first + offset;
            if (x < 0 ||
                static_cast<std::size_t>(x) >=
                    rays.size()) {
                continue;
            }
            // 36F4E/36FCE/37157/371D7 compare the signed ray depth
            // against argument 6; >= suppresses the source column.
            if (static_cast<std::int32_t>(
                    rays[static_cast<std::size_t>(x)].depth) >=
                depth) {
                continue;
            }
            out.push_back(
                Type7VisibleColumn{
                    column.source_x,
                    column.source_column_index,
                    static_cast<std::size_t>(x),
                });
        }
    }

    return out;
}

}  // namespace rloft
