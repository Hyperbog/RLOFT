#include "games/ravenloft/game/Code1CharacterChoice266C.h"

#include "games/ravenloft/game/Code1CharacterOptions2198.h"
#include "games/ravenloft/game/Code1CharacterTriple1F24.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

void Code1CharacterChoice266C::run(Code1CharacterChoiceState266C& state,
                                   const Code1CharacterChoiceOps266C& ops) {
    std::int32_t lastSelection = 0;
    std::int32_t modeDelta = 0;
    bool pageInitialized = false;

    state.fields680D.fill(0xFFu);

    std::int32_t advanceState = 0;
    std::int32_t stage = 0;

    for (;;) {
        if (stage >= 3 || modeDelta == -1 || advanceState != 0) {
            break;
        }

        std::array<std::int32_t, 6> optionFlags{};
        const Code1CharacterOptionState2198 optionState{
            state.field680C, state.fields680D[0], state.fields680D[1]
        };
        if (!Code1CharacterOptions2198::build(optionFlags, stage, optionState)) {
            break;
        }

        std::array<std::int32_t, 9> slots{
            -1, -1, -1, -1, -1, -1, -1, -1, -1
        };
        for (std::size_t i = 0; i < 6; ++i) {
            slots[i] = optionFlags[i] != 0
                ? static_cast<std::int32_t>(i) + 6
                : 0x15;
        }

        if (pageInitialized) {
            if (stage != 0) {
                ops.call24D4(lastSelection - 1, 0x114, 0x6D);
            }

            for (std::size_t i = 0; i < 6; ++i) {
                if (slots[i] != 0x15) {
                    if (stage == 0) {
                        state.previous8F4[i] = state.current8F8[i];
                        state.current8F8[i] = slots[i];
                    } else {
                        state.current8F8[i] = slots[i];
                        state.previous8F4[i] = slots[i];
                    }
                } else {
                    state.previous8F4[i] = state.current8F8[i];
                    state.current8F8[i] = 0x15;
                }
            }

            ops.call6588(state.buffer7138);
            ops.call86C4(0xC8, 0, 0x19, 7);
            ops.call04B0(3);
            state.text940 = 0x0B;
            state.text948 = 3;
            ops.call07E4(0, 6);
        } else {
            ops.call1464(3, slots, 0x0A, -1, 0x28);
            state.previous8F4.fill(0x15);
            state.current8F8 = slots;
            pageInitialized = true;
        }

        while (ops.call53B4() != 0) {
        }

        advanceState = 0;
        while (advanceState == 0) {
            const std::int32_t selection = ops.call043C();
            lastSelection = selection;

            if (selection != 0 && selection != 0xFF) {
                if (selection >= 1 && selection <= 6 &&
                    slots[static_cast<std::size_t>(selection - 1)] != 0x15) {
                    state.fields680D[static_cast<std::size_t>(stage)] =
                        static_cast<std::uint8_t>(selection - 1);
                    modeDelta = 1;
                    advanceState = 1;
                }
                continue;
            }

            if (selection == 0xFF && stage != 0) {
                modeDelta = 1;
                advanceState = 2;
                continue;
            }

            if (selection == 0) {
                if (stage != 0) {
                    state.fields680D.fill(0xFFu);
                    stage = -1;
                } else {
                    modeDelta = -1;
                }
                advanceState = 1;
            }
        }

        --advanceState;
        ++stage;
    }

    if (lastSelection != 0 && lastSelection != 0xFF) {
        ops.call15B0(lastSelection - 1, 0x114, 0x6D, 3);
    } else {
        ops.call15B0(-1, -1, -1, -1);
    }

    state.mode4D0 += modeDelta;
    if (modeDelta == 1) {
        Code1CharacterTriple1F24::normalize(state.fields680D);
    }
}

} // namespace ravenloft::game
