#include "Code1CharacterVariant2A64.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

Code1UiHitTestState035C hitState(const Code1CharacterVariantState2A64& state) {
    Code1UiHitTestState035C h{};
    h.mode4D0 = state.mode4D0;
    h.panelX = state.x87C;
    h.panelY = state.y880;
    h.specialX8E4 = state.specialX8E4;
    h.specialY8A0 = state.specialY8A0;
    return h;
}

void drawHover(std::int32_t selection,
               std::int32_t baseOffset,
               std::uint8_t category,
               const std::array<std::uint8_t, 6>& enabled,
               const Code1CharacterVariantState2A64& state,
               const Code1CharacterVariantOps2A64& ops) {
    if (selection <= 0 || selection == 0xFF || selection > 6) return;
    const auto index = static_cast<std::size_t>(selection - 1);
    if (enabled[index] == 0) return;

    const std::int32_t x = state.x87C[index];
    const std::int32_t y = state.y880[index];
    ops.draw.call6C60(0, x, y, 0x14, 7);
    const std::int32_t resource = state.global713C +
        static_cast<std::int32_t>(category) * 0x17 +
        baseOffset + static_cast<std::int32_t>(index);
    ops.draw.call6EB0(0, x + 0x15, y, resource, 1, 0x5AF8);
    ops.call55DC(state.buffer7138, x + 0x15, y, 0x50, 0x30);
}

} // namespace

void Code1CharacterVariant2A64::run(
    Code1CharacterVariantState2A64& state,
    const Code1CharacterVariantOps2A64& ops) {
    bool firstPage = true;
    std::int32_t baseOffset = 0;
    std::int32_t exitState = 0;

    const auto selected = static_cast<std::size_t>(state.selectedCharacter4CC);
    const std::uint8_t category = state.characters[selected].field680B;

    while (exitState == 0) {
        ops.call6588(state.buffer7138);

        if (!firstPage) {
            for (std::size_t i = 0; i < 6; ++i) {
                state.previous8F4[i] = state.current8F8[i];
            }
        }

        std::array<std::uint8_t, 6> enabled{};
        std::array<std::int32_t, 9> slots{
            -1,-1,-1,-1,-1,-1,-1,-1,-1
        };

        const auto other = static_cast<std::size_t>(state.selectedCharacter4CC ^ 1);
        for (std::int32_t i = 0; i < 6; ++i) {
            const auto index = static_cast<std::size_t>(i);
            const std::int32_t candidate = baseOffset + i;

            const bool duplicateOther =
                state.characters[other].field680B == category &&
                state.characters[other].active4C4 &&
                static_cast<std::int32_t>(state.characters[other].field680A) == candidate;

            if (duplicateOther || candidate == 0x11) {
                slots[index] = 0x15;
                enabled[index] = 0;
            } else {
                slots[index] = state.global713C +
                    static_cast<std::int32_t>(category) * 0x17 + candidate;
                enabled[index] = 1;
            }
            state.current8F8[index] = slots[index];
        }

        if (firstPage) {
            ops.call1464(4, slots, 0x0D, 4, 0x2A);
            state.previous8F4.fill(0x15);
            state.current8F8 = slots;
            firstPage = false;
        } else {
            ops.call07E4(0, 6);
        }

        Code1PointerSample043C pointer{};
        do {
            pointer = ops.pollPointer();
        } while (pointer.buttons != 0);

        std::int32_t previousHover = -1;

        for (;;) {
            pointer = ops.pollPointer();
            const auto h = hitState(state);

            if (pointer.buttons == 0) {
                const std::int32_t hover =
                    Code1UiHitTest035C::hitTest(pointer.x, pointer.y, h);
                if (hover == previousHover) continue;

                ops.call6588(state.buffer7138);
                drawHover(previousHover, baseOffset, category, enabled, state, ops);
                drawHover(hover, baseOffset, category, enabled, state, ops);
                previousHover = hover;
                continue;
            }

            const std::int32_t selection =
                Code1UiHitTest035C::hitTest(pointer.x, pointer.y, h);
            if (selection < 0) continue;

            if (selection > 0 && selection != 0xFF) {
                if (selection > 6 || enabled[static_cast<std::size_t>(selection - 1)] == 0) {
                    continue;
                }

                state.characters[selected].field680A = static_cast<std::int8_t>(
                    baseOffset + selection - 1);
                ops.call15B0(selection - 1, 0x114, 0xD7, 5);
                ++state.mode4D0;
                exitState = 2;
            } else if (selection == 0xFF) {
                baseOffset = (baseOffset + 6) % 0x12;
                exitState = 1;
            } else {
                ops.call15B0(-1, -1, -1, -1);
                --state.mode4D0;
                exitState = 2;
            }

            if (exitState != 0) break;
        }

        --exitState;
    }
}

} // namespace ravenloft::game
