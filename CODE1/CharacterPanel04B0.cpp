#include "CharacterPanel04B0.h"

#include <cstddef>

namespace ravenloft::game {

void CharacterPanel04B0::drawThroughStage(
    std::int32_t stage,
    const CharacterPanelState04B0& state,
    const CharacterPanelOps04B0& ops) {
    // 0x04B4..0x04E4.
    if (stage >= 1) {
        ops.draw.call6EB0(0, 0x114, 3,
                          static_cast<std::int32_t>(state.field680B) + 0x0C,
                          7, 0x5FF8);
    }

    // 0x04E7..0x0514.
    if (stage >= 2) {
        ops.draw.call6EB0(0, 0x114, 0x38,
                          static_cast<std::int32_t>(state.field680C),
                          7, 0x5FF8);
    }

    // 0x0517..0x0568: scan record bytes 0x680F, 0x680E, 0x680D.
    if (stage >= 3) {
        for (std::int32_t slot = 2; slot >= 0; --slot) {
            const std::int32_t value =
                static_cast<std::int32_t>(
                    state.fields680D[static_cast<std::size_t>(slot)]);
            if (value == -1) {
                continue;
            }
            ops.draw.call6EB0(0,
                              0x114 - slot * 0x14,
                              0x6D - slot,
                              value + 6,
                              7,
                              0x5FF8);
        }
    }

    // 0x0568..0x059B.
    if (stage >= 4) {
        ops.draw.call6EB0(0, 0x114, 0xA2,
                          static_cast<std::int32_t>(state.field6812) + 0x0E,
                          7, 0x5FF8);
    }

    // 0x059E..0x062B.
    if (stage >= 5) {
        ops.draw.call6C60(0, 0x114, 0xD7, 0x14, 7);

        const std::int32_t computed =
            static_cast<std::int32_t>(state.field680B) * 0x17 +
            state.global713C +
            static_cast<std::int32_t>(state.field680A);
        ops.draw.call6C60(0, 0x129, 0xD7, computed, 1);

        if (state.flag4C4) {
            ops.draw.call7D80(0x116, 0xD7, 0x30, state.recordBase67F8, 0);
        }
    }

    // 0x062B..0x0640.
    if (stage >= 6) {
        ops.call38A4(0x114, 0x10C);
    }
}

} // namespace ravenloft::game
