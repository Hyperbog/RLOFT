#include "UiMove24D4.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

struct SavedPage24D4 {
    std::uint32_t buffer = 0;
    std::int32_t x = 0;
    std::int32_t y = 0;
};

} // namespace

void UiMove24D4::animate(std::int32_t slot,
                         std::int32_t targetX,
                         std::int32_t targetY,
                         UiMoveState24D4& state,
                         const UiMoveOps24D4& ops) {
    ops.call56DC();
    std::array<SavedPage24D4, 2> saved{};
    saved[0].buffer = ops.call71C8(0x1E80u);
    saved[1].buffer = ops.call71C8(0x1E80u);

    const auto index = static_cast<std::size_t>(static_cast<std::uint32_t>(slot));

    // 0x2502 is MOVZX BYTE DS:7138.
    ops.call6588(static_cast<std::uint8_t>(state.buffer7138));
    ops.draw.call6C60(0, state.x87C[index], state.y880[index], slot + 0x1E, 7);

    const std::int32_t startX = state.x87C[index];
    const std::int32_t startY = state.y880[index];
    for (auto& page : saved) {
        page.x = startX;
        page.y = startY;
        ops.call66C0(page.buffer, startX, startY, 0x7A, 0x30);
    }

    // 0x2583..0x25AA implements signed division by eight with truncation toward
    // zero; C++ signed /8 has the same quotient semantics for these int32 values.
    const std::int32_t deltaX = (targetX - startX) / 8;
    const std::int32_t deltaY = (targetY - startY) / 8;

    std::int32_t x = startX;
    std::int32_t y = startY;

    for (std::int32_t frame = 0; frame <= 8; ++frame) {
        // 0x25AF reads only the low byte for 0x6588, then 0x25BF/25DC use the
        // complete DWORD as the stack-array index.
        ops.call6588(static_cast<std::uint8_t>(state.buffer7138));
        auto& page = saved[static_cast<std::size_t>(state.buffer7138)];

        ops.call6744(page.buffer, page.x, page.y, 0x7A, 0x30);
        page.x = x;
        page.y = y;
        ops.call66C0(page.buffer, x, y, 0x7A, 0x30);

        const auto resource = ops.call954C(state.current8F8[index], 7);
        ops.call67C8(0, x, y, resource.pointer);

        y += deltaY;
        x += deltaX;

        // PUSH DWORD DS:7138 at 0x262D, then XOR BYTE DS:7138,1 at 0x263B.
        ops.call53E4(state.buffer7138);
        state.buffer7138 = (state.buffer7138 & 0xFFFFFF00u) |
                           ((state.buffer7138 ^ 1u) & 0xFFu);
    }

    ops.call7240(saved[0].buffer);
    ops.call7240(saved[1].buffer);
}

} // namespace ravenloft::game
