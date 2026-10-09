#pragma once

#include <chrono>

namespace rloft {

// Native host adapter for CODE.2 201A7..201F7: the original main loop
// anchors to the IRQ clock, advances simulation, then waits TWO game ticks.
// Video refresh at ~70.086 Hz is a DIFFERENT cadence and must never call
// 1FC14 -> 2E120 at every refresh.
//
// The 54,925-us BIOS period is a source-backed fallback, NOT a certified
// measured period for every installed audio/PIT driver combination. A future
// IRQ/PIT trace can replace the host interval without changing input math.
class ExplorationGameplayCadence final {
public:
    using Clock = std::chrono::steady_clock;
    static constexpr auto kFallbackTick = std::chrono::microseconds(54925);
    static constexpr int kOriginalWaitTicks = 2;
    static constexpr auto kFallbackUpdate =
        kFallbackTick * kOriginalWaitTicks;

    // The initial frame is serviced promptly. Do not catch up by running
    // multiple old movement steps after a stalled Windows repaint: the DOS
    // anchored wait also expires immediately after an overrun, and only one
    // gameplay loop step may execute per native presentation iteration.
    [[nodiscard]] bool should_integrate(Clock::time_point now) noexcept {
        if (!initialized_ || now >= next_) {
            initialized_ = true;
            next_ = now + kFallbackUpdate;
            return true;
        }
        return false;
    }

    void reset() noexcept {
        initialized_ = false;
        next_ = Clock::time_point{};
    }

private:
    bool initialized_{};
    Clock::time_point next_{};
};

}  // namespace rloft
