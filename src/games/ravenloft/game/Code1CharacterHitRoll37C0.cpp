#include "games/ravenloft/game/Code1CharacterHitRoll37C0.h"

#include "games/ravenloft/game/Code1Dice0010.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

constexpr std::array<std::int8_t, 6> kDieSides0853{{10,10,10,8,4,6}};

constexpr std::array<std::int32_t, 38> kBonus04D4{{
     0,-3,-2,-2,-1,-1,-1,0,0,0,0,0,0,0,0,1,2,3,4,
     5,-3,-2,-2,-1,-1,-1,0,0,0,0,0,0,0,0,1,2,2,2
}};

std::int32_t activeClassCount(
    const std::array<std::uint8_t, 3>& classes) noexcept {
    std::int32_t count = 1;
    if (static_cast<std::int8_t>(classes[1]) != -1) count = 2;
    if (static_cast<std::int8_t>(classes[2]) != -1) ++count;
    return count;
}

} // namespace

void Code1CharacterHitRoll37C0::initialize(
    Code1CharacterHitRollState37C0& state,
    Code1RuntimeRng618A& rng) noexcept {
    const std::int32_t count = activeClassCount(state.fields680D);

    std::int32_t total = 0;
    std::int32_t lastClass = 0;

    // 0x3800..0x3844.
    for (std::int32_t slot = 0; slot < count; ++slot) {
        const auto index = static_cast<std::size_t>(slot);
        lastClass = static_cast<std::int8_t>(state.fields680D[index]);

        const std::int32_t sides =
            static_cast<std::int32_t>(
                kDieSides0853[static_cast<std::size_t>(
                    static_cast<std::uint8_t>(lastClass))]);
        const std::int32_t diceCount =
            static_cast<std::int16_t>(state.words6818[index]);

        total += Code1Dice0010::roll(
            diceCount, sides, 0, 0, rng);
    }

    // 0x3846..0x384F: signed average across active class slots.
    total /= count;

    // 0x3851..0x3873. Unlike 0x3404, negative table entries are NOT clamped
    // here; the signed DWORD is added directly before the low WORD is stored.
    const std::uint32_t statValue = state.bytes682A[4];
    const std::int32_t group = lastClass / 3;
    const std::size_t bonusIndex =
        static_cast<std::size_t>(group * 19) +
        static_cast<std::size_t>(statValue);
    total += kBonus04D4[bonusIndex];

    state.word6816 = static_cast<std::uint16_t>(total);
}

} // namespace ravenloft::game
