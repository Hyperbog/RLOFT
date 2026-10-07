#include "rloft/original_random.hpp"

#include <cstdint>

namespace rloft {

std::uint16_t OriginalRandom::next15() noexcept {
    // 32-bit IMUL/add wrap exactly as the original x86 state update.
    state_ =
        state_ * 0x41C64E6DU +
        0x00003039U;
    return static_cast<std::uint16_t>(
        (state_ >> 16U) & 0x7FFFU);
}

std::uint16_t OriginalRandom::dice(
    std::uint8_t count,
    std::uint8_t sides,
    std::int16_t per_die_bonus,
    std::int16_t final_bonus) noexcept {
    std::int32_t total = 0;

    for (std::uint16_t i = 0;
         i < count;
         ++i) {
        const auto random =
            static_cast<std::uint32_t>(
                next15());
        const auto roll =
            static_cast<std::int32_t>(
                (random *
                 static_cast<std::uint32_t>(
                     sides)) >>
                15U) +
            1;
        total += roll;
        total += per_die_bonus;
    }

    total += final_bonus;
    if (total < 1) {
        total = 1;
    }
    return static_cast<std::uint16_t>(
        total);
}

}  // namespace rloft
