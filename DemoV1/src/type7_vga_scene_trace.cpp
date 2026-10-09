#include "rloft/type7_vga_scene_trace.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
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
        if (run.width == 0U ||
            run.width > std::numeric_limits<std::uint8_t>::max() ||
            (!individual_columns_mode && run.width > 7U)) {
            // Grouped original 36B14 calls require validated 1..7
            // sequencer masks. In individual C029 mode, 36B42 loops
            // one 369D8 invocation PER pixel and uses width=1 masks;
            // preserve the unsplit original run up to uint8 capacity.
            // Exact 256-column C029 iteration is not yet modeled.
            throw std::out_of_range(
                "scene VGA invocation has unvalidated run width");
        }
        if (run.viewport_x >= 256U ||
            run.width > 256U - run.viewport_x) {
            // The original 36CEC source traversal clips to the
            // 256-ray viewport BEFORE it invokes 36B14.
            throw std::out_of_range(
                "scene VGA run exceeds original 256-ray viewport");
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
        invocation.individual_columns_mode = individual_columns_mode;
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
