#include "games/ravenloft/game/Code1NameScreen3020.h"

#include "games/ravenloft/game/Code1TextEntry2DFC.h"

#include <array>

namespace ravenloft::game {

void Code1NameScreen3020::run(Code1NameScreenState3020& state,
                              const Code1NameScreenOps3020& ops) {
    const std::array<std::int32_t,9> slots{
        -1,-1,-1,-1,-1,-1,-1,0x14,-1
    };
    ops.call1464(6, slots, -1, -1, 0x2E);

    ops.call6588(state.buffer7138);
    ops.draw.call6C60(0, 0x5B, 0x88, 0x14, 7);
    ops.call53E4(state.buffer7138);
    state.buffer7138 ^= 1u;

    Code1TextEntryState2DFC textState{};
    textState.name67F8 = state.name67F8;
    textState.buffer7138 = state.buffer7138;
    Code1TextEntry2DFC::run(0x93, 0x0F, textState, ops.textEntry);
    state.name67F8 = textState.name67F8;
    state.buffer7138 = textState.buffer7138;

    if (state.handle958 != 0) {
        ops.call7830(state.handle958);
    }

    ops.call6588(state.buffer7138);
    ops.draw.call6C60(0, 0x5B, 0x88, 0x14, 7);
    ops.draw.call7D80(0x93, 0x88, 0x30, state.name67F8.data(), 0);
    state.handle958 = ops.call7868(0x5B, 0x88, 0x7A, 0x30, 0xFF);

    state.value92C = 0;
    state.value930 = 1;

    ops.call15B0(-1, -1, -1, -1);
}

} // namespace ravenloft::game
