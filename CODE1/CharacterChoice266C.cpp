#include "CharacterChoice266C.h"

#include "CharacterOptions2198.h"
#include "CharacterTriple1F24.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

void CharacterChoice266C::run(CharacterChoiceState266C& state,
                              const CharacterChoiceOps266C& ops) {
    std::int32_t lastSelection = 0;
    std::int32_t modeDelta = 0;
    bool pageInitialized = false;

    state.fields680D.fill(0xFFu);

    std::int32_t advanceState = 0;
    std::int32_t stage = 0;

    for (;;) {
        if (stage >= 3 || modeDelta == -1 || advanceState != 0) break;

        std::array<std::int32_t, 6> optionFlags{};
        const CharacterOptionState2198 optionState{
            state.field680C, state.fields680D[0], state.fields680D[1]
        };
        const CharacterOptionOps2198 optionOps{ops.readDsS8};
        // stage is constrained by 0x2889..0x2895 to 0..2, and all three of
        // those 0x2198 paths define EDX before returning.
        if (CharacterOptions2198::build(optionFlags, stage, optionState, 0, optionOps) == 0) break;

        std::array<std::int32_t, 9> slots{
            -1, -1, -1, -1, -1, -1, -1, -1, -1
        };
        for (std::size_t i = 0; i < 6; ++i) {
            slots[i] = optionFlags[i] != 0 ? static_cast<std::int32_t>(i) + 6 : 0x15;
        }

        if (pageInitialized) {
            if (stage != 0) ops.call24D4(lastSelection - 1, 0x114, 0x6D);
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
            // 0x27B2..0x27E0 only calls 0x1464 and sets the initialized flag;
            // it does not synthesize an 8F4 fill here.
            ops.call1464(3, slots, 0x0A, -1, 0x28);
            pageInitialized = true;
        }

        while (ops.call53B4() != 0) {}

        advanceState = 0;
        while (advanceState == 0) {
            const std::int32_t selection = ops.call043C();
            lastSelection = selection;
            if (selection != 0 && selection != 0xFF) {
                // 0x2807 performs the stack lookup directly with EAX; there is
                // no explicit 1..6 guard in CODE.1. The UI hit-test contract is
                // the source of the normal six-option range.
                const auto rawIndex = static_cast<std::uint32_t>(selection) - 1u;
                if (*(slots.data() + rawIndex) != 0x15) {
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

    std::uint32_t post15B0Edx;
    if (lastSelection != 0 && lastSelection != 0xFF) {
        post15B0Edx = ops.call15B0(lastSelection - 1, 0x114, 0x6D, 3);
    } else {
        post15B0Edx = ops.call15B0(-1, -1, -1, -1);
    }

    // ADD DWORD DS:04D0,EDI wraps modulo 2^32.
    state.mode4D0 = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(state.mode4D0) +
        static_cast<std::uint32_t>(modeDelta));
    if (modeDelta == 1) {
        CharacterTriple1F24::normalize(
            state.fields680D, static_cast<std::uint8_t>(post15B0Edx));
    }
}

} // namespace ravenloft::game
