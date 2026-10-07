#include "rloft/type7_vga_scene_trace.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {

Type7VgaSceneTrace Type7VgaSceneTraceModel::build(
    const WorldSceneRenderPlan& plan,
    const Type7SceneShape& shape,
    const RendererTables& tables,
    const Type7VgaPlaneMaskModel& masks,
    std::uint8_t video_page,
    bool individual_columns_mode) {
    Type7VgaSceneTrace out{};
    out.final_lookup_word =
        plan.traversal.lookup_value;
    if (plan.visible_runs.empty()) {
        return out;
    }
    if (!plan.rasterizer_setup.drawable) {
        throw std::logic_error(
            "type-7 rasterizer-suppressed scene contains visible runs");
    }

    out.runs.reserve(plan.visible_runs.size());
    for (const auto& run : plan.visible_runs) {
        if (run.width == 0U || run.width > 7U) {
            // The 32-byte CODE.2 plane tables contain only 1..7 runs.
            // Do not narrow a larger scene span and accidentally alias a
            // valid table slot before proving original run segmentation.
            throw std::out_of_range(
                "scene VGA invocation has unvalidated run width");
        }
        // One representative destination column is sufficient to recover
        // the source display-list entry/stop patch operands. The original
        // 369D8 walks every descriptor for each VGA mask pass, while 36B14
        // handles the run-wide count and stateful ECX byte swap outside it.
        const std::vector<Type7VisibleColumn> representative{
            Type7VisibleColumn{
                run.source_x,
                run.source_column_index,
                run.viewport_x,
            },
        };
        Type7VgaSceneRun invocation{};
        invocation.source_x = run.source_x;
        invocation.source_column_index =
            run.source_column_index;
        invocation.viewport_x = run.viewport_x;
        invocation.width = run.width;
        invocation.descriptors =
            Type7MicrocodePlanModel::build(
                shape,
                plan.traversal,
                representative,
                tables);

        invocation.dispatch =
            Type7VgaRasterDispatchModel::build(
                masks,
                static_cast<std::int32_t>(
                    run.viewport_x),
                static_cast<std::uint8_t>(
                    run.width),
                video_page,
                plan.rasterizer_setup.
                    render_phase_after_mode,
                tables.c6e0(),
                out.final_lookup_word,
                individual_columns_mode);
        out.final_lookup_word =
            invocation.dispatch.lookup_after_dispatch;
        out.runs.push_back(
            std::move(invocation));
    }
    return out;
}

} // namespace rloft
