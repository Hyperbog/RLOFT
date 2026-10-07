#include "rloft/exploration_runtime_motion.hpp"

#include <cassert>
#include <cstddef>

namespace {

rloft::RendererTables tables() {
    rloft::RendererTables t{};
    // Motion test uses grid mode, so the trig tables are not consumed.
    return t;
}

rloft::ExplorationSessionState session() {
    rloft::ExplorationSessionState s{};
    s.coordinate_a = 0x00018000U;
    s.coordinate_b = 0x00018000U;
    s.facing = 0;
    s.level.width = 4;
    s.level.height = 4;
    s.level.cell_words.assign(16, 0);
    return s;
}

std::size_t index(int column, int row) {
    return static_cast<std::size_t>(
        row * 4 + column);
}

}  // namespace

int main() {
    auto t = tables();

    {
        auto s = session();
        rloft::ExplorationRuntimeMotionState runtime{};
        runtime.integrator.heading = 123; // session must win.

        rloft::ExplorationMotionInputs input{};
        input.angular_input = -20;
        input.grid_mode = true;

        const auto result =
            rloft::ExplorationRuntimeMotionModel::tick(
                s, runtime, input, t);

        // Grid-mode one integration quantum turns +90 degrees because
        // 2E120 subtracts a -90 snapped step.
        assert(s.facing == 90);
        assert(result.applied.acceptance ==
               rloft::LocomotionAcceptance::NoTranslation);
        assert(runtime.integrator.heading == 90);
        assert(runtime.integrator.angular_remainder == 0);
    }

    {
        auto s = session();
        s.facing = 270;
        rloft::ExplorationRuntimeMotionState runtime{};

        // Positive C6E8 velocity moves on heading+180. At facing 270 this is
        // grid heading 90, whose exact table delta is X=-1,Y=0.
        rloft::ExplorationMotionInputs input{};
        input.velocity = 0x10000 / 0x64;
        input.grid_mode = true;

        const auto result =
            rloft::ExplorationRuntimeMotionModel::tick(
                s, runtime, input, t);

        assert(result.integrated.translation.has_value());
        assert(s.facing == 270);
        assert(result.applied.acceptance ==
               rloft::LocomotionAcceptance::Full);
        assert(s.coordinate_a == 0x00008000U);
        assert(s.coordinate_b == 0x00018000U);
    }

    {
        auto s = session();
        s.facing = 270;

        // The same move is blocked by the 0x5800 footprint when the target
        // cell is ordinary solid geometry; facing still remains authoritative.
        s.level.cell_words[index(0, 1)] = 1U;

        rloft::ExplorationRuntimeMotionState runtime{};
        rloft::ExplorationMotionInputs input{};
        input.velocity = 0x10000 / 0x64;
        input.grid_mode = true;

        const auto result =
            rloft::ExplorationRuntimeMotionModel::tick(
                s, runtime, input, t);

        assert(result.applied.acceptance ==
               rloft::LocomotionAcceptance::Blocked);
        assert(s.coordinate_a == 0x00018000U);
        assert(s.coordinate_b == 0x00018000U);
        assert(s.facing == 270);
    }
}
