#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "rloft/renderer_tables.hpp"
#include "rloft/type7_column_traversal.hpp"
#include "rloft/type7_microcode_plan.hpp"
#include "rloft/type7_rasterizer_setup.hpp"
#include "rloft/world_scene_draw_calls.hpp"

namespace rloft {

// A single CODE.2 36CEC invocation, resolved through the actual RES3 type-7
// column shape and current 0x1C-byte wall-ray depth/visibility records.
struct WorldSceneRenderPlan final {
    WorldSceneDrawCall call{};
    // A failed original RES3 plan still occupies its exact original
    // ordered call slot. Never reclassify that source as fully occluded.
    bool source_unresolved{};
    Type7ColumnTraversalPlan traversal{};
    Type7RasterizerSetup rasterizer_setup{};
    std::vector<Type7VisibleRun> visible_runs;
    std::vector<Type7VisibleColumn> visible_columns;
    std::vector<Type7MicrocodeDraw> microcode_draws;
};

struct WorldSceneRenderPlanFailure final {
    std::size_t call_index{};
    std::uint16_t source_class{};
    std::uint16_t source_index{};
    std::uint16_t resource_id{};
    std::string reason;
};

struct WorldSceneRenderPlanAttempt final {
    std::vector<WorldSceneRenderPlan> plans;
    std::vector<WorldSceneRenderPlanFailure> failures;
};

class WorldSceneRenderPlanModel final {
public:
    using ShapeLookup =
        std::function<const Type7SceneShape&(std::uint16_t absolute_res3_id)>;

    // Preserve the draw-call sequence as emitted by the CODE.2 scene loop.
    // Horizontal/occlusion and vertical SINTAB entry/stop operands are
    // resolved, but generated-x86 execution and pixel output remain separate.
    [[nodiscard]] static std::vector<WorldSceneRenderPlan> build(
        const std::vector<WorldSceneDrawCall>& calls,
        const RendererTables& tables,
        const std::vector<ViewSweepRecord>& rays,
        const ShapeLookup& shape_lookup);

    // Native presentation recovery only. Preserve the strict original
    // planning API and the one-to-one original scene draw call positions.
    // A bad local source becomes an unresolved slot, not missing pixels.
    [[nodiscard]] static WorldSceneRenderPlanAttempt try_build(
        const std::vector<WorldSceneDrawCall>& calls,
        const RendererTables& tables,
        const std::vector<ViewSweepRecord>& rays,
        const ShapeLookup& shape_lookup);
};

}  // namespace rloft
