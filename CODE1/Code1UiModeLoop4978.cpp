#include "Code1UiModeLoop4978.h"

namespace ravenloft::game {

std::int32_t Code1UiModeLoop4978::run(
    Code1UiModeLoopState4978& state,
    const Code1UiModeLoopOps4978& ops) {
    state.mode4D0 = 0;

    // 0x4985..0x49B1.
    ops.callC254(0x64F8, 0, 8, 0x64, 0x100);
    Code1UiSequence45FC::run(state.sequence, ops.sequence);
    state.byte73EE =
        static_cast<std::uint8_t>(state.sequence.buffer7138 ^ 1u);
    ops.call57DC();

    for (;;) {
        // 0x49B6..0x4A08.
        if (state.mode4D0 == 7) {
            state.mode4D0 = 0;
            continue;
        }

        // Original indirect call through DWORD table DS:085C.
        ops.dispatchMode(state.mode4D0);

        while (ops.call53B4() != 0) {
        }

        if (state.value4CC != -1) {
            continue;
        }

        return (state.flag4C4 != 0 && state.flag4C5 != 0) ? 10 : 0;
    }
}

} // namespace ravenloft::game
