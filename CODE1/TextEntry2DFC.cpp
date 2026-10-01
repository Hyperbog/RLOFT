#include "TextEntry2DFC.h"

#include <cstdint>

namespace ravenloft::game {
namespace {
std::uint32_t add32(std::uint32_t a, std::uint32_t b) noexcept { return a + b; }
std::uint32_t mul32(std::uint32_t a, std::uint32_t b) noexcept { return a * b; }
std::uint32_t nameBase(const TextEntryOps2DFC& ops) {
    return add32(mul32(ops.readDword(0x04CC), 0x248u), 0x67F8u);
}
}

void TextEntry2DFC::run(std::int32_t x,
                        std::int32_t maxCharacters,
                        TextEntryState2DFC&,
                        const TextEntryOps2DFC& ops) {
    std::uint32_t blink = 1; // [EBP-10]; only its low byte is XORed at 2EC9.
    (void)ops.call7F50();

    std::int32_t key = 0;    // EBX after XOR EBX,EBX
    std::int32_t length = 0; // ESI

    // 2E15..2E3A: memset 16 bytes, then redundantly clear byte zero.
    ops.call84A0(nameBase(ops), 0, 0x10);
    ops.writeByte(nameBase(ops), 0);
    ops.call7D68(x, 0xA0);

    while (key != 0x0D) {
        ops.call6588(static_cast<std::uint32_t>(ops.readByte(0x7138)));
        const std::int32_t cursorX = ops.call7DE8();
        key = cursorX; // EBX receives EAX at 2E70 until 82C0 later replaces it.
        ops.call9EB0(x, cursorX, 0x0A, 1, 0);

        std::uint32_t deadline = ops.call7F50() + 9u;
        // PUSH DWORD PTR DS:7138, not a byte load.
        ops.call550C(ops.readDword(0x7138), x, 0x88, 0x0A, 0x30);
        ops.writeByte(0x7138, static_cast<std::uint8_t>(ops.readByte(0x7138) ^ 1u));

        while (ops.call831C() == 0) {
            const std::uint32_t now = ops.call7F50();
            // CMP EDI,EAX / JAE: unsigned deadline >= now.
            if (deadline >= now) continue;

            deadline = ops.call7F50() + 9u;
            blink = (blink & 0xFFFFFF00u) |
                    static_cast<std::uint8_t>(static_cast<std::uint8_t>(blink) ^ 1u);
            ops.call6588(static_cast<std::uint32_t>(ops.readByte(0x7138)));
            ops.call9EB0(x, cursorX, 0x0A, 1, blink != 0 ? 0 : 0x5F);
            ops.call53E4(ops.readDword(0x7138));
            ops.writeByte(0x7138, static_cast<std::uint8_t>(ops.readByte(0x7138) ^ 1u));
        }

        ops.call6588(static_cast<std::uint32_t>(ops.readByte(0x7138)));
        blink = 0;
        ops.call9EB0(x, cursorX, 0x0A, 1, 0x5F);

        key = ops.call82C0();
        const std::uint8_t bl = static_cast<std::uint8_t>(key);

        if (key == 8) {
            if (length != 0) {
                ops.call9EB0(x, 0x88, 0x0A, 0x30, 0x5F);
                --length;
                ops.writeByte(add32(nameBase(ops), static_cast<std::uint32_t>(length)), 0);
            }
        } else {
            const bool accepted =
                (key >= 0x41 && key <= 0x5A) || key == 0x20 ||
                (key >= 0x30 && key <= 0x39);

            if (accepted && length < maxCharacters) {
                const std::uint32_t base = nameBase(ops);
                const std::int32_t stringWidth = ops.call7AB4(base, 0);
                // MOVSX EAX,BL: glyph lookup uses signed low byte, not full key.
                const std::int32_t signedBl = static_cast<std::int8_t>(bl);
                const std::int32_t glyphWidth = ops.call7B50(signedBl, 0);
                if (static_cast<std::int32_t>(
                        static_cast<std::uint32_t>(stringWidth) +
                        static_cast<std::uint32_t>(glyphWidth)) < 0x30) {
                    // No 16-byte C++ safety check exists in CODE.1.
                    ops.writeByte(add32(nameBase(ops), static_cast<std::uint32_t>(length)), bl);
                    ++length;
                }
            }
        }

        ops.call9EB0(x, 0x88, 0x0A, 0x30, 0x5F);
        ops.call7D80(x, 0x88, 0x30, nameBase(ops), 0);
    }
}

} // namespace ravenloft::game
