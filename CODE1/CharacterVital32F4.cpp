#include "CharacterVital32F4.h"

#include "Dice0010.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

struct VitalRow32F4 {
    std::uint8_t base;
    std::uint8_t diceCount;
    std::uint8_t dieSides;
};

// Exact 18-byte table from CODE.1 data object DS:082E.
constexpr std::array<VitalRow32F4, 6> kVital082E{{
    {15, 1, 4},
    {15, 1, 6},
    {20, 3, 4},
    {60, 3, 12},
    {40, 5, 6},
    {100, 5, 6},
}};

} // namespace

void CharacterVital32F4::initialize(
    CharacterVitalState32F4& state,
    RuntimeRng618A& rng) noexcept {
    // 0x3302..0x331A.
    const auto& row = kVital082E[static_cast<std::size_t>(state.field680C)];
    state.word6810 = row.base;

    // 0x3321..0x3340 calls CODE.1 0x0010 with
    // (diceCount, dieSides, perDieModifier=0, flatModifier=0), then performs a
    // 16-bit add into the character record.
    const int rolled = Dice0010::roll(
        row.diceCount, row.dieSides, 0, 0, rng);
    state.word6810 = static_cast<std::uint16_t>(
        state.word6810 + static_cast<std::uint16_t>(rolled));
}

} // namespace ravenloft::game
