#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/exploration_gameplay_cadence.hpp"

#include <cassert>
#include <chrono>

int main() {
    using C = rloft::ExplorationGameplayCadence;
    using namespace std::chrono;
    static_assert(C::kOriginalWaitTicks == 2);
    static_assert(C::kFallbackTick.count() == 54925);
    static_assert(C::kFallbackUpdate.count() == 109850);

    C cadence{};
    const auto t0 = C::Clock::time_point{microseconds(2000000)};
    assert(cadence.should_integrate(t0));
    // Native VGA presents at ~70 Hz, but no intervening refresh may
    // turn into an extra CODE.2 movement step before two timer ticks.
    for (int frame = 1; frame <= 7; ++frame) {
        assert(!cadence.should_integrate(t0 + microseconds(frame*14268)));
    }
    assert(!cadence.should_integrate(t0 + microseconds(109849)));
    assert(cadence.should_integrate(t0 + microseconds(109850)));
    assert(!cadence.should_integrate(t0 + microseconds(109851)));

    // Late frame: exactly one integration, not replay of a pile of
    // accumulated input impulses; next native cycle gets a full interval.
    const auto delayed = t0 + seconds(3);
    assert(cadence.should_integrate(delayed));
    assert(!cadence.should_integrate(delayed + microseconds(109849)));
    assert(cadence.should_integrate(delayed + microseconds(109850)));

    // Changing modal/pause state resets timing without stale movement.
    cadence.reset();
    assert(cadence.should_integrate(t0));
    assert(!cadence.should_integrate(t0 + microseconds(30000)));
}
