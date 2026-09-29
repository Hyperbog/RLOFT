#include "Code1CharacterActivation3138.h"

#include <array>
#include <cstddef>

namespace ravenloft::game {

void Code1CharacterActivation3138::run(
    Code1CharacterActivationState3138& state,
    const Code1CharacterActivationOps3138& ops) {
    const std::array<std::int32_t, 9> slots{
        -1, -1, 0x2F, 0x30, -1, -1, -1, -1, -1
    };

    ++state.mode4D0;
    ops.call1464(6, slots, -1, -1, 0x2D);
    --state.mode4D0;

    bool finished = false;
    while (!finished) {
        const std::int32_t savedMode = state.mode4D0;
        state.mode4D0 = 0;
        const std::int32_t selection = ops.call043C();
        state.mode4D0 = savedMode;

        if (selection == 3) {
            ops.call15B0(-1, -1, -1, -1);
            ops.call3020();

            const auto character =
                static_cast<std::size_t>(state.selectedCharacter4CC);
            state.active4C4[character] = 1;
            state.status6808[character] = 0;
            finished = true;
        } else if (selection == 4) {
            ops.call15B0(-1, -1, -1, -1);

            const auto character =
                static_cast<std::size_t>(state.selectedCharacter4CC);
            state.active4C4[character] = 0;
            state.status6808[character] =
                static_cast<std::uint16_t>(state.status6808[character] | 0x8000u);
            finished = true;
        }
    }
}

} // namespace ravenloft::game
