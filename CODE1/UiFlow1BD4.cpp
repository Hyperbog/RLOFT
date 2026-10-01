#include "UiFlow1BD4.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

void UiFlow1BD4::run(UiFlowState1BD4& state,
                     const UiFlowOps1BD4& ops) {
    // 0x1BD8..0x1BFD: save the current pair, then install 0x2F/0x30.
    state.previous904 = state.current908;
    state.previous90C = state.current910;
    state.current908 = 0x2F;
    state.current910 = 0x30;

    // 0x1C00..0x1C09: cdecl 0x07E4(0, 6).
    ops.call07E4(0, 6);

    // 0x1C0C..0x1C19: MOVZX BYTE PTR DS:7138 before calling 0x6588.
    ops.call6588(static_cast<std::uint8_t>(state.buffer7138));

    // 0x1C1C..0x1C23.
    ops.call04B0(6);

    // 0x1C26..0x1C42: the first argument is PUSH DWORD PTR DS:7138,
    // unlike the byte-sized access immediately above.
    ops.call550C(state.buffer7138, 0xC8, 0, 0xC8, 0x140);

    // 0x1C45: XOR BYTE PTR DS:7138,1. Preserve the upper 24 bits.
    state.buffer7138 = (state.buffer7138 & 0xFFFFFF00u) |
                      ((state.buffer7138 ^ 1u) & 0xFFu);

    // 0x1C4C..0x1C8F: EBX is the loop-exit flag. 0x043C's return is
    // compared unsigned: values below 3 repeat, 3 exits, 4 performs the
    // selected-character mutation and exits, and all values above 4 repeat.
    std::uint32_t done = 0;
    do {
        const std::uint32_t selection =
            static_cast<std::uint32_t>(ops.call043C());

        if (selection == 3u) {
            done = 1;
        } else if (selection == 4u) {
            const auto character =
                static_cast<std::size_t>(state.selectedCharacter4CC);

            // 0x1C5F..0x1C70: EAX = DS:04CC * 0x248, then WORD
            // [EAX+6808] = 0x8000.
            state.word6808[character] = 0x8000u;

            // 0x1C72..0x1C77: this second access intentionally uses the
            // unscaled DS:04CC as a BYTE index into DS:04C4.
            state.flags4C4[character] = 0;
            state.current940 = 2;
            done = 1;
        }
    } while (done == 0);

    // 0x1C91..0x1CB6: save the active pair again, then install 1/1.
    state.previous904 = state.current908;
    state.previous90C = state.current910;
    state.current908 = 1;
    state.current910 = 1;

    // 0x1CB9..0x1CC7.
    ops.call1A04();
    ops.call07E4(0, 6);
}

} // namespace ravenloft::game
