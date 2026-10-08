#include "rloft/exploration_presenter.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <utility>

namespace rloft {

ExplorationPresenter::ExplorationPresenter(
    const RavenloftBootAssets& assets)
    : assets_(assets),
      hud_(assets),
      walls_(assets.source()),
      fill_table_(
          SurfaceFillTable::from_code2(
              assets.source().read("CODE.2"))),
      surface_vga_masks_(
          Type7VgaPlaneMaskModel::load(assets.source())),
      res0_raster_base_(assets.init_u16(0x00U)),
      res1_portrait_base_(assets.init_u16(0x04U)) {}

ExplorationSurfaceCoverage
ExplorationPresenter::surface_coverage(
    const ExplorationRuntimeFrame& frame,
    const RendererTables& tables) {
    ExplorationSurfaceCoverage out{};

    for (const auto& run : frame.sweep_runs) {
        if (!run.drawable()) {
            continue;
        }

        const auto extent =
            (run.depth & 0xFFF8U) >> 1U;

        if (run.pass == SweepRenderPass::Prepass) {
            if (extent != 0U) {
                ++out.sintab_prepass_runs_pending;
            }
            continue;
        }

        if (extent == 0U) {
            continue;
        }

        if (extent <= tables.runtime_lookup_max) {
            ++out.sintab_main_runs_pending;
        } else {
            ++out.fallback_main_runs;
        }
    }

    if (frame.scene_vga_trace.has_value()) {
        out.type7_unvalidated_wide_runs =
            frame.scene_vga_trace->unresolved_wide_runs;
    }
    // This static pre-presentation estimate marks every visible type-7
    // scene call pending. present() subtracts successful canonical SINTAB
    // rasterizations and reports any transactional draw failure separately.
    // Fully occluded calls never contribute missing pixels.
    for (const auto& plan : frame.world_scene_render_plans) {
        if (plan.visible_columns.empty()) {
            continue;
        }
        ++out.type7_scene_calls_pending;
        out.type7_visible_columns_pending +=
            plan.visible_columns.size();
    }

    return out;
}

void ExplorationPresenter::draw_environment(
    const ExplorationRuntimeFrame& frame) {
    if (!frame.scene.environment_commands.empty()) {
        for (const auto& command :
             frame.scene.environment_commands) {
            const auto raster =
                assets_.type1(
                    8,
                    command.resource_id);
            EnvironmentPresenter::draw_subtracted(
                framebuffer_,
                raster,
                0,
                command.y,
                command.x,
                command.shade,
                command.kind ==
                    EnvironmentDrawKind::
                        ProfilePlacement);
        }
        return;
    }

    SurfaceFillModel::draw_upper(
        framebuffer_,
        fill_table_,
        frame.scene.frame.renderer_fill_base_a);
}

ExplorationPresentedFrame ExplorationPresenter::present(
    const ExplorationRuntimeFrame& frame,
    const RendererTables& tables,
    int presentation_mode) {
    framebuffer_.clear(0);

    // The HUD frame is persistent in CODE.2; drawing it first establishes the
    // same opaque side/lower chrome before the clipped world fields are filled.
    hud_.draw_base(
        framebuffer_,
        presentation_mode);

    draw_environment(frame);

    SurfaceFillModel::draw_lower(
        framebuffer_,
        fill_table_,
        frame.scene.frame.renderer_fill_base_b);

    auto output_coverage =
        surface_coverage(frame, tables);

    // CODE.2 335DC first renders any valid extra surface prepass,
    // before 336F7 performs the ordinary wall pass. The prepass calls
    // 32620 with the actual RES4 texture column and original SINTAB
    // generated code. Unsupported original memory geometry, unavailable
    // resources and invalid selectors remain explicitly PENDING.
    std::vector<SurfacePrepassRasterFailure> prepass_failures;
    if (!frame.prepass_plans.empty()) {
        VgaPlanarMemory prepass_video;
        prepass_video.import_page(framebuffer_, frame.scene_video_page);
        bool any_prepass_rasterized = false;
        for (const auto& plan : frame.prepass_plans) {
            try {
                const auto matches_original_run = std::any_of(
                    frame.sweep_runs.begin(),
                    frame.sweep_runs.end(),
                    [&](const ViewSweepRun& run) {
                        return run.pass == SweepRenderPass::Prepass &&
                               run.drawable() &&
                               run.first_ray == plan.first_ray &&
                               run.ray_count == plan.ray_count &&
                               run.resource_id == plan.resource_id &&
                               run.texture_column_index ==
                                   plan.texture_column_index &&
                               run.texture_source_byte_offset ==
                                   plan.texture_source_byte_offset &&
                               ((run.depth & 0xFFF8U) >> 1U) ==
                                   plan.extent;
                    });
                if (!matches_original_run) {
                    throw std::invalid_argument(
                        "surface prepass plan has no matching sweep run");
                }
                auto cached =
                    surface_texture_cache_.find(plan.resource_id);
                if (cached == surface_texture_cache_.end()) {
                    auto texture =
                        walls_.texture(static_cast<int>(
                            plan.resource_id));
                    cached = surface_texture_cache_.emplace(
                        plan.resource_id,
                        std::move(texture)).first;
                }
                if (output_coverage.sintab_prepass_runs_pending == 0U) {
                    throw std::logic_error(
                        "surface prepass coverage underflow");
                }
                const auto report =
                    SurfaceSintabPrepassExecutor::draw(
                        plan,
                        cached->second,
                        tables,
                        surface_vga_masks_,
                        frame.scene_video_page,
                        prepass_video);
                any_prepass_rasterized = true;
                ++output_coverage.sintab_prepass_runs_rasterized;
                output_coverage.sintab_prepass_indexed_writes +=
                    report.indexed_writes;
                --output_coverage.sintab_prepass_runs_pending;
            } catch (const std::exception& error) {
                ++output_coverage.sintab_prepass_runs_failed;
                prepass_failures.push_back(SurfacePrepassRasterFailure{
                    plan.resource_id, plan.first_ray, error.what(),
                });
            }
        }
        if (any_prepass_rasterized) {
            prepass_video.export_page(
                framebuffer_, frame.scene_video_page);
        }
    }

    // This presenter currently draws the exact 338C8 fallback path for
    // extents above SINTAB's runtime lookup boundary. The ordinary
    // <= boundary main-pass runs remain pending; the separate prepass
    // now uses validated original SINTAB bytecode for supported runs.
    walls_.draw_main_runs(
        framebuffer_,
        frame.sweep_runs,
        frame.surface_shades,
        tables);

    std::vector<Type7SceneRasterFailure> raster_failures;


    // The original draw order places world-scene type-7 shapes after
    // the environment and wall surfaces, but before party-panel chrome.
    // 369D8 renders to one of two planar VGA pages. A per-call private
    // copy ensures that a malformed/incompletely modeled descriptor
    // cannot partially corrupt the user's current world framebuffer.
    if (frame.scene_vga_trace.has_value() &&
        frame.scene_vga_trace->scene_calls.size() ==
            frame.world_scene_render_plans.size()) {
        VgaPlanarMemory video;
        video.import_page(
            framebuffer_,
            frame.scene_video_page);
        bool any_rasterized = false;
        for (std::size_t index = 0U;
             index < frame.world_scene_render_plans.size();
             ++index) {
            const auto& plan =
                frame.world_scene_render_plans[index];
            if (plan.visible_columns.empty()) {
                continue;
            }
            const auto& optional_trace =
                frame.scene_vga_trace->scene_calls[index];
            if (!optional_trace.has_value()) {
                continue;
            }
            try {
                const auto found =
                    type7_source_cache_.find(plan.call.resource_id);
                const auto body =
                    found != type7_source_cache_.end()
                        ? found->second
                        : assets_.res3_type7_body(
                            plan.call.resource_id);
                // Resource-cache allocation happens before pixel
                // execution. If it fails, no framebuffer changes occur.
                // The executor itself now stages and commits the entire
                // scene, including late generated-code failure cases.
                if (found == type7_source_cache_.end()) {
                    type7_source_cache_.emplace(
                        plan.call.resource_id,
                        body);
                }
                const auto drawn =
                    Type7ScenePixelExecutor::draw(
                        plan,
                        *optional_trace,
                        body,
                        tables,
                        video);
                any_rasterized = true;
                ++output_coverage.type7_calls_rasterized;
                output_coverage.type7_columns_rasterized +=
                    plan.visible_columns.size();
                output_coverage.type7_indexed_writes +=
                    drawn.indexed_writes;
                --output_coverage.type7_scene_calls_pending;
                output_coverage.type7_visible_columns_pending -=
                    plan.visible_columns.size();
            } catch (const std::exception& error) {
                // The executor commits a complete sprite or none of it.
                // Do not discard the reason: without working CI, runtime
                // output must name the exact RES3 source that failed.
                ++output_coverage.type7_calls_failed;
                raster_failures.push_back(Type7SceneRasterFailure{
                    plan.call.resource_id,
                    plan.call.source_class,
                    plan.call.source_index,
                    error.what(),
                });
            }
        }
        if (any_rasterized) {
            video.export_page(
                framebuffer_,
                frame.scene_video_page);
        }
    }

    const auto panels =
        PartyHudModel::panels(
            frame.scene.party,
            res0_raster_base_,
            res1_portrait_base_,
            presentation_mode);
    hud_.draw_party_panels(
        framebuffer_,
        panels);

    ExplorationPresentedFrame out{};
    out.palette =
        assets_.colors_palette(
            frame.scene.palette_index);
    out.surface_coverage =
        output_coverage;
    out.surface_prepass_failures =
        std::move(prepass_failures);
    out.type7_failures = std::move(raster_failures);
    out.type7_trace_failure =
        frame.scene_vga_trace_failure;
    return out;
}

}  // namespace rloft
