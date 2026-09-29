#include "CharacterFlags28E4.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

void CharacterFlags28E4::build(
    std::array<std::int32_t, 6>& out,
    const std::array<std::uint8_t, 3>& fields680D) noexcept {
    out.fill(1);

    if (fields680D[0] == 1u) {
        out.fill(0);
        out[0] = 1;
        return;
    }

    if (fields680D[0] == 5u || fields680D[1] == 5u || fields680D[2] == 5u) {
        out[0] = 0;
    }

    if (fields680D[0] == 2u || fields680D[1] == 2u) {
        out[1] = 0;
        out[3] = 0;
        out[5] = 0;
    }
}

void CharacterFlagChoice2994::run(
    CharacterFlagChoiceState2994& state,
    const CharacterFlagChoiceOps2994& ops) {
    std::array<std::int32_t, 6> flags{};
    CharacterFlags28E4::build(flags, state.fields680D);

    std::array<std::int32_t, 9> slots{
        -1,-1,-1,-1,-1,-1,-1,-1,-1
    };
    for (std::size_t i = 0; i < 6; ++i) {
        slots[i] = flags[i] != 0
            ? static_cast<std::int32_t>(i) + 0x0E
            : 0x15;
    }

    ops.call1464(3, slots, 0x0C, -1, 0x29);

    for (;;) {
        const std::int32_t selection = ops.call043C();
        if (selection == 0) {
            --state.mode4D0;
            ops.call15B0(-1, -1, -1, -1);
            return;
        }

        if (selection < 1 || selection > 6 ||
            slots[static_cast<std::size_t>(selection - 1)] == 0x15) {
            continue;
        }

        state.field6812 = static_cast<std::uint8_t>(selection - 1);
        ++state.mode4D0;
        ops.call15B0(selection - 1, 0x114, 0xA2, 4);
        return;
    }
}

} // namespace ravenloft::game
