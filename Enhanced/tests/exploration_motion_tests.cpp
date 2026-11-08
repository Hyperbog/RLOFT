#include "rloft/exploration_motion.hpp"

#include <cassert>
#include <cstddef>

int main() {
    rloft::RendererTables tables{};

    // Cardinal signed-magnitude Q16 sine values sufficient for exact tests.
    tables.sine_q16_signmag[0] = 0;
    tables.sine_q16_signmag[90] = 0x0000FFFFU;
    tables.sine_q16_signmag[180] = 0;
    tables.sine_q16_signmag[270] = 0x8000FFFFU;
    tables.sine_q16_signmag[360] = 0x80000000U;

    assert(rloft::ExplorationMotionModel::normalize_heading(-1) == 359);
    assert(rloft::ExplorationMotionModel::normalize_heading(360) == 0);

    // Grid-mode 0 degrees uses the exact CODE.2 table: +Y, zero X.
    auto v = rloft::ExplorationMotionModel::direction_vector(
        0x1234U, 0, true, tables);
    assert(v.delta_x == 0);
    assert(v.delta_y == 0x00010000U);

    // Grid-mode 90 degrees: -X, zero Y.
    v = rloft::ExplorationMotionModel::direction_vector(
        0x1234U, 90, true, tables);
    assert(v.delta_x == 0xFFFF0000U);
    assert(v.delta_y == 0);

    // Free-angle distance is clamped below 0xB000 for a zero high word.
    v = rloft::ExplorationMotionModel::direction_vector(
        0x0000B000U, 0, false, tables);
    assert(v.delta_x == 0);
    assert(v.delta_y != 0);

    rloft::ExplorationMotionState state{};
    state.heading = 270;
    state.angular_remainder = 0;
    state.velocity = 0;

    // 40 raw angular units -> step 2, heading decrements by 2.
    auto frame = rloft::ExplorationMotionModel::integrate(
        state,
        rloft::ExplorationMotionInputs{
            40, 0, 0, false},
        tables);
    assert(frame.state.heading == 268);
    assert(frame.state.angular_remainder == 0);
    assert(!frame.translation.has_value());

    // Grid mode clamps any nonzero turn step to a 90-degree turn.
    frame = rloft::ExplorationMotionModel::integrate(
        state,
        rloft::ExplorationMotionInputs{
            20, 0, 0, true},
        tables);
    assert(frame.state.heading == 180);

    // Positive velocity uses reverse heading and distance 0x64*velocity.
    state.heading = 270;
    frame = rloft::ExplorationMotionModel::integrate(
        state,
        rloft::ExplorationMotionInputs{
            0, 0, 2, true},
        tables);
    assert(frame.translation.has_value());
    assert(frame.translation_heading == 90);
    assert(frame.translation_distance == 200U);
    assert(frame.translation->delta_x == 0xFFFF0000U);
    assert(frame.translation->delta_y == 0);

    // Orthogonal input rotates the movement heading +/-90 and replaces
    // velocity with its absolute value before translation.
    frame = rloft::ExplorationMotionModel::integrate(
        state,
        rloft::ExplorationMotionInputs{
            0, -3, 0, true},
        tables);
    assert(frame.state.velocity == 3);
    assert(frame.translation_heading == 0);
    assert(frame.translation.has_value());
    assert(frame.translation->delta_x == 0);
    assert(frame.translation->delta_y == 0x00010000U);
}
