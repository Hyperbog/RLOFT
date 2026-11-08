#include "rloft/exploration_runtime_motion.hpp"

namespace rloft {

ExplorationRuntimeMotionResult ExplorationRuntimeMotionModel::tick(
    ExplorationSessionState& session,
    ExplorationRuntimeMotionState& runtime,
    ExplorationMotionInputs inputs,
    const RendererTables& tables,
    const ExplorationLocomotionModel::EntityProbe& entity_probe) {
    // C658 is authoritative. Re-seed the integrator heading from the current
    // session so future transition/event code can change facing without
    // desynchronizing input integration.
    runtime.integrator.heading = session.facing;

    ExplorationRuntimeMotionResult out{};
    out.integrated = ExplorationMotionModel::integrate(
        runtime.integrator,
        inputs,
        tables);

    out.applied = ExplorationLocomotionModel::apply(
        session,
        out.integrated,
        entity_probe);

    runtime.integrator = out.integrated.state;
    runtime.integrator.heading = session.facing;
    return out;
}

}  // namespace rloft
