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



std::vector<Type7VisibleRun>
Type7ColumnTraversalModel::visible_runs(
    const Type7ColumnTraversalPlan& plan,
    int screen_x,
    std::uint32_t raw_depth,
    const std::vector<ViewSweepRecord>& rays) {
    std::vector<Type7VisibleRun> runs;
    // CODE.2's C6D8 horizontal viewport dimension is 256 in SINTAB.
    // The supplied ray buffer defines the available viewport domain.
    const auto width =
        static_cast<std::int64_t>(rays.size());
    const auto depth =
        static_cast<std::int32_t>(raw_depth);
    std::int64_t left_cursor = screen_x;
    std::int64_t right_cursor = screen_x;
    bool left_pass_finished = false;

    const auto front_of_wall =
        [&rays, depth](std::int64_t x) {
            return static_cast<std::int32_t>(
                rays[static_cast<std::size_t>(x)].depth) < depth;
        };
    const auto push =
        [&runs](const Type7ColumnStep& source,
                std::int64_t x,
                std::int64_t count) {
            if (count <= 0) {
                return;
            }
            runs.push_back(Type7VisibleRun{
                source.source_x,
                source.source_column_index,
                static_cast<std::size_t>(x),
                static_cast<std::uint16_t>(count),
            });
        };

    for (const auto& column : plan.columns) {
        if (column.side == Type7ColumnSide::Left &&
            left_pass_finished) {
            continue;
        }
        std::int64_t count = column.scale_step;
        if (count == 0) {
            continue;
        }

        if (column.side == Type7ColumnSide::Left) {
            if (count == 1) {
                // 36F2E decrements 105E3 before the signed ray compare.
                --left_cursor;
                if (left_cursor >= 0 &&
                    left_cursor < width &&
                    front_of_wall(left_cursor)) {
                    push(column, left_cursor, 1);
                }
                continue;
            }
            // 36F6F..36FBE trims a left run against the viewport before
            // probing its first and last wall-ray depths.
            if (left_cursor <= width) {
                if (count > left_cursor) {
                    count = left_cursor;
                }
                left_cursor -= count;
            } else {
                left_cursor -= count;
                count = width - left_cursor;
            }
            if (count <= 0 ||
                left_cursor < 0 ||
                left_cursor >= width) {
                continue;
            }

            const auto first = left_cursor;
            const auto last = first + count - 1;
            if (last >= width) {
                continue;
            }
            const bool first_visible = front_of_wall(first);
            const bool last_visible = front_of_wall(last);
            if (first_visible && last_visible) {
                push(column, first, count);
            } else if (first_visible) {
                // 3701A..37047 removes the blocked *right-hand* suffix,
                // checking the new endpoint until it becomes visible.
                while (count > 0 &&
                       !front_of_wall(first + count - 1)) {
                    --count;
                }
                push(column, first, count);
            } else if (last_visible) {
                // 3705B..37085 removes the blocked left-hand prefix and
                // mutates the horizontal cursor for the next source x.
                while (count > 0 &&
                       !front_of_wall(left_cursor)) {
                    ++left_cursor;
                    --count;
                }
                push(column, left_cursor, count);
                // CODE.2 3708F branches to 370AE, not 37094:
                // the left traversal ends after drawing this suffix.
                // Further left-side source columns are not visited,
                // but CODE.2 resumes at 370AE and enters the RIGHT pass.
                left_pass_finished = true;
                continue;
            }
            // 37056 explicitly skips a run with *both* endpoints blocked,
            // even when its interior happens to be visible.
            continue;
        }

        // Right-hand pass, 37140..3729B. The source-column cursor
        // advances by the final clipped run width, including blocked runs.
        auto& x = right_cursor;
        if (count == 1) {
            if (x >= 0 &&
                x < width &&
                front_of_wall(x)) {
                push(column, x, 1);
            }
            ++x;
            continue;
        }
        if (x < 0) {
            if (-x >= count) {
                x += count;
                continue;
            }
            count += x;
            x = 0;
        } else if (x + count > width) {
            count = width - x;
        }
        if (count <= 0 ||
            x < 0 ||
            x >= width ||
            x + count > width) {
            x += count;
            continue;
        }

        const auto last = x + count - 1;
        const bool first_visible = front_of_wall(x);
        const bool last_visible = front_of_wall(last);
        if (first_visible && last_visible) {
            push(column, x, count);
        } else if (first_visible) {
            while (count > 0 &&
                   !front_of_wall(x + count - 1)) {
                --count;
            }
            push(column, x, count);
        } else if (last_visible) {
            while (count > 0 &&
                   !front_of_wall(x)) {
                ++x;
                --count;
            }
            push(column, x, count);
        }
        x += count;
    }
    return runs;
}

std::vector<Type7VisibleColumn>
Type7ColumnTraversalModel::visible_columns(
    const Type7ColumnTraversalPlan& plan,
    int screen_x,
    std::uint32_t raw_depth,
    const std::vector<ViewSweepRecord>& rays) {
    std::vector<Type7VisibleColumn> out;
    const auto runs =
        visible_runs(plan, screen_x, raw_depth, rays);
    for (const auto& run : runs) {
        for (std::size_t i = 0;
             i < run.width;
             ++i) {
            out.push_back(Type7VisibleColumn{
                run.source_x,
                run.source_column_index,
                run.viewport_x + i,
            });
        }
    }
    return out;
}

}  // namespace rloft
