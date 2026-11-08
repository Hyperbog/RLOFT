#include "rloft/type7_vga_frame_trace.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {

// Per-call original CODE.2 36B14 VGA tracing. An unsupported wide
// source is a null trace, not a success; a malformed *other* source
// must not invalidate this independently drawable call.
std::optional<Type7VgaSceneTrace> build_scene(
    const WorldSceneRenderPlan& plan,
    const RendererTables& tables,
    const Type7VgaPlaneMaskModel& masks,
    std::uint8_t video_page,
    bool individual_columns_mode,
    const Type7VgaFrameTraceModel::ShapeLookup& shape_lookup,
    std::size_t& unresolved_wide_runs) {
    if (plan.source_unresolved) {
        // A planning failure must never be interpreted as a fully
        // hidden, successfully traced object.
        return std::nullopt;
    }
    bool unsupported = false;
    for (const auto& run : plan.visible_runs) {
        if (run.width == 0U || run.width > 255U ||
            (!individual_columns_mode && run.width > 7U)) {
            ++unresolved_wide_runs;
            unsupported = true;
        }
    }
    if (unsupported) {
        // Never synthesize a partial ECX swap/mask sequence.
        return std::nullopt;
    }
    if (plan.visible_runs.empty()) {
        // Fully occluded calls are not missing scene pixels.
        return Type7VgaSceneTrace{};
    }
    return Type7VgaSceneTraceModel::build(
        plan, shape_lookup(plan.call.resource_id), tables, masks,
        video_page, individual_columns_mode);
}

void validate_frame(
    const std::vector<WorldSceneRenderPlan>& plans,
    std::uint8_t video_page,
    const Type7VgaFrameTraceModel::ShapeLookup& shape_lookup) {
    if (video_page > 1U) {
        throw std::out_of_range(
            "type-7 VGA page is outside the two 320x400 pages");
    }
    if (!plans.empty() && !shape_lookup) {
        throw std::invalid_argument(
            "type-7 scene frame trace requires the RES3 shape lookup");
    }
}
} // namespace

Type7VgaFrameTrace Type7VgaFrameTraceModel::build(
    const std::vector<WorldSceneRenderPlan>& plans,
    const RendererTables& tables,
    const Type7VgaPlaneMaskModel& masks,
    std::uint8_t video_page,
    bool individual_columns_mode,
    const ShapeLookup& shape_lookup) {
    validate_frame(plans, video_page, shape_lookup);
    Type7VgaFrameTrace out{};
    out.scene_calls.reserve(plans.size());
    for (const auto& plan : plans) {
        if (plan.source_unresolved) {
            throw std::invalid_argument(
                "strict VGA scene trace cannot accept unresolved RES3 plan");
        }
        out.scene_calls.push_back(build_scene(
            plan, tables, masks, video_page, individual_columns_mode,
            shape_lookup, out.unresolved_wide_runs));
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
        validate_frame(plans, video_page, shape_lookup);
    } catch (const std::exception& error) {
        // A bad global VGA page or missing shape resolver cannot be
        // rescued by a different scene. Retain existing fatal behavior.
        out.failure = error.what();
        return out;
    }

    Type7VgaFrameTrace partial{};
    partial.scene_calls.reserve(plans.size());
    for (std::size_t index=0U; index<plans.size(); ++index) {
        const auto& plan = plans[index];
        try {
            partial.scene_calls.push_back(build_scene(
                plan, tables, masks, video_page,
                individual_columns_mode, shape_lookup,
                partial.unresolved_wide_runs));
        } catch (const std::exception& error) {
            // Preserve ordering and identity; never claim successful
            // pixel output for this original RES3 source.
            partial.scene_calls.push_back(std::nullopt);
            out.scene_failures.push_back(Type7VgaSceneTraceFailure{
                index, plan.call.source_class, plan.call.source_index,
                plan.call.resource_id, error.what(),
            });
        }
    }
    out.trace = std::move(partial);
    return out;
}

} // namespace rloft
