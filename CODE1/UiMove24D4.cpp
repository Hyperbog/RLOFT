#include "UiMove24D4.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

struct SavedPage24D4 {
    std::uintptr_t buffer = 0;
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
    saved[0].buffer = ops.call71C8(0x1E80);
    saved[1].buffer = ops.call71C8(0x1E80);

    const auto index = static_cast<std::size_t>(slot);

    ops.call6588(state.buffer7138);
    ops.draw.call6C60(0, state.x87C[index], state.y880[index], slot + 0x1E, 7);

    const std::int32_t startX = state.x87C[index];
    const std::int32_t startY = state.y880[index];
    for (auto& page : saved) {
        page.x = startX;
        page.y = startY;
        ops.call66C0(page.buffer, startX, startY, 0x7A, 0x30);
    }

    const std::int32_t deltaX = (targetX - startX) / 8;
    const std::int32_t deltaY = (targetY - startY) / 8;

    std::int32_t x = startX;
    std::int32_t y = startY;

    for (std::int32_t frame = 0; frame <= 8; ++frame) {
        ops.call6588(state.buffer7138);
        auto& page = saved[static_cast<std::size_t>(state.buffer7138)];

        ops.call6744(page.buffer, page.x, page.y, 0x7A, 0x30);
        page.x = x;
        page.y = y;
        ops.call66C0(page.buffer, x, y, 0x7A, 0x30);

        const auto resource = ops.call954C(state.current8F8[index], 7);
        ops.call67C8(0, x, y, resource.handle);

        y += deltaY;
        x += deltaX;

        ops.call53E4(state.buffer7138);
        state.buffer7138 ^= 1u;
    }

    ops.call7240(saved[0].buffer);
    ops.call7240(saved[1].buffer);
}

} // namespace ravenloft::game
