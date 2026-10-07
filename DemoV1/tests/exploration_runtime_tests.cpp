#include "rloft/exploration_runtime.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

rloft::ExplorationSessionState session() {
    rloft::ExplorationSessionState s{};
    s.level_id = 22;
    s.coordinate_a = 0x00018000U;
    s.coordinate_b = 0x00018000U;
    s.facing = 0;
    s.time_of_day_16_16_hours = 0x000E0000U;
    s.level.width = 4;
    s.level.height = 4;
    s.level.environment_profile = 0xFFU;
    s.level.cell_words.assign(16, 0);
    s.level.automap_cache.assign(16, 0xFFU);
    return s;
}

rloft::RendererTables renderer() {
    rloft::RendererTables t{};
    t.constants[0] = 1;       // one test ray
    t.constants[5] = 0;       // camera origin unchanged
    t.constants[7] = 0x1600;  // valid depth numerator
    t.view_angle_tenths.fill(0);
    t.tangent_q16.fill(0);
    t.sine_q16_signmag.fill(0);
    t.runtime_lookup_max = 0;
    return t;
}

}  // namespace

int main() {
    rloft::ExplorationRuntime runtime(
        session(),
        renderer(),
        {});

    {
        const auto frame = runtime.build_frame();

        assert(frame.scene.frame.level_id == 22);
        const rloft::CellPosition expected_party_cell{1, 1};
        assert(frame.scene.frame.party_cell ==
               expected_party_cell);
        assert(frame.scene.palette_index == 4);
        assert(frame.scene.environment_mode == 0);
        assert(frame.sweep_records.size() == 1);
        assert(!frame.sweep_runs.empty());
        assert(frame.prepass_plans.empty());

        // The automap was rebuilt from the same post-sweep level state.
        assert(runtime.session().level.automap_cache.size() == 16);
        const auto party_index =
            rloft::ExplorationWorldModel::cell_index(
                runtime.session().level,
                {1, 1});
        assert(runtime.session().level.automap_cache[party_index] == 0x2FU);
    }

    {
        rloft::ExplorationMotionInputs input{};
        input.angular_input = -20;
        input.grid_mode = true;

        const auto motion = runtime.tick_motion(input);
        assert(runtime.session().facing == 90);
        assert(motion.applied.acceptance ==
               rloft::LocomotionAcceptance::NoTranslation);

        const auto frame = runtime.build_frame();
        assert(frame.scene.frame.facing_degrees == 90);
    }
}
