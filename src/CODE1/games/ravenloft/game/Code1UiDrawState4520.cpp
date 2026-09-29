#include "games/ravenloft/game/Code1UiDrawState4520.h"

namespace ravenloft::game {

void Code1UiDrawState4520::draw(
    std::int32_t value,
    std::int32_t delta,
    const Code1UiDrawStateOps4520& ops) {
    if (value == 0x16) {
        const auto resource = ops.call954C(0x3D, 7);
        ops.callA5C8(0, resource);

        ops.call6C98(0, 0xE5, value - delta, 0x3D, 7);

        const auto resourceAgain = ops.call954C(0x3D, 7);
        ops.callA5C8(0, resourceAgain);
    } else if (value == -2) {
        ops.call6C98(0, 0xE5, value - delta, 0x3D, 7);
    } else {
        if (delta > 0) {
            const auto resource = ops.call954C(0x3C, 7);
            ops.callA5C8(0, resource);
        }

        ops.call6C98(0, 0xE5, value, 0x3C, 7);

        if (delta > 0) {
            const auto resource = ops.call954C(0x3C, 7);
            ops.callA5C8(0, resource);
        }
    }

    ops.call6C60(0, 0xE5, 0, 0x3B, 7);
}

} // namespace ravenloft::game
