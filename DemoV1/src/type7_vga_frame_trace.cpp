#include "rloft/type7_vga_frame_trace.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {

Type7VgaFrameTrace Type7VgaFrameTraceModel::build(
    const std::vector<WorldSceneRenderPlan>& plans,
    const RendererTables& tables,
    const Type7VgaPlaneMaskModel& masks,
    std::uint8_t video_page,
    bool individual_columns_mode,
    const ShapeLookup& shape_lookup) {
    if (video_page > 1U) {
        throw std::out_of_range(
            "type-7 VGA page is outside the two 320x400 pages");
    }
    if (!plans.empty() && !shape_lookup) {
        throw std::invalid_argument(
            "type-7 scene frame trace requires the RES3 shape lookup");
    }

    Type7VgaFrameTrace out{};
    out.scene_calls.reserve(plans.size());
    for (const auto& plan : plans) {
        bool unsupported = false;
        for (const auto& run : plan.visible_runs) {
            if (run.width == 0U ||
                run.width > 7U) {
                ++out.unresolved_wide_runs;
                unsupported = true;
            }
        }
        if (unsupported) {
            // A call containing an unvalidated run must remain untraced,
            // rather than fabricating an incomplete ECX swap sequence.
            out.scene_calls.push_back(std::nullopt);
            continue;
        }
        if (plan.visible_runs.empty()) {
            // A fully hidden call does not dereference a scene resource.
            out.scene_calls.push_back(
                Type7VgaSceneTrace{});
            continue;
        }
        out.scene_calls.push_back(
            Type7VgaSceneTraceModel::build(
                plan,
                shape_lookup(plan.call.resource_id),
                tables,
                masks,
                video_page,
                individual_columns_mode));
    }
    return out;
}

Type7VgaFrameTraceAttempt Type7VgaFrameTraceModel::try_build(
    const std::vector<WorldSceneRenderPlan>& plans,
    const RendererTables& tables,
    const Type7VgaPlaneMaskModel& masks,
    std::uint8_t video_page,
    bool individual_columns_mode,
    const ShapeLookup& shape_lookup) {
    Type7VgaFrameTraceAttempt out{};
    try {
        out.trace = build(
            plans,
            tables,
            masks,
            video_page,
            individual_columns_mode,
            shape_lookup);
    } catch (const std::exception& error) {
        out.failure = error.what();
    }
    return out;
}

} // namespace rloft
