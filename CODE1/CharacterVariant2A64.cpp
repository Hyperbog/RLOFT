#include "CharacterVariant2A64.h"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::uint32_t add32(std::uint32_t a, std::uint32_t b) { return a + b; }
std::uint32_t mul32(std::uint32_t a, std::uint32_t b) { return a * b; }
std::int32_t sx32(std::uint32_t v) { return std::bit_cast<std::int32_t>(v); }
std::int32_t sx8(std::uint8_t v) {
    return (v & 0x80u) ? static_cast<std::int32_t>(v) - 0x100
                       : static_cast<std::int32_t>(v);
}

std::uint8_t frameEnabled(const std::array<std::uint8_t, 6>& enabled,
                          std::int32_t selection,
                          const CharacterVariantOps2A64& ops) {
    if (selection >= 1 && selection <= 6)
        return enabled[static_cast<std::size_t>(selection - 1)];
    return ops.readFrameByte(selection - 0x25);
}

} // namespace

void CharacterVariant2A64::run(CharacterVariantState2A64&,
                                const CharacterVariantOps2A64& ops) {
    std::int32_t firstPage = 1;                 // [EBP-1C]
    std::uint32_t baseOffset = 0;               // [EBP-14]
    std::int32_t exitState = 0;                 // EDI

    const std::uint32_t selected = ops.readDword(0x04CC);
    const std::uint32_t selectedScaled = mul32(selected, 0x248u);
    const std::uint32_t category = ops.readByte(add32(selectedScaled, 0x680Bu));

    while (exitState == 0) {                    // 2A91
        ops.call6588(ops.readByte(0x7138));

        if (firstPage == 0) {
            for (std::uint32_t eax = 0; eax < 6; ++eax) {
                const std::uint32_t stride = mul32(eax, 8u);
                const std::uint32_t v = ops.readDword(add32(0x08F8u, stride));
                ops.writeDword(add32(0x08F4u, stride), v);
            }
        }

        std::array<std::uint8_t, 6> enabled{};  // [EBP-24..EBP-1F]
        std::array<std::int32_t, 9> slots{};

        for (std::uint32_t eax = 0; eax < 6; ++eax) {
            std::uint32_t ebx = ops.readDword(0x04CC);
            // XOR BL,1 changes only the low byte; upper 24 bits survive.
            ebx = (ebx & 0xFFFFFF00u) | ((ebx ^ 1u) & 0xFFu);
            const std::uint32_t edx = mul32(ebx, 0x248u);
            const std::uint32_t otherCategory = ops.readByte(add32(edx, 0x680Bu));
            bool blocked = false;
            if (otherCategory == category && ops.readByte(add32(ebx, 0x04C4u)) != 0) {
                const std::int32_t otherVariant = sx8(ops.readByte(add32(edx, 0x680Au)));
                const std::uint32_t candidate = add32(baseOffset, eax);
                blocked = otherVariant == sx32(candidate);
            }
            const std::uint32_t candidate = add32(baseOffset, eax);
            if (blocked || candidate == 0x11u) {
                slots[eax] = 0x15;
                ops.writeDword(add32(0x08F8u, mul32(eax, 8u)), 0x15u);
                enabled[eax] = 0;
            } else {
                const std::uint32_t resource = add32(
                    add32(ops.readDword(0x713C), mul32(category, 0x17u)), candidate);
                slots[eax] = sx32(resource);
                ops.writeDword(add32(0x08F8u, mul32(eax, 8u)), resource);
                enabled[eax] = 1;
            }
        }

        if (firstPage != 0) {
            slots[6] = slots[7] = slots[8] = -1;
            ops.call1464(4, slots, 0x0D, 4, 0x2A);
            firstPage = 0;
        } else {
            ops.call07E4(0, 6);
        }

        PointerSample043C pointer{};
        do { pointer = ops.pollPointer(); } while (pointer.buttons != 0);

        exitState = 0;
        std::int32_t previous = -1;             // ESI
        for (;;) {                              // 2BB8
            pointer = ops.pollPointer();
            if (pointer.buttons == 0) {
                const std::int32_t current = ops.call035C(pointer.x, pointer.y);
                if (previous == current) continue;
                ops.call6588(ops.readByte(0x7138));

                if (previous > 0 && previous != 0xFF && frameEnabled(enabled, previous, ops) != 0) {
                    const std::uint32_t i = static_cast<std::uint32_t>(previous);
                    const std::uint32_t xBits = ops.readDword(add32(0x0874u, mul32(i, 8u)));
                    const std::int32_t x = sx32(xBits);
                    const std::int32_t y = sx32(ops.readDword(add32(0x0878u, mul32(i, 8u))));
                    ops.draw.call6C60(0, x, y, 0x14, 7);
                    const std::uint32_t resource = add32(add32(add32(
                        ops.readDword(0x713C), mul32(category, 0x17u)), i - 1u), baseOffset);
                    const std::int32_t x15 = sx32(add32(xBits, 0x15u));
                    ops.draw.call6EB0(0, x15, y, sx32(resource), 1, 0x5AF8);
                    ops.call55DC(ops.readDword(0x7138), x15, y, 0x50, 0x30);
                }

                if (current > 0 && current != 0xFF && frameEnabled(enabled, current, ops) != 0) {
                    const std::uint32_t i = static_cast<std::uint32_t>(current);
                    const std::uint32_t xBits = ops.readDword(add32(0x0874u, mul32(i, 8u)));
                    const std::int32_t x = sx32(xBits);
                    const std::int32_t y = sx32(ops.readDword(add32(0x0878u, mul32(i, 8u))));
                    ops.draw.call6C60(0, x, y, 0x14, 7);
                    const std::uint32_t resource = add32(add32(add32(
                        ops.readDword(0x713C), mul32(category, 0x17u)), i - 1u), baseOffset);
                    const std::int32_t x15 = sx32(add32(xBits, 0x15u));
                    // Original 2CC2..2CE9 calls 0x6C60 here, not 0x6EB0.
                    ops.draw.call6C60(0, x15, y, sx32(resource), 1);
                    ops.call55DC(ops.readDword(0x7138), x15, y, 0x50, 0x30);
                }
                previous = current;
                continue;
            }

            const std::int32_t selection = ops.call035C(pointer.x, pointer.y);
            if (selection < 0) {
                previous = -1;
                continue;
            }

            if (selection != 0 && selection != 0xFF) {
                if (frameEnabled(enabled, selection, ops) == 0) continue;

                const std::uint8_t dl = static_cast<std::uint8_t>(
                    static_cast<std::uint8_t>(selection) - std::uint8_t{1});
                const std::uint8_t bl = static_cast<std::uint8_t>(
                    static_cast<std::uint8_t>(baseOffset) + dl);
                const std::uint32_t cur = ops.readDword(0x04CC);
                ops.writeByte(add32(mul32(cur, 0x248u), 0x680Au), bl);
                ops.call15B0(selection - 1, 0x114, 0xD7, 5);
                exitState = 2;
                ops.writeDword(0x04D0, ops.readDword(0x04D0) + 1u);
            } else if (selection == 0xFF) {
                const std::int32_t dividend = sx32(add32(baseOffset, 6u));
                baseOffset = static_cast<std::uint32_t>(dividend % 0x12);
                exitState = 1;
            } else {
                ops.writeDword(0x04D0, ops.readDword(0x04D0) - 1u);
                ops.call15B0(-1, -1, -1, -1);
                exitState = 2;
            }

            if (exitState != 0) {
                --exitState;
                break;
            }
        }
    }
}

} // namespace ravenloft::game
