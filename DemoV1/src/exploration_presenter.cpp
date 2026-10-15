#include "rloft/exploration_presenter.hpp"
#include "rloft/enhanced_type7_compositor.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <utility>

namespace rloft {
namespace {
bool original_initial_ground_enabled(const GameSource& source) {
    const auto code2=source.read("CODE.2");
    constexpr auto source_at=0x75F24U+0xC026U;
    if(code2.size()<=source_at) {
        throw std::runtime_error(
            "CODE.2 initial C026 ground-mode byte is missing");
    }
    return code2[source_at]!=0U;
}
} // namespace

ExplorationPresenter::ExplorationPresenter(
    const RavenloftBootAssets& assets)
    : assets_(assets),
      hud_(assets),
      walls_(assets.source()),
      fill_table_(
          SurfaceFillTable::from_code2(
              assets.source().read("CODE.2"))),
      ground_shades_(
          GroundTextureShadeTable::from_code2(
              assets.source().read("CODE.2"))),
      original_c026_initial_enabled_(
          original_initial_ground_enabled(assets.source())),
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
    const auto unresolved_slots=static_cast<std::size_t>(
        std::count_if(
            frame.world_scene_render_plans.begin(),
            frame.world_scene_render_plans.end(),
            [](const WorldSceneRenderPlan& plan) {
                return plan.source_unresolved;
            }));
    // Source-plan slots are the authoritative failure state. A missing
    // metadata entry must never turn an unknown sprite into occlusion.
    out.type7_plan_calls_failed=std::max(
        unresolved_slots,frame.scene_plan_failures.size());
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

std::vector<Type7SceneRenderOutcome>
ExplorationPresenter::initial_scene_outcomes(
    const ExplorationRuntimeFrame& frame) {
    std::vector<Type7SceneRenderOutcome> out;
    out.reserve(frame.world_scene_render_plans.size());
    for (const auto& plan:frame.world_scene_render_plans) {
        if(plan.source_unresolved) {
            out.push_back(Type7SceneRenderOutcome::PlanningFailed);
        } else if(plan.visible_columns.empty()) {
            out.push_back(Type7SceneRenderOutcome::FullyOccluded);
        } else {
            out.push_back(Type7SceneRenderOutcome::TraceUnavailable);
        }
    }
    return out;
}

bool ExplorationPresenter::original_full_ground_eligible(
    const ExplorationRuntimeFrame& frame) {
    // Original CODE.2 1F388 chooses RES4 from the current level
    // header, not the specific authored Level 22 ID. Retain every
    // original full-128-row/256-ray no-wall material proof below.
    if(!frame.ground_source_verified ||
       !frame.ground_perspective_ready ||
       frame.ground_raster_source.size()!=
           GroundTextureRasterSpec::kTextureHeaderBytes+
           GroundTextureRasterSpec::kTextureBytes ||
       frame.sweep_records.size()!=256U) {
        return false;
    }
    // CODE.2 335C8 initializes nearest wall-depth at 0x800,
    // but original 330F3/33284/3338B/33493 reset 22220 to ZERO
    // when a materialized main resource equals 0x93. Either 0 or
    // 0x800 produces 128 lower FF50 rows by DIFFERENT source paths.
    // Source 335DC reads resource_outer ONLY for active +0C prepass,
    // while 336F7 uses resource_inner for every ordinary/main ray.
    // An INACTIVE alternate bank value cannot lower original 22220
    // or change FF50; demanding it be 0x93 was a false restriction.
    // Native Full #657 field audit: all 54 views had only 0x93
    // main runs and zero prepass runs, but failed the old both-bank
    // guard. Preserve a strict 128-row proof: all source-ACTIVE
    // material must be the original 0x93 non-drawable sentinel.
    // Unknown +0C values fail closed rather than being guessed.
    return std::all_of(
        frame.sweep_records.begin(),
        frame.sweep_records.end(),
        [](const ViewSweepRecord& ray) {
            return ray.resource_inner==0x93U &&
                   (ray.prepass==0U ||
                    (ray.prepass==1U && ray.resource_outer==0x93U));
        });
}

bool ExplorationPresenter::enhanced_ground_eligible(
    const ExplorationRuntimeFrame& frame) {
    // The modern renderer composites walls *over* the background floor.
    // Consequently the legacy DOS 22220/FF50 all-rays-sentinel test is
    // not a requirement for drawing a verified RES4 texture. Keep the
    // original audit method available for offline parity research.
    return frame.ground_source_verified &&
           frame.ground_perspective_ready &&
           frame.ground_raster_source.size() ==
               GroundTextureRasterSpec::kTextureHeaderBytes+
               GroundTextureRasterSpec::kTextureBytes;
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
                        ProfilePlacement,
                true,  // native indexed palette: no wrapped bright hues
                command.width); // authored strip/placement width, not full RES8 image
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

    bool ground_textured_rasterized=false;
    std::size_t ground_indexed_writes=0U;
    std::size_t ground_shade_underflow_pixels=0U;
    std::string ground_render_failure;
    if (enhanced_ground_eligible(frame)) {
        try {
            GroundTextureRasterSpec ground{};
            ground.phase_u=frame.ground_phase_u;
            ground.phase_v=frame.ground_phase_v;
            // Verified all-source-active-0x93 open field produces
            // original FF50=128 either by 1F2A5 (22220=0x800) or
            // source 15618[0]=0 (materializer-set 22220=0).
            // Other depths/modes must NOT be given a guessed span.
            ground.row_count=128U;
            // Native palette policy matches the enhanced RES8 sky;
            // the standalone original raster defaults to VGA wrap.
            ground.shade_policy=GroundShadePolicy::ClampToBlack;
            const auto report=OriginalGroundTextureRaster::draw(
                framebuffer_,frame.ground_raster_source,
                ground_shades_,frame.ground_ff58,ground);
            ground_textured_rasterized=true;
            ground_indexed_writes=report.indexed_writes;
            ground_shade_underflow_pixels=report.shade_underflow_pixels;
        } catch(const std::exception& error) {
            // Preserve the old lower fill if any original texel
            // lookup/row-bound validation fails. This does not turn
            // an unsupported source into invented grass.
            ground_render_failure=error.what();
        }
    }
    if(!ground_textured_rasterized) {
        if(ground_render_failure.empty() &&
           original_c026_initial_enabled_) {
            ground_render_failure=frame.ground_source_verified
                ? "original full 128-row 34A74 requires all 256"
                  " sweep rays to carry 0x93"
                : (frame.ground_source_failure.empty()
                   ? "original RES4 ground source not verified"
                   : frame.ground_source_failure);
        }
        SurfaceFillModel::draw_lower(
            framebuffer_,fill_table_,
            frame.scene.frame.renderer_fill_base_b);
    }

    auto output_coverage =
        surface_coverage(frame, tables);

    // CODE.2 335DC first renders any valid extra surface prepass,
    // before 336F7 performs the ordinary wall pass. The prepass calls
    // 32620 with the actual RES4 texture column and original SINTAB
    // generated code. Unsupported original memory geometry, unavailable
    // resources and invalid selectors remain explicitly PENDING.
    std::vector<SurfacePrepassRasterFailure> prepass_failures;
    for (const auto& failure : frame.prepass_plan_failures) {
        prepass_failures.push_back(SurfacePrepassRasterFailure{
            failure.resource_id, failure.first_ray, failure.reason,
        });
        ++output_coverage.sintab_prepass_runs_failed;
    }
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

    // CODE.2 336F7 invokes 3263C with FCD0=0 for the ordinary
    // all-record pass. These calls execute the complete original SINTAB
    // scale block from its normal entry point, not the FCD0=1 temporary
    // RET/vertical-offset path. Preserve prior prepass pixels and commit
    // only source-verified complete four-plane writes.
    std::vector<SurfaceMainRasterFailure> main_failures;
    std::optional<VgaPlanarMemory> main_video;
    bool any_main_rasterized = false;
    for (const auto& run : frame.sweep_runs) {
        if (run.pass != SweepRenderPass::Main || !run.drawable()) {
            continue;
        }
        const auto extent = (run.depth & 0xFFF8U) >> 1U;
        if (extent == 0U || extent > tables.runtime_lookup_max) {
            continue; // 338C8 remains the separate large-extent path.
        }
        try {
            if (!main_video.has_value()) {
                main_video.emplace();
                main_video->import_page(
                    framebuffer_, frame.scene_video_page);
            }
            auto cached = surface_texture_cache_.find(run.resource_id);
            if (cached == surface_texture_cache_.end()) {
                auto texture = walls_.texture(
                    static_cast<int>(run.resource_id));
                cached = surface_texture_cache_.emplace(
                    run.resource_id, std::move(texture)).first;
            }
            if (output_coverage.sintab_main_runs_pending == 0U) {
                throw std::logic_error(
                    "SINTAB main surface coverage underflow");
            }
            const auto report = SurfaceSintabMainExecutor::draw(
                run, cached->second, tables, frame.surface_shades,
                surface_vga_masks_, frame.scene_video_page, *main_video);
            any_main_rasterized = true;
            ++output_coverage.sintab_main_runs_rasterized;
            output_coverage.sintab_main_indexed_writes +=
                report.indexed_writes;
            --output_coverage.sintab_main_runs_pending;
        } catch (const std::exception& error) {
            ++output_coverage.sintab_main_runs_failed;
            main_failures.push_back(SurfaceMainRasterFailure{
                run.resource_id, run.first_ray, error.what(),
            });
        }
    }
    if (any_main_rasterized) {
        main_video->export_page(
            framebuffer_, frame.scene_video_page);
    }

    // CODE.2 338C8 generic scaling applies ONLY ABOVE the original
    // SINTAB threshold; it must never replace in-range generated code.
    walls_.draw_main_runs(
        framebuffer_,
        frame.sweep_runs,
        frame.surface_shades,
        tables);

    std::vector<Type7SceneRasterFailure> raster_failures;
    std::vector<Type7SceneZeroPixelDraw> zero_pixel_draws;
    std::vector<Type7SceneNativeDraw> native_draws;
    auto scene_outcomes=initial_scene_outcomes(frame);

    // Enhanced PC default: RES3 sprites already have decoded native
    // 128-row source pixels and a per-pixel transparency mask. Horizontal
    // placement and wall occlusion remain the existing authored depth-
    // clipped columns; vertical scale uses SINTAB row spans. The old DOS
    // VGA interpreter is preserved below as optional historical code,
    // but it no longer determines whether trees/NPCs are visible.
    constexpr bool kUseNativePcSprites = true;
    if (kUseNativePcSprites) {
        for (std::size_t index=0U;
             index<frame.world_scene_render_plans.size();++index) {
            const auto& plan=frame.world_scene_render_plans[index];
            if (plan.visible_columns.empty()) continue;
            try {
                auto found=enhanced_type7_cache_.find(plan.call.resource_id);
                if (found==enhanced_type7_cache_.end()) {
                    found=enhanced_type7_cache_.emplace(
                        plan.call.resource_id,
                        assets_.res3_type7_native_raster(
                            plan.call.resource_id)).first;
                }
                const auto& block=tables.scale_blocks.at(
                    plan.traversal.scale_block_index);
                const auto source_top=
                    plan.rasterizer_setup.source_center+4;
                // Original SINTAB's packed AL/AH subtractors are presentation
                // color state, separate from original RES3 pixel geometry.
                const auto shades=EnhancedType7ShadeModel::build(
                    plan.visible_runs,plan.visible_columns,
                    plan.traversal.lookup_value,
                    frame.scene_individual_columns_mode,block,
                    plan.rasterizer_setup.render_phase_after_mode,
                    tables.c6e0());
                EnhancedType7DrawMetrics metrics{};
                const auto writes=EnhancedType7Compositor::draw(
                    framebuffer_,found->second,
                    plan.visible_columns,block.scale_step,source_top,
                    &metrics,&shades);
                native_draws.push_back(Type7SceneNativeDraw{
                    plan.call.resource_id,plan.call.source_class,
                    plan.call.source_index,source_top,
                    plan.visible_columns.size(),
                    metrics.potential_opaque_pixels,writes,
                    metrics.left,metrics.top,metrics.right,metrics.bottom,
                    metrics.min_palette_index,metrics.max_palette_index,
                    metrics.shade_underflow_pixels});
                // Successful invocation is not necessarily visible output.
                // Preserve zero-write scenes as diagnostic evidence rather
                // than incorrectly certifying their pixels as rasterized.
                if (writes == 0U) {
                    ++output_coverage.type7_calls_zero_pixels;
                    scene_outcomes[index]=
                        Type7SceneRenderOutcome::RasterizedNoPixels;
                    zero_pixel_draws.push_back(Type7SceneZeroPixelDraw{
                        plan.call.resource_id,plan.call.source_class,
                        plan.call.source_index,source_top,
                        plan.visible_columns.size()});
                } else {
                    ++output_coverage.type7_calls_rasterized;
                    output_coverage.type7_columns_rasterized+=
                        plan.visible_columns.size();
                    scene_outcomes[index]=Type7SceneRenderOutcome::Rasterized;
                }
                output_coverage.type7_indexed_writes+=writes;
                --output_coverage.type7_scene_calls_pending;
                output_coverage.type7_visible_columns_pending-=
                    plan.visible_columns.size();
            } catch(const std::exception& error) {
                ++output_coverage.type7_calls_failed;
                scene_outcomes[index]=Type7SceneRenderOutcome::RasterFailed;
                raster_failures.push_back(Type7SceneRasterFailure{
                    plan.call.resource_id,plan.call.source_class,
                    plan.call.source_index,error.what()});
            }
        }
        // No original VGA microcode trace is needed for enhanced PC
        // scene drawing; don't label its unsupported wide-instruction
        // count as missing native-pixel output. Preserve trace failures
        // separately in ExplorationPresentedFrame for research.
        output_coverage.type7_unvalidated_wide_runs=0U;
    }

    // The original draw order places world-scene type-7 shapes after
    // the environment and wall surfaces, but before party-panel chrome.
    // 369D8 renders to one of two planar VGA pages. A per-call private
    // copy ensures that a malformed/incompletely modeled descriptor
    // cannot partially corrupt the user's current world framebuffer.
    if (!kUseNativePcSprites &&
        frame.scene_vga_trace.has_value() &&
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
                scene_outcomes[index]=Type7SceneRenderOutcome::Rasterized;
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
                scene_outcomes[index]=Type7SceneRenderOutcome::RasterFailed;
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
    out.ground_textured_rasterized=ground_textured_rasterized;
    out.ground_indexed_writes=ground_indexed_writes;
    out.ground_shade_underflow_pixels=ground_shade_underflow_pixels;
    out.ground_render_failure=std::move(ground_render_failure);
    out.palette =
        assets_.colors_palette(
            frame.scene.palette_index);
    out.surface_coverage =
        output_coverage;
    out.surface_prepass_failures =
        std::move(prepass_failures);
    out.surface_main_failures =
        std::move(main_failures);
    out.type7_failures = std::move(raster_failures);
    out.type7_zero_pixel_draws = std::move(zero_pixel_draws);
    out.type7_native_draws = std::move(native_draws);
    out.type7_scene_outcomes = std::move(scene_outcomes);
    out.type7_trace_failure =
        frame.scene_vga_trace_failure;
    return out;
}

}  // namespace rloft
