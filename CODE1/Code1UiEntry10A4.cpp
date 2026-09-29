#include "Code1UiEntry10A4.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

struct SavedRect10A4 {
    std::uintptr_t buffer = 0;
    std::int32_t x = 0;
    std::int32_t y = 0;
    std::int32_t width = 0;
    std::int32_t height = 0;
};

std::int32_t interpolate12(std::int32_t base,
                           std::int32_t target,
                           std::int32_t step) noexcept {
    return base + (((target - base) * step) + 6) / 12;
}

void restore(const SavedRect10A4& r, const Code1UiEntryOps10A4& ops) {
    ops.call6744(r.buffer, r.x, r.y, r.width, r.height);
}

} // namespace

void Code1UiEntry10A4::animate(std::int32_t characterPanelStage,
                               Code1UiEntryState10A4& state,
                               const Code1UiEntryOps10A4& ops) {
    std::array<SavedRect10A4, 2> saved{};
    saved[0].buffer = ops.call71C8(0x3540);
    saved[1].buffer = ops.call71C8(0x3540);

    for (std::int32_t pass = 0; pass < 2; ++pass) {
        ops.call6588(state.buffer7138);

        if (state.mode4D0 == 0) {
            ops.call86C4(0, 0, 0x18, 7);
        }
        ops.call86C4(0xC8, 0, 0x19, 7);

        ops.draw.call6C60(0, 5, 0, 0x2A, 7);
        ops.draw.call7D80(5, 0, 0x140, state.text7140, 1);

        for (std::int32_t slot = 0; slot < 9; ++slot) {
            const auto i = static_cast<std::size_t>(slot);
            ops.draw.call6C60(0, state.x87C[i], state.y880[i],
                              slot + 0x1E, 7);
        }

        if (characterPanelStage != 0) {
            ops.call04B0(characterPanelStage);
        }

        ops.call53E4(state.buffer7138);
        state.buffer7138 ^= 1u;
    }

    (void)ops.call954C(0x15, 7);
    (void)ops.call954C(0x27, 7);
    (void)ops.call954C(0x28, 7);
    (void)ops.call954C(0x29, 7);

    for (std::int32_t slot = 0; slot < 9; ++slot) {
        const auto i = static_cast<std::size_t>(slot);
        if (state.current8F8[i] == -1) {
            continue;
        }

        const std::int32_t baseY = 0xA0;
        const std::int32_t baseX = -0x38;
        std::int32_t currentY = baseY;
        std::int32_t currentX = baseX;
        const std::int32_t targetY = state.y880[i] + 0x18;
        const std::int32_t targetX = state.x87C[i] + 0x3D;
        std::int32_t sequenceIndex = 0;

        for (std::int32_t frame = 0; frame < 12; ++frame) {
            ops.call6588(state.buffer7138);
            const auto page = static_cast<std::size_t>(state.buffer7138);

            if (frame >= 2) {
                restore(saved[page], ops);
            }

            const auto resource =
                ops.call954C(state.animation964[static_cast<std::size_t>(sequenceIndex)], 7);

            if (state.flag970 != 0) {
                ops.callA5C8(0, resource.handle);
            }

            SavedRect10A4& r = saved[page];
            r.x = currentX - static_cast<std::int32_t>(resource.word4 / 2u);
            r.y = currentY - static_cast<std::int32_t>(resource.byte6) * 2;
            r.width = static_cast<std::int32_t>(resource.word4);
            r.height = static_cast<std::int32_t>(resource.byte6) * 8;

            ops.call66C0(r.buffer, r.x, r.y, r.width, r.height);
            ops.call6D54(0, r.x, r.y, resource.handle);

            if (state.flag970 != 0) {
                ops.callA5C8(0, resource.handle);
            }

            sequenceIndex = (sequenceIndex + 1) % 12;

            const std::int32_t step = frame + 1;
            currentY = interpolate12(baseY, targetY, step);
            currentX = interpolate12(baseX, targetX, step);

            ops.call53E4(state.buffer7138);
            state.buffer7138 ^= 1u;
        }

        ops.call6588(state.buffer7138);
        restore(saved[static_cast<std::size_t>(state.buffer7138)], ops);
        ops.draw.call6C60(0, state.x87C[i], state.y880[i], 0x15, 7);
        ops.call53E4(state.buffer7138);
        state.buffer7138 ^= 1u;

        ops.call6588(state.buffer7138);
        restore(saved[static_cast<std::size_t>(state.buffer7138)], ops);
        ops.draw.call6C60(0, state.x87C[i], state.y880[i], 0x15, 7);
        ops.call53E4(state.buffer7138);
        state.buffer7138 ^= 1u;
    }

    ops.call7240(saved[0].buffer);
    ops.call7240(saved[1].buffer);
}

} // namespace ravenloft::game
