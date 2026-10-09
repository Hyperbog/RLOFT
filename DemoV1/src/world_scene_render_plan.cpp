#include "rloft/world_scene_render_plan.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {

void require_lookup(
    const std::vector<WorldSceneDrawCall>& calls,
    const WorldSceneRenderPlanModel::ShapeLookup& shape_lookup) {
    if (!calls.empty() && !shape_lookup) {
        throw std::invalid_argument(
            "world scene rendering requires original RES3 shape lookup");
    }
}

// Each original CODE.2 36CEC scene call is independently derived from its
// RES3 shape, current wall-ray occlusion records and original scale table.
// Keeping this helper shared ensures strict and native recovery paths
// cannot silently diverge on a valid source.
WorldSceneRenderPlan build_one(
    const WorldSceneDrawCall& call,
    const RendererTables& tables,
    const std::vector<ViewSweepRecord>& rays,
    const WorldSceneRenderPlanModel::ShapeLookup& shape_lookup) {
    WorldSceneRenderPlan plan{};
    plan.call = call;
    const auto& shape = shape_lookup(call.resource_id);
    plan.traversal = Type7ColumnTraversalModel::build(
        shape, tables, call.resource_id, call.scale_extent,
        call.render_scale_lookup_bias);
    plan.rasterizer_setup = Type7RasterizerSetupModel::build(
        plan.traversal, tables, call.render_mode);
    if (!plan.rasterizer_setup.drawable) {
        return plan;
    }
    plan.visible_runs = Type7ColumnTraversalModel::visible_runs(
        plan.traversal, static_cast<int>(call.screen_x),
        call.raw_depth, rays);
    // Original 36B14 run grouping is retained. Descriptor planning needs
    // the complete individually projected destination coordinates.
    for (const auto& run : plan.visible_runs) {
        for (std::size_t i=0U; i<run.width; ++i) {
            plan.visible_columns.push_back(Type7VisibleColumn{
                run.source_x, run.source_column_index,
                run.viewport_x+i,
            });
        }
    }
    plan.microcode_draws = Type7MicrocodePlanModel::build(
        shape, plan.traversal, plan.visible_columns, tables);
    return plan;
}

} // namespace

std::vector<WorldSceneRenderPlan>
WorldSceneRenderPlanModel::build(
    const std::vector<WorldSceneDrawCall>& calls,
    const RendererTables& tables,
    const std::vector<ViewSweepRecord>& rays,
    const ShapeLookup& shape_lookup) {
    require_lookup(calls,shape_lookup);
    std::vector<WorldSceneRenderPlan> out;
    out.reserve(calls.size());
    for(const auto& call:calls) {
        out.push_back(build_one(call,tables,rays,shape_lookup));
    }
    return out;
}

WorldSceneRenderPlanAttempt WorldSceneRenderPlanModel::try_build(
    const std::vector<WorldSceneDrawCall>& calls,
    const RendererTables& tables,
    const std::vector<ViewSweepRecord>& rays,
    const ShapeLookup& shape_lookup) {
    require_lookup(calls,shape_lookup);
    WorldSceneRenderPlanAttempt out{};
    out.plans.reserve(calls.size());
    for (std::size_t index=0U; index<calls.size(); ++index) {
        const auto& call=calls[index];
        try {
            out.plans.push_back(build_one(
                call,tables,rays,shape_lookup));
        } catch(const std::exception& error) {
            // Preserve the exact CODE.2 draw-call order, resource and
            // interaction source. A placeholder is NOT a hidden sprite.
            WorldSceneRenderPlan unresolved{};
            unresolved.call=call;
            unresolved.source_unresolved=true;
            out.plans.push_back(std::move(unresolved));
            out.failures.push_back(WorldSceneRenderPlanFailure{
                index, call.source_class, call.source_index,
                call.resource_id, error.what(),
            });
        }
    }
    return out;
}

} // namespace rloft
