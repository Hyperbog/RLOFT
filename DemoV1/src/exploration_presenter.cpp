#include "rloft/exploration_presenter.hpp"

#include <cstddef>
#include <cstdint>

namespace rloft {

ExplorationPresenter::ExplorationPresenter(
    const RavenloftBootAssets& assets)
    : assets_(assets),
      hud_(assets),
      walls_(assets.source()),
      fill_table_(
          SurfaceFillTable::from_code2(
              assets.source().read("CODE.2"))),
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
    // No type-7 invocation is considered presented while the original
    // 369D8 vertical generated-rasterizer path is still untranslated.
    // Fully occluded calls have no pending visible pixels.
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

    // This presenter currently draws the exact 338C8 fallback path for
    // extents above SINTAB's runtime lookup boundary. <= boundary runs remain
    // deliberately untouched until the generated rasterizer translation is
    // byte-validated.
    walls_.draw_main_runs(
        framebuffer_,
        frame.sweep_runs,
        frame.surface_shades,
        tables);

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
        surface_coverage(
            frame,
            tables);
    return out;
}

}  // namespace rloft
