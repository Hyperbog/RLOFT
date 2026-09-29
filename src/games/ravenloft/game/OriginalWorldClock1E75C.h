#pragma once

#include <cstdint>

namespace ravenloft::game {

// Opening-relevant reconstruction of CODE.2 0x1E75C and 0x31F94.
//
// DS:CF4C stores the world clock with the hour in the high 16 bits. The
// normal gameplay service advances that hour once for every 0x438 elapsed
// DS:A7E4 gameplay ticks. 0x31F94 normalizes the high word to 0..23 while
// preserving the low word, with the original 0xFFFF -> 23 special case.
class OriginalWorldClock1E75C {
public:
    static constexpr std::uint32_t kGameplayTicksPerHour = 0x438u; // 1080

    OriginalWorldClock1E75C(std::uint32_t worldClockCf4c = 0,
                            std::uint32_t gameplayA7E4AtLoad = 0)
        : worldClockCf4c_(worldClockCf4c), baselineBf50_(gameplayA7E4AtLoad) {}

    std::uint32_t valueCf4c() const { return worldClockCf4c_; }
    std::uint16_t hourHighWord() const {
        return static_cast<std::uint16_t>(worldClockCf4c_ >> 16u);
    }
    std::uint32_t baselineBf50() const { return baselineBf50_; }

    // CODE.2 0x1E75C opening-relevant clock branch. Returns the number of
    // whole world-clock hours advanced by this service call.
    std::uint32_t service1E75C(std::uint32_t gameplayA7E4);

    // CODE.2 0x222C0..0x222E5. Returns true when the high word is already 2.
    // Otherwise it performs the original INC WORD PTR [CF4E] + 0x31F94 and
    // returns false so the caller can expose the original display/update step.
    bool mistWaitStep222C0();

    // Exact world-clock mutation performed by the 0x31F94 boundary after an
    // hour increment. Public for focused regression coverage.
    void incrementHourAndNormalize31F94();

    static std::uint32_t normalize31F94(std::uint32_t value);

private:
    std::uint32_t worldClockCf4c_ = 0;
    std::uint32_t baselineBf50_ = 0;
};

} // namespace ravenloft::game
