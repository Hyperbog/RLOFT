#include "rloft/exploration_legacy_input.hpp"

#include <cassert>

int main() {
    using C = rloft::ExplorationLegacyInputController;

    static_assert(C::kLimits[0] == 160);
    static_assert(C::kLimits[1] == 80);
    static_assert(C::kGain[0] == 10);
    static_assert(C::kGain[1] == 15);
    static_assert(C::kDecay[0] == 25);
    static_assert(C::kDecay[1] == 80);

    C input;

    // Up (scan 48 -> translated 148) selects C000 direction -1 in the
    // 1FC14 switch. Gain 10 then C6E8 scaling by 20/8 yields -25.
    input.press(C::kUp);
    auto frame = input.tick(false);
    assert(input.axes()[0] == -10);
    assert(frame.velocity == -25);
    assert(frame.angular_input == 0);
    assert(frame.orthogonal_input == 0);

    // Held key repeats through F572/F571 and continues accelerating.
    frame = input.tick(false);
    assert(input.axes()[0] == -20);
    assert(frame.velocity == -50);

    // Releasing the only held key leaves no selection; the C000 decay of 25
    // crosses zero and clamps it there in one frame.
    input.release(C::kUp);
    frame = input.tick(false);
    assert(input.axes()[0] == 0);
    assert(frame.velocity == 0);

    // Home drives C002 negative: gain 15, no decay while selected, then
    // app45_scale_cwde(*,20) produces -37 due to the exact logical-shift /
    // low-word sign-extension sequence.
    input.press(C::kHome);
    frame = input.tick(true);
    assert(input.axes()[1] == -15);
    assert(frame.angular_input == -38 || frame.angular_input == -37);
    // Compute expected value from the encoded integer operation explicitly.
    const auto expected_home =
        static_cast<std::int16_t>(
            static_cast<std::uint16_t>(
                (static_cast<std::uint32_t>(-15) * 20U) >> 3U));
    assert(frame.angular_input == expected_home);
    assert(frame.grid_mode);

    input.clear();

    // Left is orthogonal C004, not angular C002.
    input.press(C::kLeft);
    frame = input.tick(false);
    assert(input.axes()[2] == -10);
    assert(frame.orthogonal_input == -10);
    assert(frame.angular_input == 0);

    input.clear();

    // Most-recent held key controls the switch, while simultaneous held keys
    // select the coupled diagonal axis. Up after Home selects axis 4,-1.
    input.press(C::kHome);
    input.press(C::kUp);
    frame = input.tick(false);
    assert(input.held_order().back() == C::kUp);
    assert(input.axes()[4] < 0);
    assert(input.axes()[0] <= 0);
    assert(input.axes()[1] <= 0);
    assert(frame.velocity < 0);
    assert(frame.angular_input < 0);

    // Re-pressing Home moves it to the newest queue position exactly as 307F0.
    input.press(C::kHome);
    assert(input.held_order().back() == C::kHome);
    assert(input.held_order().size() == 2);

    // Center clears all six persistent axes.
    input.press(C::kCenter);
    frame = input.tick(false);
    for (const auto value : input.axes()) {
        assert(value == 0);
    }
    assert(frame.velocity == 0);
    assert(frame.angular_input == 0);
    assert(frame.orthogonal_input == 0);
}
