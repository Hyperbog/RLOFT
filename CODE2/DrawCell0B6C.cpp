#include "DrawCell0B6C.h"

#include <cstddef>

namespace ravenloft::game {
namespace {

std::uint16_t read16(std::span<const std::uint8_t> bytes,
                     std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(bytes[offset]) |
           (static_cast<std::uint16_t>(bytes[offset + 1u]) << 8u);
}

} // namespace

void DrawCell0B6C::run(DrawCellState0B6C& state,
                       const DrawCellOps0B6C& ops,
                       std::int32_t x,
                       std::int32_t y,
                       std::int32_t arg3,
                       std::int32_t arg4) {
    const std::int32_t linear =
        x + static_cast<std::int32_t>(state.width12756) * y;
    std::int32_t code =
        state.map1760[static_cast<std::size_t>(linear)];

    if (code == 0xFF) {
        return;
    }

    if (code < 0x20) {
        code = ops.call09B0(linear, y, x);
    }

    const std::size_t flagOffset = static_cast<std::size_t>(linear) * 2u;
    const std::uint16_t raw = read16(state.flags198E0, flagOffset);
    const std::uint16_t index = static_cast<std::uint16_t>(raw & 0x003Fu);

    state.value764D = state.palette25822[static_cast<std::size_t>(index) * 2u];
    state.value7651 = state.palette25822[static_cast<std::size_t>(index) * 2u + 1u];

    ops.call2975C(code, arg3, arg4, state.drawContext, 0);

    // 0x0BE7..0x0BF5 restores the two globals to fixed defaults.
    state.value764D = 0xB1u;
    state.value7651 = 0xB5u;
}

} // namespace ravenloft::game
