#pragma once
// Host elapsed-time accounting for reconstructed CODE.2 IRQ8.
// Dispatch MUST invoke original interrupt_30708 on the owning guest thread.
// The frequency is an explicit, externally authenticated input, not a
// guessed DOS PIT/RTC frequency. This header never edits F58C directly.
#include <chrono>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace rloft { namespace original_irq8 {
enum class Status : std::uint8_t {
    Delivered, NotArmed, InvalidConfiguration, ClockReversed,
    TickOverflow, ExcessiveBacklog
};
struct Progress {
    Status status;
    std::uint32_t interrupts;
    std::uint64_t delivered_total;
};

class ElapsedScheduler final {
public:
    using Clock = std::chrono::steady_clock;
    using Time = Clock::time_point;
    // Neither rate nor interrupt batching limit has a default. Production
    // must authenticate the INT 08h vector cadence from device state.
    bool arm(Time start, std::uint32_t interrupts_per_second,
             std::uint32_t max_interrupts_per_poll) noexcept {
        disarm();
        if(interrupts_per_second==0u || interrupts_per_second>8192u ||
           max_interrupts_per_poll==0u ||
           max_interrupts_per_poll>8192u)
            return false;
        start_=start;
        observed_=start;
        rate_=interrupts_per_second;
        batch_limit_=max_interrupts_per_poll;
        return true;
    }
    void disarm() noexcept {
        rate_=0u;batch_limit_=0u;delivered_=0u;
        start_=Time{};observed_=Time{};
    }
    bool armed() const noexcept { return rate_!=0u; }
    std::uint64_t delivered() const noexcept { return delivered_; }

    template<class Interrupt>
    Progress poll(Time now,Interrupt&& irq8) noexcept {
        static_assert(std::is_nothrow_invocable<Interrupt&>::value,
                      "Original IRQ8 dispatch must be noexcept");
        if(!armed())return {Status::NotArmed,0u,delivered_};
        if(now<observed_ || now<start_)
            return {Status::ClockReversed,0u,delivered_};

        // Split integral seconds and the subsecond remainder to avoid
        // elapsed_nanoseconds * rate overflowing a 64-bit integer.
        const auto elapsed=now-start_;
        const auto seconds=std::chrono::duration_cast<std::chrono::seconds>(elapsed);
        const auto remainder=std::chrono::duration_cast<std::chrono::nanoseconds>(
            elapsed-seconds);
        if(seconds.count()<0 || remainder.count()<0)
            return {Status::ClockReversed,0u,delivered_};
        const auto whole=static_cast<std::uint64_t>(seconds.count());
        if(whole>std::numeric_limits<std::uint64_t>::max()/rate_)
            return {Status::TickOverflow,0u,delivered_};
        const std::uint64_t base=whole*rate_;
        const auto sub=static_cast<std::uint64_t>(remainder.count());
        const std::uint64_t fraction=(sub*rate_)/1000000000ull;
        if(base>std::numeric_limits<std::uint64_t>::max()-fraction)
            return {Status::TickOverflow,0u,delivered_};
        const auto expected=base+fraction;
        if(expected<delivered_)
            return {Status::ClockReversed,0u,delivered_};
        const auto owed=expected-delivered_;
        if(owed>batch_limit_)
            return {Status::ExcessiveBacklog,0u,delivered_};
        observed_=now;
        for(std::uint64_t i=0;i<owed;++i) {
            irq8(); // original CODE.2 interrupt, never a host tick write
            ++delivered_;
        }
        return {Status::Delivered,static_cast<std::uint32_t>(owed),delivered_};
    }
private:
    Time start_{};
    Time observed_{};
    std::uint32_t rate_=0u;
    std::uint32_t batch_limit_=0u;
    std::uint64_t delivered_=0u;
};
}} // namespace rloft::original_irq8
