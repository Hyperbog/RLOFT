#include "UiTransition07E4.h"

#include "UiTextComposite00EC.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

bool inResourceWindow(std::int32_t value, std::int32_t base) noexcept {
    return value >= base && value < base + 0x2E;
}

std::int32_t animatedY(std::int32_t y, std::int32_t amount) noexcept {
    std::int32_t eax = ((amount * 4) - amount) << 4;
    eax += 3;
    eax /= 6;
    const std::int32_t sign = eax < 0 ? -1 : 0;
    eax -= sign;
    eax >>= 1;
    return y + 0x18 - eax;
}

void drawTextFields(UiTransitionState07E4& state,
                    const TextDatabase& text,
                    const UiTransitionOps07E4& ops) {
    if (state.textIndex940 != -1) {
        const auto& label = text.at(static_cast<std::size_t>(state.textIndex940));
        UiTextComposite00EC::draw(0x114, 0x10C, 0x7A, 0x30,
                                       label.c_str(), 0, 0x5FF8, ops.draw);
    }
    if (state.textIndex948 != -1) {
        const auto& label = text.at(static_cast<std::size_t>(state.textIndex948));
        UiTextComposite00EC::draw(0x114, 0xD7, 0x7A, 0x30,
                                       label.c_str(), 0, 0x5FF8, ops.draw);
    }
}

} // namespace

void UiTransition07E4::animate(
    std::int32_t startIndex,
    std::int32_t endIndex,
    UiTransitionState07E4& state,
    const TextDatabase& text,
    const UiTransitionOps07E4& ops) {
    std::array<std::uint8_t, 9> counters{};

    ops.call8E3C(0x14, 7);

    for (std::int32_t index = startIndex; index < endIndex; ++index) {
        const auto slot = static_cast<std::size_t>(index);
        const std::int32_t value = state.current8F8[slot];
        if (value < 0) continue;
        if (state.mode4D0 == 0 && value == 1) continue;
        if (index == 7 && value != 0x14) continue;
        if (inResourceWindow(value, state.global713C)) {
            ops.call8E3C(value, 1);
        } else {
            ops.call8E3C(value, 7);
        }
    }

    std::int32_t frame = 0;

    while (counters[static_cast<std::size_t>(endIndex - 1)] != 0x0B) {
        ops.call6588(state.buffer7138);

        if (state.mode4D0 == 0 && frame < 2) {
            ops.call86C4(0xC8, 0, 0x19, 7);
        }

        for (std::int32_t index = startIndex;
             index < endIndex && (index - startIndex) <= frame;
             ++index) {
            ++counters[static_cast<std::size_t>(index)];
        }

        for (std::int32_t index = startIndex; index < endIndex; ++index) {
            const auto slot = static_cast<std::size_t>(index);
            const std::int32_t counter = counters[slot];
            std::int32_t x = state.x87C[slot];
            std::int32_t y = state.y880[slot];
            const std::int32_t current = state.current8F8[slot];
            const std::int32_t previous = state.previous8F4[slot];

            if (current == previous) {
                if (current == -1) continue;
                if (index == 7 && current != 0x14) {
                    ops.call67C8(0, x, y, state.specialResource958);
                    continue;
                }
                if (inResourceWindow(previous, state.global713C)) {
                    ops.draw.call6C60(0, x, y, 0x14, 7);
                    ops.draw.call6EB0(0, x + 0x15, y, current, 1, 0x5AF8);
                    continue;
                }
                ops.draw.call6EB0(0, x, y, current, 7, 0x5FF8);
                continue;
            }

            if (current == -1) continue;

            if (counter == 0) {
                if (state.mode4D0 == 0 && previous == 1) {
                    ops.call67C8(0, x, y, state.resource954[slot]);
                    continue;
                }
                if (inResourceWindow(previous, state.global713C)) {
                    ops.draw.call6C60(0, x, y, 0x14, 7);
                    ops.draw.call6EB0(0, x + 0x15, y, previous, 1, 0x5AF8);
                    continue;
                }
                if (index == 7 && previous != 0x15) {
                    ops.call67C8(0, x, y, state.resource954[0]);
                    continue;
                }
                ops.draw.call6C60(0, x, y, previous, 7);
                continue;
            }

            if (counter > 0 && counter < 6) {
                ops.draw.call6C60(0, x, y, index + 0x1E, 7);
                const std::int32_t amount = 6 - counter;
                const std::int32_t animY = animatedY(y, amount);

                if (state.mode4D0 == 0 && previous == 1) {
                    ops.call9C44(0, amount, 0x0A, 1, 1,
                                 state.resource954[slot], x, animY);
                    continue;
                }
                if (inResourceWindow(previous, state.global713C)) {
                    const auto baseResource = ops.call954C(0x14, 7);
                    ops.call9C44(0, amount, 0x0A, 1, 1,
                                 baseResource, x, animY);
                    const auto overlayResource = ops.call954C(previous, 1);
                    ops.call9C44(1, amount, 0x0A, 1, 1,
                                 overlayResource, x + 0x15, animY);
                    continue;
                }
                if (index == 7 && previous != 0x15) {
                    ops.call9C44(0, amount, 0x0A, 1, 1,
                                 state.resource954[0], x, animY);
                    continue;
                }
                const auto resource = ops.call954C(previous, 7);
                ops.call9C44(0, amount, 0x0A, 1, 1, resource, x, animY);
                continue;
            }

            const std::int32_t lowerY = y + 0x18;
            if (counter == 6) {
                ops.draw.call6C60(0, x, y, index + 0x1E, 7);
                ops.callA05C(x, lowerY, x + 0x79, lowerY, state.byte63F8);
                continue;
            }

            if (counter > 6 && counter < 12) {
                const std::int32_t mirrored = 12 - counter;
                ops.draw.call6C60(0, x, y, index + 0x1E, 7);
                const std::int32_t amount = 6 - mirrored;
                const std::int32_t animY = animatedY(y, amount);

                if (state.mode4D0 == 0 && current == 1) {
                    ops.call9C44(0, amount, 6, 1, 1,
                                 state.resource954[slot], x, animY);
                    continue;
                }
                if (index == 7 && current != 0x14) {
                    ops.call9C44(0, amount, 6, 1, 1,
                                 state.specialResource958, x, animY);
                    continue;
                }
                if (inResourceWindow(current, state.global713C)) {
                    const auto baseResource = ops.call954C(0x14, 7);
                    ops.call9C44(0, amount, 6, 1, 1,
                                 baseResource, x, animY);
                    const auto overlayResource = ops.call954C(current, 1);
                    ops.call9C44(1, amount, 6, 1, 1,
                                 overlayResource, x + 0x15, animY);
                    continue;
                }
                const auto resource = ops.call954C(current, 7);
                ops.call9C44(0, amount, 6, 1, 1, resource, x, animY);
                continue;
            }

            if (state.mode4D0 == 0 && current == 1) {
                ops.call67C8(0, x, y, state.resource954[slot]);
                continue;
            }
            if (index == 7 && current != 0x14) {
                ops.call67C8(0, x, y, state.specialResource958);
                continue;
            }
            if (inResourceWindow(current, state.global713C)) {
                ops.draw.call6C60(0, x, y, 0x14, 7);
                ops.draw.call6EB0(0, x + 0x15, y, current, 1, 0x5AF8);
                continue;
            }
            ops.draw.call6C60(0, x, y, current, 7);
        }

        if (frame < 2) {
            drawTextFields(state, text, ops);
            ops.call55DC(state.buffer7138, 0xEC, 0, 0xA4, 0x140);
        } else {
            ops.call53E4(state.buffer7138);
            state.buffer7138 ^= 1u;
        }

        ++frame;
    }

    ops.call6588(state.buffer7138);
    ops.call064C();
    ops.call53E4(state.buffer7138);
    state.buffer7138 ^= 1u;
}

} // namespace ravenloft::game
