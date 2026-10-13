#pragma once

#include "rloft_native/native_scene_instances.hpp"
#include <cstddef>
#include <optional>
#include <vector>

namespace rloft_native {

// Native host inspection, NOT a reconstructed CODE.2 targeting function.
// Candidates come from the verified source-authored 11-byte placement
// directory; their positions are never synthesized or snapped to cell
// centers. This operation has no combat, pickup, inventory or quest side
// effects.
struct NativeSceneTarget {
    NativeAuthoredSceneInstance original;
    double distance_cells{};
    double bearing_error_radians{};
};

// Closest-to-crosshair candidate inside a bounded cone/radius with a
// source-level ordinary-cell line-of-sight test. A *blocked* target cell
// is allowed only as the final destination; intermediate blocked cells
// obstruct the target. This is a provisional host inspection rule.
[[nodiscard]] std::optional<NativeSceneTarget>
select_native_authored_scene_target(
    const NativeWorldRuntime& world, unsigned level,
    const std::vector<NativeAuthoredSceneInstance>& candidates,
    double camera_column, double camera_row, double heading_radians,
    double radius_cells=2.5, double half_cone_radians=0.45);

} // namespace rloft_native
