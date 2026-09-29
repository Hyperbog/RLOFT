#include "UiExit15B0.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

bool inResourceWindow(std::int32_t value, std::int32_t base) noexcept {
    return value >= base && value < base + 0x2E;
}

std::int32_t interpolate8(std::int32_t base,
                          std::int32_t target,
                          std::int32_t step) noexcept {
    return base + (((target - base) * step) + 4) / 8;
}

struct SavedPage15B0 {
    std::uintptr_t buffer = 0;
    std::int32_t x = 0;
    std::int32_t y = 0;
};

} // namespace

void UiExit15B0::animate(std::int32_t selectedSlot,
                              std::int32_t targetX,
                              std::int32_t targetY,
                              std::int32_t characterPanelStage,
                              UiExitState15B0& state,
                              const UiExitOps15B0& ops) {
    ops.call56DC();

    for (std::int32_t intensity = 0; intensity < 5; ++intensity) {
        ops.call6588(state.buffer7138);

        for (std::int32_t slot = 0; slot < 9; ++slot) {
            if (slot == selectedSlot) continue;
            const auto i = static_cast<std::size_t>(slot);
            const std::int32_t value = state.current8F8[i];
            if (value == -1) continue;

            const std::int32_t layerOffset = intensity << 8;
            const std::int32_t surface5FF8 = 0x5FF8 + layerOffset;

            if (state.mode4D0 == 0) {
                ops.call6824(0, state.x87C[i], state.y880[i],
                             static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(state.resource954[i])),
                             surface5FF8);
                continue;
            }

            if (slot == 7) {
                ops.call6824(0, state.x87C[i], state.y880[i],
                             static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(state.specialResource958)),
                             surface5FF8);
                continue;
            }

            if (inResourceWindow(value, state.global713C)) {
                ops.draw.call6EB0(0, state.x87C[i], state.y880[i],
                                  0x14, 7, surface5FF8);
                const auto overlay = ops.call954C(value, 1);
                ops.call6824(0, state.x87C[i] + 0x15, state.y880[i],
                             overlay.handle, 0x5AF8 + layerOffset);
                continue;
            }

            ops.draw.call6EB0(0, state.x87C[i], state.y880[i],
                              value, 7, surface5FF8);
        }

        ops.call53E4(state.buffer7138);
        state.buffer7138 ^= 1u;
    }

    for (std::int32_t intensity = 4; intensity >= 0; --intensity) {
        ops.call6588(state.buffer7138);

        for (std::int32_t slot = 0; slot < 9; ++slot) {
            if (slot == selectedSlot) continue;
            const auto i = static_cast<std::size_t>(slot);
            if (state.current8F8[i] == -1) continue;

            ops.draw.call6EB0(0, state.x87C[i], state.y880[i],
                              slot + 0x1E, 7,
                              0x5FF8 + (intensity << 8));
        }

        if (intensity == 0) {
            ops.call5454(state.buffer7138);
        } else {
            ops.call53E4(state.buffer7138);
        }
        state.buffer7138 ^= 1u;
    }

    if (selectedSlot == -1) return;

    std::array<SavedPage15B0, 2> saved{};
    saved[0].buffer = ops.call71C8(0x1E80);
    saved[1].buffer = ops.call71C8(0x1E80);

    const auto selected = static_cast<std::size_t>(selectedSlot);

    ops.call6588(state.buffer7138);
    ops.draw.call6C60(0, state.x87C[selected], state.y880[selected],
                      selectedSlot + 0x1E, 7);

    const std::int32_t startX = state.x87C[selected];
    const std::int32_t startY = state.y880[selected];
    for (auto& page : saved) {
        page.x = startX;
        page.y = startY;
        ops.call66C0(page.buffer, startX, startY, 0x7A, 0x30);
    }

    std::int32_t currentX = startX;
    std::int32_t currentY = startY;

    for (std::int32_t frame = 0; frame <= 8; ++frame) {
        ops.call6588(state.buffer7138);
        auto& page = saved[static_cast<std::size_t>(state.buffer7138)];

        ops.call6744(page.buffer, page.x, page.y, 0x7A, 0x30);
        page.x = currentX;
        page.y = currentY;
        ops.call66C0(page.buffer, currentX, currentY, 0x7A, 0x30);

        const std::int32_t value = state.current8F8[selected];
        if (inResourceWindow(value, state.global713C)) {
            const auto baseResource = ops.call954C(0x14, 7);
            ops.call67C8(0, currentX, currentY, baseResource.handle);
            const auto overlay = ops.call954C(value, 1);
            ops.call67C8(0, currentX + 0x15, currentY, overlay.handle);
        } else {
            const auto resource = ops.call954C(value, 7);
            ops.call67C8(0, currentX, currentY, resource.handle);
        }

        const std::int32_t step = frame + 1;
        currentX = interpolate8(startX, targetX, step);
        currentY = interpolate8(startY, targetY, step);

        ops.call53E4(state.buffer7138);
        state.buffer7138 ^= 1u;
    }

    ops.call7240(saved[0].buffer);
    ops.call7240(saved[1].buffer);

    ops.call6588(state.buffer7138);
    ops.call86C4(0xC8, 0, 0x19, 7);
    ops.call04B0(characterPanelStage);
    ops.call5454(state.buffer7138);
    state.buffer7138 ^= 1u;
}

} // namespace ravenloft::game
