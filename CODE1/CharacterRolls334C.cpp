#include "CharacterRolls334C.h"

#include "Dice0010.h"
#include "RuntimeMemset84A0.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

void CharacterRolls334C::initialize(
    CharacterRollState334C& state,
    RuntimeRng618A& rng) noexcept {
    // 0x3350..0x336C: memset(record + 0x32, 0, 14).
    RuntimeMemset84A0::set(
        state.bytes682A.data(), 0u, state.bytes682A.size());

    // 0x336F..0x339B: six 3d6 rolls, each increased by three, written to every
    // second byte beginning at record offset 0x32.
    for (std::size_t i = 0; i < 6; ++i) {
        const int rolled = Dice0010::roll(3, 6, 0, 0, rng);
        state.bytes682A[i * 2u] =
            static_cast<std::uint8_t>(rolled + 3);
    }

    // 0x339D..0x33FE: only the first rolled byte controls this extra field.
    if (state.bytes682A[0] < 0x12u) {
        return;
    }
    if (state.field680C == 2u) {
        return;
    }

    const bool qualifyingClass =
        state.fields680D[0] == 1u ||
        state.fields680D[0] == 2u ||
        state.fields680D[0] == 0u ||
        state.fields680D[1] == 0u ||
        state.fields680D[2] == 0u;

    if (!qualifyingClass) {
        return;
    }

    // DS:6836 is byte offset 12 within the zeroed 14-byte block.
    state.bytes682A[12] = static_cast<std::uint8_t>(
        Dice0010::roll(1, 100, 0, 0, rng));
}

} // namespace ravenloft::game
