#include "rloft/type7_column_traversal.hpp"

#include <cassert>
#include <cstdint>
#include <vector>

namespace {

rloft::Type7ColumnTraversalPlan right_plan(bool next = false) {
    rloft::Type7ColumnTraversalPlan p{};
    p.columns.push_back({
        rloft::Type7ColumnSide::Right, 32U, 0U, 120U, 3U
    });
    if (next) {
        p.columns.push_back({
            rloft::Type7ColumnSide::Right, 33U, 1U, 122U, 1U
        });
    }
    return p;
}

std::vector<rloft::ViewSweepRecord> walls() {
    std::vector<rloft::ViewSweepRecord> rays(256U);
    for (auto& ray : rays) ray.depth = 500U;
    return rays;
}

} // namespace

int main() {
    // 37001..37018: endpoints are both in front of walls, so a single
    // grouped 36B14 call covers *all three columns* even though the
    // middle destination ray is occluded. The previous pixelwise filter
    // incorrectly omitted the middle column.
    auto rays = walls();
    rays[128].depth = 499U;
    rays[129].depth = 500U;
    rays[130].depth = 499U;
    auto runs =
        rloft::Type7ColumnTraversalModel::visible_runs(
            right_plan(), 128, 500U, rays);
    assert(runs.size() == 1U);
    assert(runs[0].viewport_x == 128U);
    assert(runs[0].width == 3U);
    auto expanded =
        rloft::Type7ColumnTraversalModel::visible_columns(
            right_plan(), 128, 500U, rays);
    assert(expanded.size() == 3U);
    assert(expanded[1].viewport_x == 129U);

    // 3720A..3725F: both endpoints occluded suppress the entire run,
    // even if a middle ray has an unblocked depth.
    rays = walls();
    rays[129].depth = 499U;
    assert(rloft::Type7ColumnTraversalModel::visible_runs(
        right_plan(), 128, 500U, rays).empty());

    // Right endpoint occluded: trim a suffix, then advance the right
    // source-column cursor by the *new* width (not original scale_step).
    rays = walls();
    rays[128].depth = 499U;
    rays[129].depth = 499U;
    runs = rloft::Type7ColumnTraversalModel::visible_runs(
        right_plan(true), 128, 500U, rays);
    assert(runs.size() == 1U);
    assert(runs[0].viewport_x == 128U);
    assert(runs[0].width == 2U);

    // Left endpoint occluded: advance to first unblocked pixel and keep
    // the visible suffix. The following source column begins after it.
    rays = walls();
    rays[129].depth = 499U;
    rays[130].depth = 499U;
    rays[131].depth = 499U;
    runs = rloft::Type7ColumnTraversalModel::visible_runs(
        right_plan(true), 128, 500U, rays);
    assert(runs.size() == 2U);
    assert(runs[0].viewport_x == 129U);
    assert(runs[0].width == 2U);
    assert(runs[1].viewport_x == 131U);
    assert(runs[1].width == 1U);

    // 36F6F..37094: on the left side, source column traversal proceeds
    // toward smaller X; a 3-column group begins at screen_x - 3.
    rloft::Type7ColumnTraversalPlan left{};
    left.columns.push_back({
        rloft::Type7ColumnSide::Left, 31U, 0U, 90U, 3U
    });
    left.columns.push_back({
        rloft::Type7ColumnSide::Left, 30U, 1U, 88U, 1U
    });
    rays = walls();
    rays[125].depth = 499U;
    rays[126].depth = 500U;
    rays[127].depth = 499U;
    rays[124].depth = 499U;
    runs = rloft::Type7ColumnTraversalModel::visible_runs(
        left, 128, 500U, rays);
    assert(runs.size() == 2U);
    assert(runs[0].viewport_x == 125U);
    assert(runs[0].width == 3U);
    assert(runs[1].viewport_x == 124U);
    assert(runs[1].width == 1U);

    // 3705B..3708F: if only the last endpoint is drawable, CODE.2
    // trims the blocked left prefix, draws that suffix and jumps
    // directly to 370AE. It does NOT process the next left-side column.
    rays = walls();
    rays[125].depth = 500U;
    rays[126].depth = 499U;
    rays[127].depth = 499U;
    rays[124].depth = 499U; // Would draw if left traversal continued.
    runs = rloft::Type7ColumnTraversalModel::visible_runs(
        left, 128, 500U, rays);
    assert(runs.size() == 1U);
    assert(runs[0].source_x == 31U);
    assert(runs[0].viewport_x == 126U);
    assert(runs[0].width == 2U);

    // The last right-hand source span is clipped to the viewport edge.
    rays = walls();
    rays[254].depth = 0U;
    rays[255].depth = 0U;
    runs = rloft::Type7ColumnTraversalModel::visible_runs(
        right_plan(), 254, 500U, rays);
    assert(runs.size() == 1U);
    assert(runs[0].viewport_x == 254U);
    assert(runs[0].width == 2U);

    assert(rloft::Type7ColumnTraversalModel::visible_runs(
        right_plan(), 128, 500U, {}).empty());
}
