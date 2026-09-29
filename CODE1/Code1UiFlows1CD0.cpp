#include "Code1UiFlows1CD0.h"

#include <array>
#include <cstddef>
#include <utility>

namespace ravenloft::game {

void Code1UiFlow1CD0::run(Code1UiFlowCharacterState& state,
                          const Code1UiFlowOps1CD0& ops) {
    // 0x1CD5.
    ops.call1A04();

    // 0x1CDA..0x1D13.
    std::array<std::int32_t, 9> slots{
        -1, -1, 1, 1, -1, -1, -1, -1, -1
    };
    const std::int32_t text940 =
        (state.flags4C4[0] != 0 && state.flags4C4[1] != 0) ? 1 : 2;
    ops.call1464(0, slots, text940, -1, -1);

    // 0x1D16..0x1D8D.
    bool finished = false;
    while (!finished) {
        const std::int32_t selection = ops.call043C();

        if (selection <= 0) {
            state.selectedCharacter4CC = -1;
            finished = true;
            continue;
        }

        if (selection < 3 || selection > 4) {
            continue;
        }

        const std::int32_t characterIndex = selection - 3;
        state.selectedCharacter4CC = characterIndex;

        if (state.flags4C4[static_cast<std::size_t>(characterIndex)] != 0) {
            ops.call1BD4();
            continue;
        }

        ops.call15B0(-1, -1, -1, -1);
        ++state.mode4D0;
        state.selectedCharacter4CC = characterIndex;
        state.dword6903[static_cast<std::size_t>(characterIndex)] = 0;
        state.dword6907[static_cast<std::size_t>(characterIndex)] = 0;
        finished = true;
    }

    // 0x1D8F..0x1DD1.
    if (state.handle95C != 0) {
        ops.call7830(state.handle95C);
        state.handle95C = 0;
    }
    if (state.handle960 != 0) {
        ops.call7830(state.handle960);
        state.handle960 = 0;
    }
}

void Code1UiFlow1DD8::run(Code1UiFlowCharacterState& state,
                          const Code1UiFlowOps1CD0& ops) {
    // 0x1DDC..0x1DFB.
    const std::array<std::int32_t, 9> slots{
        -1, -1, 0x0C, 0x0D, -1, -1, -1, -1, -1
    };
    ops.call1464(0, slots, 8, -1, 0x26);

    // 0x1DFE..0x1E5A.
    for (;;) {
        const std::int32_t selection = ops.call043C();

        if (selection <= 0) {
            ops.call15B0(-1, -1, -1, -1);
            --state.mode4D0;
            break;
        }

        if (selection < 3 || selection > 4) {
            continue;
        }

        const std::uint8_t value = static_cast<std::uint8_t>(selection - 3);
        state.byte680B[static_cast<std::size_t>(state.selectedCharacter4CC)] = value;
        ++state.mode4D0;
        ops.call15B0(selection - 1, 0x114, 3, 1);
        break;
    }
}

void Code1UiFlow1E64::run(Code1UiFlowCharacterState& state,
                          const Code1UiFlowOps1CD0& ops) {
    // 0x1E68..0x1E87.
    const std::array<std::int32_t, 9> slots{
        0, 1, 2, 3, 4, 5, -1, -1, -1
    };
    ops.call1464(1, slots, 9, -1, 0x27);

    // 0x1E8A..0x1ED9. The assembly forces EAX=1 after one iteration, so this
    // routine intentionally processes exactly one selection result.
    const std::int32_t selection = ops.call043C();
    if (selection != 0) {
        state.byte680C[static_cast<std::size_t>(state.selectedCharacter4CC)] =
            static_cast<std::uint8_t>(selection - 1);
        ++state.mode4D0;
        ops.call15B0(selection - 1, 0x114, 0x38, 2);
    } else {
        ops.call15B0(-1, -1, -1, -1);
        --state.mode4D0;
    }
}

void Code1SwapByte1EE4::swap(std::uint8_t& a, std::uint8_t& b) noexcept {
    const std::uint8_t temporary = a;
    a = b;
    b = temporary;
}

} // namespace ravenloft::game
