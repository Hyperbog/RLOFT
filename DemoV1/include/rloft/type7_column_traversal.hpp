#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/renderer_tables.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/view_sweep_record.hpp"

namespace rloft {

enum class Type7ColumnSide : std::uint8_t {
    Left,
    Right,
};

// The CODE.2 36CEC traversal consumes type-7 display lists in this order.
// A scale_step value is a horizontal destination-column count, not a
// substitute for the separate vertical SINTAB-generated rasterizer.
struct Type7ColumnStep final {
    Type7ColumnSide side{Type7ColumnSide::Right};
    std::uint16_t source_x{};
    std::size_t source_column_index{};
    std::uint16_t display_list_offset{};
    std::uint16_t scale_step{};
};

struct Type7ColumnTraversalPlan final {
    std::uint32_t initial_scale{};
    std::uint32_t clamped_scale{};
    std::uint32_t scale_pointer_index{};
    // DGROUP 10450: horizontal/vertical renderer phase correction.
    // It is separate from scale_pointer_index (CODE.2 36D34..36D59).
    std::uint32_t renderer_phase_adjustment{};
    // 36CFA..36D8C: the SINTAB runtime lookup is indexed before drawing.
    std::uint32_t lookup_index{};
    std::uint16_t lookup_value{};
    std::size_t scale_block_index{};
    std::vector<Type7ColumnStep> columns;
};

struct Type7VisibleColumn final {
    std::uint16_t source_x{};
    std::size_t source_column_index{};
    std::size_t viewport_x{};
};

// A single CODE.2 36B14 invocation after edge clipping. The DOS renderer
// tests each run's first and last ray, then either draws the entire run,
// trims an occluded edge, or omits it. This is NOT per-pixel depth rejection.
struct Type7VisibleRun final {
    std::uint16_t source_x{};
    std::size_t source_column_index{};
    std::size_t viewport_x{};
    std::uint16_t width{};
};

class Type7ColumnTraversalModel final {
public:
    // Models the scale-pointer selection and left/right per-column traversal
    // at CODE.2 36CEC..372F3. Does not execute 369D8 micro-rasterizer code,
    // perform its temporary patches, or substitute approximate depth clipping.
    [[nodiscard]] static Type7ColumnTraversalPlan build(
        const Type7SceneShape& shape,
        const RendererTables& tables,
        std::uint16_t absolute_res3_id,
        std::uint32_t projected_extent,
        std::uint16_t render_scale_lookup_bias = 0U);

    // CODE.2 36F2E..3729B contiguous spans, signed wall-depth endpoint
    // tests, single-column gates and side-specific cursor advances.
    // Only existing original drawable spans are returned.
    [[nodiscard]] static std::vector<Type7VisibleRun> visible_runs(
        const Type7ColumnTraversalPlan& plan,
        int screen_x,
        std::uint32_t raw_depth,
        const std::vector<ViewSweepRecord>& rays);

    // Expands the original accepted runs for the later vertical descriptor
    // planner, without changing their draw-call grouping or precedence.
    [[nodiscard]] static std::vector<Type7VisibleColumn> visible_columns(
        const Type7ColumnTraversalPlan& plan,
        int screen_x,
        std::uint32_t raw_depth,
        const std::vector<ViewSweepRecord>& rays);
};

}  // namespace rloft
