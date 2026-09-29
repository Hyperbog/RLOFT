#include "games/ravenloft/game/Code1UiFlow1BD4.h"

#include <cstddef>

namespace ravenloft::game {

void Code1UiFlow1BD4::run(Code1UiFlowState1BD4& state,
                          const Code1UiFlowOps1BD4& ops) {
    state.previous904 = state.current908;
    state.previous90C = state.current910;
    state.current908 = 0x2F;
    state.current910 = 0x30;

    ops.call07E4(0, 6);
    ops.call6588(state.buffer7138);
    ops.call04B0(6);
    ops.call550C(state.buffer7138, 0xC8, 0, 0xC8, 0x140);
    state.buffer7138 ^= 1u;

    for (;;) {
        const std::int32_t selection = ops.call043C();
        if (selection < 3) continue;
        if (selection == 3) break;
        if (selection == 4) {
            const auto character = static_cast<std::size_t>(state.selectedCharacter4CC);
            state.word6808[character] = 0x8000u;
            state.flags4C4[character] = 0;
            state.current940 = 2;
            break;
        }
    }

    state.previous904 = state.current908;
    state.previous90C = state.current910;
    state.current908 = 1;
    state.current910 = 1;
    ops.call1A04();
    ops.call07E4(0, 6);
}

} // namespace ravenloft::game
