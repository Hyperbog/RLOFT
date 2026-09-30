#include "UiTransition07E4.h"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::int32_t wrapAdd(std::int32_t a, std::uint32_t b) noexcept {
    return std::bit_cast<std::int32_t>(std::bit_cast<std::uint32_t>(a) + b);
}
std::int32_t wrapSub(std::int32_t a, std::int32_t b) noexcept {
    return std::bit_cast<std::int32_t>(std::bit_cast<std::uint32_t>(a) -
                                       std::bit_cast<std::uint32_t>(b));
}

bool inResourceWindow(std::int32_t value, std::int32_t base) noexcept {
    // CMP/JL followed by ADD 2Eh and CMP/JLE: signed comparisons around a
    // modulo-2^32 ADD, not mathematical unbounded addition.
    return value >= base && value < wrapAdd(base, 0x2Eu);
}

std::int32_t animatedY(std::int32_t y, std::int32_t amount) noexcept {
    // Exact arithmetic shape used repeatedly at 0x0A35, 0x0AB9, 0x0B4E,
    // 0x0C7A, 0x0CCE, 0x0D56 and 0x0DDF.  LEA/SUB/SHL/LEA wrap at 32 bits;
    // IDIV 6 is signed truncation toward zero, followed by Watcom's explicit
    // sign correction implementing another signed divide by two.
    std::uint32_t u = std::bit_cast<std::uint32_t>(amount);
    u = (u * 4u) - u;
    u <<= 4;
    u += 3u;
    std::int32_t q = std::bit_cast<std::int32_t>(u) / 6;
    q = static_cast<std::int32_t>(static_cast<std::int64_t>(q) / 2);
    std::uint32_t out = std::bit_cast<std::uint32_t>(y) + 0x18u;
    out -= std::bit_cast<std::uint32_t>(q);
    return std::bit_cast<std::int32_t>(out);
}

void drawTextFields(UiTransitionState07E4& state,
                    const UiTransitionOps07E4& ops) {
    // 0x0FAD..0x1026 calls the original 0x004C decoder into DS:55F8 before
    // each 0x00EC draw.  Do not replace this with a pre-decoded text database.
    if (state.textIndex940 != -1) {
        ops.call004C(state.textBuffer55F8, state.textIndex940);
        UiTextComposite00EC::draw(0x114, 0x10C, 0x7A, 0x30,
                                  state.textBuffer55F8, 0, 0x5FF8, ops.draw);
    }
    if (state.textIndex948 != -1) {
        ops.call004C(state.textBuffer55F8, state.textIndex948);
        UiTextComposite00EC::draw(0x114, 0xD7, 0x7A, 0x30,
                                  state.textBuffer55F8, 0, 0x5FF8, ops.draw);
    }
}

} // namespace

void UiTransition07E4::animate(
    std::int32_t startIndex,
    std::int32_t endIndex,
    UiTransitionState07E4& state,
    const UiTransitionOps07E4& ops) {
    // 0x07ED..0x07FA: memset(local counters,0,9).
    std::array<std::uint8_t, 9> counters{};

    // 0x07FD..0x0806.
    ops.call8E3C(0x14, 7);

    // 0x0809..0x0887: preload every eligible current slot in [start,end).
    for (std::int32_t index = startIndex; index < endIndex; ++index) {
        const auto slot = static_cast<std::size_t>(index);
        const std::int32_t value = state.current8F8[slot];
        if (value < 0) continue;
        if (state.mode4D0 == 0 && value == 1) continue;
        if (index == 7 && value != 0x14) continue;
        ops.call8E3C(value, inResourceWindow(value, state.global713C) ? 1 : 7);
    }

    std::int32_t frame = 0;

    // 0x0889..0x106A.  The original termination byte is local[endIndex-1].
    while (counters[static_cast<std::size_t>(endIndex - 1)] != 0x0B) {
        // MOVZX BYTE [7138] before 0x6588: only the low byte is passed here.
        ops.call6588(static_cast<std::uint8_t>(state.buffer7138));

        if (state.mode4D0 == 0 && frame < 2) {
            ops.call86C4(0xC8, 0, 0x19, 7);
        }

        // 0x08C7..0x08DE: increment a growing prefix through frame+1 slots.
        for (std::int32_t index = startIndex;
             index < endIndex && wrapSub(index, startIndex) <= frame;
             ++index) {
            ++counters[static_cast<std::size_t>(index)]; // byte INC wraps
        }

        for (std::int32_t index = startIndex; index < endIndex; ++index) {
            const auto slot = static_cast<std::size_t>(index);
            std::int32_t counter = counters[slot];
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
                    ops.draw.call6EB0(0, wrapAdd(x, 0x15u), y,
                                      current, 1, 0x5AF8);
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
                    ops.draw.call6EB0(0, wrapAdd(x, 0x15u), y,
                                      previous, 1, 0x5AF8);
                    continue;
                }
                if (index == 7 && previous != 0x15) {
                    // DS:0954 is the first element of the 4-byte resource table.
                    ops.call67C8(0, x, y, state.resource954[0]);
                    continue;
                }
                ops.draw.call6C60(0, x, y, previous, 7);
                continue;
            }

            if (counter > 0 && counter < 6) {
                ops.draw.call6C60(0, x, y, wrapAdd(index, 0x1Eu), 7);
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
                                 overlayResource, wrapAdd(x, 0x15u), animY);
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

            const std::int32_t lowerY = wrapAdd(y, 0x18u);
            if (counter == 6) {
                ops.draw.call6C60(0, x, y, wrapAdd(index, 0x1Eu), 7);
                ops.callA05C(x, lowerY, wrapAdd(x, 0x79u), lowerY,
                              state.byte63F8);
                continue;
            }

            if (counter > 6 && counter < 12) {
                // 0x0C45 stores 12-counter back into the local counter value.
                counter = 12 - counter;
                ops.draw.call6C60(0, x, y, wrapAdd(index, 0x1Eu), 7);
                const std::int32_t amount = 6 - counter;
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
                                 overlayResource, wrapAdd(x, 0x15u), animY);
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
                ops.draw.call6EB0(0, wrapAdd(x, 0x15u), y,
                                  current, 1, 0x5AF8);
                continue;
            }
            ops.draw.call6C60(0, x, y, current, 7);
        }

        if (frame < 2) {
            drawTextFields(state, ops);
            // 0x103A pushes the complete DWORD at DS:7138.
            ops.call55DC(state.buffer7138, 0xEC, 0, 0xA4, 0x140);
        } else {
            // 0x104A also pushes the complete DWORD, then XOR changes only byte 0.
            ops.call53E4(state.buffer7138);
            state.buffer7138 ^= 1u;
        }

        ++frame; // INC DWORD: modulo 2^32 in the original valid-state path
    }

    ops.call6588(static_cast<std::uint8_t>(state.buffer7138));
    ops.call064C();
    ops.call53E4(state.buffer7138);
    state.buffer7138 ^= 1u;
    // 0x10A2..0x10A3 is MOV EAX,EAX alignment padding.
}

} // namespace ravenloft::game
