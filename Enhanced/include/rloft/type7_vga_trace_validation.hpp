#pragma once

#include "rloft/type7_vga_scene_trace.hpp"

namespace rloft {

// Checks that a generated 36B14 scene trace is a complete, byte-exact
// CODE.2 369D8 call sequence before any SINTAB pixel writes are accepted.
// Mask bits and pass addresses are derived independently from the run
// geometry, rather than trusted from the trace being validated.
class Type7VgaTraceValidator final {
public:
    static void validate(const Type7VgaSceneRun& run);

    // Also enforce the caller's original starting ECX, lookup persistence
    // across successive 36B14 runs, final state and run geometry. This
    // validates the ENTIRE trace before a pixel executor writes anything.
    static void validate_scene(
        const Type7VgaSceneTrace& trace,
        const WorldSceneRenderPlan& plan);
};

} // namespace rloft
