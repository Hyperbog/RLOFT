#include "Code1Dice0010.h"

#include <cstdint>

namespace ravenloft::game {

int Code1Dice0010::roll(int diceCount,
                        int dieSides,
                        int perDieModifier,
                        int flatModifier,
                        Code1RuntimeRng618A& rng) noexcept {
    // EBX in the original routine. Preserve 32-bit wrapping for every ADD.
    std::uint32_t totalBits = 0u;

    // ESI starts at zero and the original signed JL controls the loop.
    for (std::int32_t i = 0; i < static_cast<std::int32_t>(diceCount); ++i) {
        // 0x001B..0x0027: rand15(); IMUL EAX, dieSides; SHR EAX,15; INC EAX.
        const std::uint32_t product =
            static_cast<std::uint32_t>(rng.next15()) *
            static_cast<std::uint32_t>(dieSides);
        totalBits += (product >> 15u) + 1u;

        // 0x002A: the third argument is applied once per die.
        totalBits += static_cast<std::uint32_t>(perDieModifier);
    }

    // 0x0033: the fourth argument is applied once after all dice.
    totalBits += static_cast<std::uint32_t>(flatModifier);

    const auto signedTotal = static_cast<std::int32_t>(totalBits);
    return signedTotal < 1 ? 1 : signedTotal;
}

} // namespace ravenloft::game
