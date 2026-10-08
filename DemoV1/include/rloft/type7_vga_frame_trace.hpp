#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "rloft/type7_vga_scene_trace.hpp"
#include "rloft/world_scene_render_plan.hpp"

namespace rloft {

// One slot per scene draw call, preserving original draw order and preserving
// incomplete/unvalidated calls rather than treating them as successfully
// drawn. Even a valid trace does NOT imply that pixels were rasterized.
struct Type7VgaFrameTrace final {
    std::vector<std::optional<Type7VgaSceneTrace>> scene_calls;
    std::size_t unresolved_wide_runs{};
};

struct Type7VgaFrameTraceAttempt final {
    std::optional<Type7VgaFrameTrace> trace;
    std::optional<std::string> failure;
};

class Type7VgaFrameTraceModel final {
public:
    using ShapeLookup =
        std::function<const Type7SceneShape&(
            std::uint16_t absolute_res3_id)>;

    [[nodiscard]] static Type7VgaFrameTrace build(
        const std::vector<WorldSceneRenderPlan>& plans,
        const RendererTables& tables,
        const Type7VgaPlaneMaskModel& masks,
        std::uint8_t video_page,
        bool individual_columns_mode,
        const ShapeLookup& shape_lookup);

    // Rendering diagnostics must not block gameplay/ray snapshots.
    // Preserve the exact failing reason for the presenter to report.
    [[nodiscard]] static Type7VgaFrameTraceAttempt try_build(
        const std::vector<WorldSceneRenderPlan>& plans,
        const RendererTables& tables,
        const Type7VgaPlaneMaskModel& masks,
        std::uint8_t video_page,
        bool individual_columns_mode,
        const ShapeLookup& shape_lookup);
};

} // namespace rloft
