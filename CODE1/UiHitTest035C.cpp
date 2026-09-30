#include "UiHitTest035C.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {
std::int32_t addWrap32(std::int32_t value, std::uint32_t amount) noexcept {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(value) + amount);
}
}

std::int32_t UiHitTest035C::hitTest(
    std::int32_t x,
    std::int32_t y,
    const UiHitTestState035C& state) noexcept {
    // 0x0361: ECX is the return accumulator and starts at -1.
    std::int32_t result = -1;

    // 0x0366..0x0382: mode 6 scans records 6..8; all other modes 0..5.
    std::int32_t i = state.mode4D0 == 6 ? 6 : 0;
    const std::int32_t end = state.mode4D0 == 6 ? 9 : 6;

    // 0x0384..0x03C3.  Every CMP/Jcc here is signed.  The high edges are
    // formed by 32-bit ADD, so preserve modulo-2^32 wrap before interpreting
    // the result as signed; ordinary signed C++ addition would be UB there.
    for (; i < end; ++i) {
        const std::int32_t left = state.panelX[static_cast<std::size_t>(i)];
        const std::int32_t top = state.panelY[static_cast<std::size_t>(i)];
        const std::int32_t right = addWrap32(left, 0x7Au);
        const std::int32_t bottom = addWrap32(top, 0x30u);
        if (x >= left && right > x && y >= top && bottom > y) {
            result = i + 1;
        }
    }

    // 0x03C5..0x03E9: low-inclusive/high-exclusive fixed rectangle.
    if (x >= 0x114 && x < 0x18E && y >= 0x10C && y < 0x13C) {
        result = 0;
    }

    // 0x03EB..0x0432: modes 3 and 5 add a dynamic rectangle returning 0xFF.
    // Its right/bottom calculations use the same wrapping 32-bit ADDs.
    if (state.mode4D0 == 3 || state.mode4D0 == 5) {
        const std::int32_t right = addWrap32(state.specialX8E4, 0x7Au);
        const std::int32_t bottom = addWrap32(state.specialY8A0, 0x30u);
        if (x >= state.specialX8E4 && right > x &&
            y >= state.specialY8A0 && bottom > y) {
            result = 0xFF;
        }
    }

    return result;
}

} // namespace ravenloft::game
