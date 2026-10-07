#include "rloft/world_scene_render_plan.hpp"

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {

std::vector<WorldSceneRenderPlan>
WorldSceneRenderPlanModel::build(
    const std::vector<WorldSceneDrawCall>& calls,
    const RendererTables& tables,
    const std::vector<ViewSweepRecord>& rays,
    const ShapeLookup& shape_lookup) {
    if (!calls.empty() && !shape_lookup) {
        throw std::invalid_argument(
            "world scene rendering requires original RES3 shape lookup");
    }

    std::vector<WorldSceneRenderPlan> out;
    out.reserve(calls.size());
    for (const auto& call : calls) {
        WorldSceneRenderPlan plan{};
        plan.call = call;
        const auto& shape = shape_lookup(call.resource_id);
        plan.traversal =
            Type7ColumnTraversalModel::build(
                shape,
                tables,
                call.resource_id,
                call.scale_extent,
                call.render_scale_lookup_bias);
        plan.rasterizer_setup =
            Type7RasterizerSetupModel::build(
                plan.traversal,
                tables,
                call.render_mode);
        if (!plan.rasterizer_setup.drawable) {
            out.push_back(std::move(plan));
            continue;
        }
        plan.visible_runs =
            Type7ColumnTraversalModel::visible_runs(
                plan.traversal,
                static_cast<int>(call.screen_x),
                call.raw_depth,
                rays);
        // Keep the exact 36B14 dispatch grouping, but expand each accepted
        // run into individual destinations for the descriptor/span planner.
        for (const auto& run : plan.visible_runs) {
            for (std::size_t i = 0U; i < run.width; ++i) {
                plan.visible_columns.push_back(
                    Type7VisibleColumn{
                        run.source_x,
                        run.source_column_index,
                        run.viewport_x + i,
                    });
            }
        }
        plan.microcode_draws =
            Type7MicrocodePlanModel::build(
                shape,
                plan.traversal,
                plan.visible_columns,
                tables);
        out.push_back(std::move(plan));
    }
    return out;
}

}  // namespace rloft
