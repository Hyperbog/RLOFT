#pragma once

#include "rloft/exploration_locomotion.hpp"
#include "rloft/exploration_motion.hpp"
#include "rloft/exploration_session.hpp"
#include "rloft/renderer_tables.hpp"

namespace rloft {

struct ExplorationRuntimeMotionState final {
    ExplorationMotionState integrator{};
};

struct ExplorationRuntimeMotionResult final {
    ExplorationMotionStep integrated{};
    AppliedExplorationMotion applied{};
};

class ExplorationRuntimeMotionModel final {
public:
    // One portable CODE.2 2E120 -> 2DBAC movement update. The authoritative
    // session owns position/facing; the small runtime state owns only the
    // angular remainder/velocity integration state that survives input polls.
    [[nodiscard]] static ExplorationRuntimeMotionResult tick(
        ExplorationSessionState& session,
        ExplorationRuntimeMotionState& runtime,
        ExplorationMotionInputs inputs,
        const RendererTables& tables,
        const ExplorationLocomotionModel::EntityProbe& entity_probe = {});
};

}  // namespace rloft
