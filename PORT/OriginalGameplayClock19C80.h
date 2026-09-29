#pragma once

#include <array>
#include <cstdint>

namespace ravenloft::game {

// Direct state reconstruction of the original timer/gameplay-clock cluster:
// CODE.2 0x30700/0x30708 and 0x19C80..0x19D15.
//
// The original installs PIT mode 3 with divisor 0 (65536) for the 54,925-us
// IRQ0 cadence used by the shipped game. 0x30708 increments DS:F58C, calls
// 0x19C80, then chains the previous BIOS timer handler. For the opening slice,
// one IRQ pulse therefore advances the BIOS tick source once as well.
class OriginalGameplayClock19C80 {
public:
    static constexpr std::uint32_t kOriginalTimerPeriodMicroseconds = 0xD68Du; // 54,925
    static constexpr std::size_t kEnableStackCapacity = 14;

    OriginalGameplayClock19C80(std::uint32_t timerF58C = 0,
                               std::uint32_t gameplayA7E4 = 0,
                               std::uint32_t biosTick046C = 0,
                               std::uint32_t enabledA7E8 = 1)
        : timerF58C_(timerF58C), gameplayA7E4_(gameplayA7E4),
          biosTick046C_(biosTick046C), enabledA7E8_(enabledA7E8) {}

    // CODE.2 0x30700 / CODE.2 0x311E0 / CODE.2 0x19CC4 reads.
    std::uint32_t timer30700() const { return timerF58C_; }
    std::uint32_t biosTick311E0() const { return biosTick046C_; }
    std::uint32_t currentTick19CC4() const { return gameplayA7E4_; }

    std::uint32_t enabledA7E8() const { return enabledA7E8_; }
    std::uint8_t stackDepthA7EC() const { return stackDepthA7EC_; }

    // One original IRQ0 pulse: 0x30708 increments F58C, then 0x19C80
    // conditionally increments A7E4, then the chained BIOS handler advances
    // the BIOS 0x40:0x6C tick. The omitted timed-message branch in 0x19C80 is
    // presentation-only and does not alter these clock semantics.
    void irq30708();

    // Exact state-stack semantics of CODE.2 0x19CCC / 0x19CF4.
    // Push silently does nothing when the original 14-entry stack is full.
    // Pop at depth zero leaves A7E8 unchanged.
    void push19CCC(std::uint32_t enabledValue);
    void pop19CF4();

    // Test/loader boundaries matching original writable state.
    void setTimer30738(std::uint32_t value) { timerF58C_ = value; }
    void setBiosTick311E0(std::uint32_t value) { biosTick046C_ = value; }

private:
    std::uint32_t timerF58C_ = 0;
    std::uint32_t gameplayA7E4_ = 0;
    std::uint32_t biosTick046C_ = 0;
    std::uint32_t enabledA7E8_ = 1;
    std::uint8_t stackDepthA7EC_ = 0;
    std::array<std::uint8_t,kEnableStackCapacity> enableStack140A0_{};
};

} // namespace ravenloft::game
