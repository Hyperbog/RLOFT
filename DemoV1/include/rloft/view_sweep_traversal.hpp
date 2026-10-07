#pragma once

#include <cstdint>
#include <vector>

#include "rloft/exploration_session.hpp"
#include "rloft/renderer_tables.hpp"
#include "rloft/view_sweep_record.hpp"
#include "rloft/view_sweep_setup.hpp"

namespace rloft {

struct ViewSweepOptions final {
    // DGROUP FD08: True Seeing and equivalent renderer-wide hidden topology
    // reveal/pass-through state.
    bool hidden_topology_pass_through{};
};

class ViewSweepTraversalModel final {
public:
    // Portable reconstruction of CODE.2 334C4 -> 32954. Clears only the
    // current-visibility bit, then emits one 1Ch-equivalent sweep record per
    // original view ray.
    [[nodiscard]] static std::vector<ViewSweepRecord> build(
        ExplorationSessionState& session,
        const RendererTables& tables,
        ViewSweepOptions options = {});

private:
    [[nodiscard]] static ViewSweepRecord trace_ray(
        LevelState& level,
        const RendererTables& tables,
        const ViewCameraBasis& basis,
        const ViewRaySetup& ray,
        ViewSweepOptions options);
};

}  // namespace rloft
