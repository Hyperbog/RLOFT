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
        const auto shape = shape_lookup(call.resource_id);
        plan.traversal =
            Type7ColumnTraversalModel::build(
                shape,
                tables,
                call.resource_id,
                call.scale_extent);
        plan.visible_columns =
            Type7ColumnTraversalModel::visible_columns(
                plan.traversal,
                static_cast<int>(call.screen_x),
                call.raw_depth,
                rays);
        out.push_back(std::move(plan));
    }
    return out;
}

}  // namespace rloft
