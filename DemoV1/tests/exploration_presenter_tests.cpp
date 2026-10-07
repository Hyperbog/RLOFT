#include "rloft/exploration_presenter.hpp"

#include <cassert>

int main() {
    rloft::ExplorationRuntimeFrame frame{};

    rloft::ViewSweepRun fallback{};
    fallback.pass = rloft::SweepRenderPass::Main;
    fallback.first_ray = 0;
    fallback.ray_count = 1;
    fallback.depth = 1024; // extent 512
    fallback.resource_id = 1;

    rloft::ViewSweepRun generated = fallback;
    generated.first_ray = 1;
    generated.depth = 128; // extent 64

    rloft::ViewSweepRun prepass = generated;
    prepass.first_ray = 2;
    prepass.pass = rloft::SweepRenderPass::Prepass;

    rloft::ViewSweepRun zero = generated;
    zero.first_ray = 3;
    zero.depth = 0;

    frame.sweep_runs = {
        fallback,
        generated,
        prepass,
        zero,
    };

    rloft::RendererTables tables{};
    tables.runtime_lookup_max = 508;

    const auto coverage =
        rloft::ExplorationPresenter::surface_coverage(
            frame,
            tables);

    assert(coverage.fallback_main_runs == 1);
    assert(coverage.sintab_main_runs_pending == 1);
    assert(coverage.sintab_prepass_runs_pending == 1);
    assert(!coverage.complete());

    // A scene shape can be fully projected and depth-clipped without
    // being rasterized. The coverage flag must not claim that it is drawn.
    rloft::WorldSceneRenderPlan pending{};
    pending.visible_columns.push_back(
        rloft::Type7VisibleColumn{32U, 0U, 128U});
    pending.visible_columns.push_back(
        rloft::Type7VisibleColumn{32U, 0U, 129U});
    frame.world_scene_render_plans.push_back(pending);

    frame.sweep_runs = {fallback};
    const auto scene_pending =
        rloft::ExplorationPresenter::surface_coverage(
            frame,
            tables);
    assert(scene_pending.sintab_main_runs_pending == 0U);
    assert(scene_pending.sintab_prepass_runs_pending == 0U);
    assert(scene_pending.type7_scene_calls_pending == 1U);
    assert(scene_pending.type7_visible_columns_pending == 2U);
    assert(!scene_pending.complete());

    // Fully occluded calls are not missing visible pixels.
    frame.world_scene_render_plans[0].visible_columns.clear();
    frame.sweep_runs = {fallback};
    const auto complete =
        rloft::ExplorationPresenter::surface_coverage(
            frame,
            tables);
    assert(complete.fallback_main_runs == 1);
    assert(complete.sintab_main_runs_pending == 0);
    assert(complete.sintab_prepass_runs_pending == 0);
    assert(complete.type7_scene_calls_pending == 0);
    assert(complete.type7_visible_columns_pending == 0);
    assert(complete.complete());
}
