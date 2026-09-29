#include "Code1UiHitTest035C.h"

#include <cstddef>

namespace ravenloft::game {

std::int32_t Code1UiHitTest035C::hitTest(
    std::int32_t x,
    std::int32_t y,
    const Code1UiHitTestState035C& state) noexcept {
    // CODE.1 0x0361: default return is -1.
    std::int32_t result = -1;

    // 0x0366..0x0382: mode 6 scans panels 6..8; other modes scan 0..5.
    const std::int32_t first = state.mode4D0 == 6 ? 6 : 0;
    const std::int32_t end = state.mode4D0 == 6 ? 9 : 6;

    // 0x0384..0x03C3: panel rectangles are 0x7A by 0x30, low-inclusive
    // and high-exclusive. Later hits replace earlier hits exactly as ECX did.
    for (std::int32_t i = first; i < end; ++i) {
        const auto ux = state.panelX[static_cast<std::size_t>(i)];
        const auto uy = state.panelY[static_cast<std::size_t>(i)];
        if (x >= ux && x < ux + 0x7A && y >= uy && y < uy + 0x30) {
            result = i + 1;
        }
    }

    // 0x03C5..0x03E9: fixed special rectangle returns zero.
    if (x >= 0x114 && x < 0x18E && y >= 0x10C && y < 0x13C) {
        result = 0;
    }

    // 0x03EB..0x0432: modes 3/5 expose a dynamic rectangle returning 0xFF.
    if ((state.mode4D0 == 3 || state.mode4D0 == 5) &&
        x >= state.specialX8E4 && x < state.specialX8E4 + 0x7A &&
        y >= state.specialY8A0 && y < state.specialY8A0 + 0x30) {
        result = 0xFF;
    }

    return result;
}

} // namespace ravenloft::game
