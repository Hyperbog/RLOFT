#ifdef NDEBUG
#undef NDEBUG
#endif
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
    frame.scene_vga_trace.emplace();
    frame.scene_vga_trace->unresolved_wide_runs = 1U;

    frame.sweep_runs = {fallback};
    const auto scene_pending =
        rloft::ExplorationPresenter::surface_coverage(
            frame,
            tables);
    assert(scene_pending.sintab_main_runs_pending == 0U);
    assert(scene_pending.sintab_prepass_runs_pending == 0U);
    assert(scene_pending.type7_scene_calls_pending == 1U);
    assert(scene_pending.type7_visible_columns_pending == 2U);
    assert(scene_pending.type7_unvalidated_wide_runs == 1U);
    assert(!scene_pending.complete());

    // Fully occluded calls are not missing visible pixels.
    frame.world_scene_render_plans[0].visible_columns.clear();
    frame.scene_vga_trace->unresolved_wide_runs = 0U;
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
    assert(complete.type7_unvalidated_wide_runs == 0U);
    assert(complete.complete());

    // An unsupported SINTAB plan cannot be reported as a complete
    // frame even if a synthetic caller supplies zero pending work.
    auto failed_prepass = complete;
    failed_prepass.sintab_prepass_runs_failed = 1U;
    assert(!failed_prepass.complete());
    failed_prepass.sintab_prepass_runs_failed = 0U;
    failed_prepass.sintab_prepass_runs_rasterized = 1U;
    failed_prepass.sintab_prepass_indexed_writes = 2U;
    assert(failed_prepass.complete());

    // A failed ordinary wall raster remains incomplete even if its
    // original run was attempted and its renderer reported some writes.
    auto failed_main = complete;
    failed_main.sintab_main_runs_failed = 1U;
    failed_main.sintab_main_runs_rasterized = 1U;
    failed_main.sintab_main_indexed_writes = 3U;
    assert(!failed_main.complete());
    failed_main.sintab_main_runs_failed = 0U;
    assert(failed_main.complete());

    // A RES3 source that fails before projection has unknown visibility.
    // It must not be reported as a completely occluded or complete frame.
    auto failed_plan_frame=frame;
    failed_plan_frame.scene_plan_failures.push_back(
        rloft::WorldSceneRenderPlanFailure{
            0U,1U,17U,4020U,"original RES3 unavailable"
        });
    const auto bad_plan_coverage=
        rloft::ExplorationPresenter::surface_coverage(
            failed_plan_frame,tables);
    assert(bad_plan_coverage.type7_plan_calls_failed==1U);
    assert(bad_plan_coverage.type7_scene_calls_pending==0U);
    assert(!bad_plan_coverage.complete());

    // Fail closed even when a synthetic caller omits the diagnostic list.
    auto orphan_plan=frame;
    orphan_plan.world_scene_render_plans[0].source_unresolved=true;
    const auto orphan_coverage=
        rloft::ExplorationPresenter::surface_coverage(
            orphan_plan,tables);
    assert(orphan_coverage.type7_plan_calls_failed==1U);
    assert(!orphan_coverage.complete());

    auto bad_raster=complete;
    bad_raster.type7_calls_failed=1U;
    assert(!bad_raster.complete());

    // A renderer-level failure is distinct from a trace that was never
    // produced and a source fully occluded by original depth records.
    auto scene_statuses=frame;
    rloft::WorldSceneRenderPlan unplanned{};
    unplanned.source_unresolved=true;
    rloft::WorldSceneRenderPlan hidden{};
    rloft::WorldSceneRenderPlan projected{};
    projected.visible_columns.push_back(
        rloft::Type7VisibleColumn{32U,0U,128U});
    scene_statuses.world_scene_render_plans={
        unplanned,hidden,projected
    };
    const auto classified=
        rloft::ExplorationPresenter::initial_scene_outcomes(
            scene_statuses);
    assert(classified.size()==3U);
    assert(classified[0]==
           rloft::Type7SceneRenderOutcome::PlanningFailed);
    assert(classified[1]==
           rloft::Type7SceneRenderOutcome::FullyOccluded);
    assert(classified[2]==
           rloft::Type7SceneRenderOutcome::TraceUnavailable);

    // CODE.2 335C8 initializes 22220=0x800, but the four
    // materializer tails explicitly reset it to zero for source
    // main resource 0x93. Both 1F2A5's initial sentinel shortcut
    // and the 15618[0]=0 dynamic lookup compute lower FF50=128.
    // Preserve the original full 34A74 raster at y259..132 only
    // under the already authenticated source-active no-wall gate,
    // never for arbitrary dynamic depths or missing RES4 bytes.
    rloft::ExplorationRuntimeFrame outdoors{};
    outdoors.source_level_id=22;
    outdoors.ground_source_verified=true;
    outdoors.ground_perspective_ready=true;
    outdoors.ground_raster_source.assign(4098U,0U);
    outdoors.sweep_records.resize(256U);
    for(auto& ray:outdoors.sweep_records) {
        ray.resource_inner=0x93U;
        ray.resource_outer=0x93U;
    }
    assert(rloft::ExplorationPresenter::original_full_ground_eligible(
        outdoors));
    outdoors.ground_raster_source.pop_back();
    assert(!rloft::ExplorationPresenter::original_full_ground_eligible(
        outdoors));
    outdoors.ground_raster_source.push_back(0U);
    // The source only reads the alternate 6F/outer bank for an
    // active +0C==1 prepass. A dormant alternate 19 cannot block
    // 34A74 when every actual 336F7 main ray is sentinel 0x93.
    outdoors.sweep_records[17U].resource_outer=19U;
    outdoors.sweep_records[17U].prepass=0U;
    assert(rloft::ExplorationPresenter::original_full_ground_eligible(
        outdoors));
    // The same drawable alternate is unsafe if its prepass is ACTIVE.
    outdoors.sweep_records[17U].prepass=1U;
    assert(!rloft::ExplorationPresenter::original_full_ground_eligible(
        outdoors));
    outdoors.sweep_records[17U].resource_outer=0x93U;
    assert(rloft::ExplorationPresenter::original_full_ground_eligible(
        outdoors));
    // An unknown source prepass state cannot establish no-wall proof.
    outdoors.sweep_records[17U].prepass=2U;
    assert(!rloft::ExplorationPresenter::original_full_ground_eligible(
        outdoors));
    outdoors.sweep_records[17U].prepass=0U;
    outdoors.sweep_records[29U].resource_inner=73U;
    assert(!rloft::ExplorationPresenter::original_full_ground_eligible(
        outdoors));
    outdoors.sweep_records[29U].resource_inner=0x93U;
    outdoors.sweep_records.pop_back();
    assert(!rloft::ExplorationPresenter::original_full_ground_eligible(
        outdoors));
    outdoors.sweep_records.emplace_back();
    outdoors.sweep_records.back().resource_inner=0x93U;
    outdoors.sweep_records.back().resource_outer=0x93U;
    outdoors.ground_source_verified=false;
    assert(!rloft::ExplorationPresenter::original_full_ground_eligible(
        outdoors));
    outdoors.ground_source_verified=true;
    // All original levels share the same source-derived RES4 selector
    // logic. Eligibility depends on authenticated current-level
    // ground/rays, not the level's authored numeric identifier.
    for (const std::uint32_t level : {0U, 21U, 22U, 31U}) {
        outdoors.source_level_id=level;
        assert(rloft::ExplorationPresenter::original_full_ground_eligible(
            outdoors));
    }
    outdoors.source_level_id=21U;
    outdoors.sweep_records[29U].resource_inner=19U;
    assert(!rloft::ExplorationPresenter::original_full_ground_eligible(
        outdoors));
    outdoors.sweep_records[29U].resource_inner=0x93U;
    outdoors.ground_source_verified=false;
    assert(!rloft::ExplorationPresenter::original_full_ground_eligible(
        outdoors));

    // Clean frames with no RES3 source or raster failure remain complete.
    assert(complete.complete());
}
