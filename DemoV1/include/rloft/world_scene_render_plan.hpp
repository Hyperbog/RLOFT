#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "rloft/renderer_tables.hpp"
#include "rloft/type7_column_traversal.hpp"
#include "rloft/world_scene_draw_calls.hpp"

namespace rloft {

// A single CODE.2 36CEC invocation, resolved through the actual RES3 type-7
// column shape and current 0x1C-byte wall-ray depth/visibility records.
struct WorldSceneRenderPlan final {
    WorldSceneDrawCall call{};
    Type7ColumnTraversalPlan traversal{};
    std::vector<Type7VisibleColumn> visible_columns;
};

class WorldSceneRenderPlanModel final {
public:
    using ShapeLookup =
        std::function<Type7SceneShape(std::uint16_t absolute_res3_id)>;

    // Preserve the draw-call sequence as emitted by the CODE.2 scene loop.
    // This is a portable horizontal/occlusion planning layer. Executing the
    // vertical SINTAB x86 rasterizer and compositing pixels remains separate.
    [[nodiscard]] static std::vector<WorldSceneRenderPlan> build(
        const std::vector<WorldSceneDrawCall>& calls,
        const RendererTables& tables,
        const std::vector<ViewSweepRecord>& rays,
        const ShapeLookup& shape_lookup);
};

}  // namespace rloft
