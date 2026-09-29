#include "games/ravenloft/game/OriginalDice1C328.h"
#include <cstdint>

namespace ravenloft::game {

int OriginalDice1C328::roll(int diceCount,
                            int dieSides,
                            int perDieModifier,
                            int flatModifier,
                            OriginalRng30CA2& rng) {
    std::int32_t total = 0;
    for (int i = 0; i < diceCount; ++i) {
        // 0x1C33B..0x1C34E: (rng15 * sides) >> 15, +1, then the
        // third argument is added once per die. Keep the original unsigned
        // multiply/shift behavior of EAX for the RNG product.
        const std::uint32_t product = static_cast<std::uint32_t>(rng.next15()) *
                                      static_cast<std::uint32_t>(dieSides);
        total = static_cast<std::int32_t>(static_cast<std::uint32_t>(total) +
                 static_cast<std::uint32_t>((product >> 15u) + 1u));
        total = static_cast<std::int32_t>(static_cast<std::uint32_t>(total) +
                 static_cast<std::uint32_t>(perDieModifier));
    }
    total = static_cast<std::int32_t>(static_cast<std::uint32_t>(total) +
            static_cast<std::uint32_t>(flatModifier));
    if (total < 1) total = 1;
    return total;
}

} // namespace ravenloft::game
