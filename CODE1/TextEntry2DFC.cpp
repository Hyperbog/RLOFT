#include "TextEntry2DFC.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

void TextEntry2DFC::run(std::int32_t x,
                             std::int32_t maxCharacters,
                             TextEntryState2DFC& state,
                             const TextEntryOps2DFC& ops) {
    std::int32_t blink = 1;
    (void)ops.call7F50();

    std::int32_t lastKey = 0;
    std::int32_t length = 0;

    state.name67F8.fill('\0');

    ops.call7D68(x, 0xA0);

    while (lastKey != 0x0D) {
        ops.call6588(state.buffer7138);
        const std::int32_t cursorX = ops.call7DE8();
        ops.call9EB0(x, cursorX, 0x0A, 1, 0);

        std::uint32_t deadline = ops.call7F50() + 9u;
        ops.call550C(state.buffer7138, x, 0x88, 0x0A, 0x30);
        state.buffer7138 ^= 1u;

        while (ops.call831C() == 0) {
            const std::uint32_t now = ops.call7F50();
            if (deadline >= now) {
                continue;
            }

            deadline = ops.call7F50() + 9u;
            blink ^= 1;
            ops.call6588(state.buffer7138);
            ops.call9EB0(x, cursorX, 0x0A, 1, blink != 0 ? 0 : 0x5F);
            ops.call53E4(state.buffer7138);
            state.buffer7138 ^= 1u;
        }

        ops.call6588(state.buffer7138);
        blink = 0;
        ops.call9EB0(x, cursorX, 0x0A, 1, 0x5F);

        lastKey = ops.call82C0();

        if (lastKey == 8) {
            if (length != 0) {
                ops.call9EB0(x, 0x88, 0x0A, 0x30, 0x5F);
                --length;
                state.name67F8[static_cast<std::size_t>(length)] = '\0';
            }
        } else {
            const bool acceptedCharacter =
                (lastKey >= 'A' && lastKey <= 'Z') ||
                lastKey == ' ' ||
                (lastKey >= '0' && lastKey <= '9');

            if (acceptedCharacter && length < maxCharacters) {
                const std::int32_t stringWidth =
                    ops.call7AB4(state.name67F8.data(), 0);
                const std::int32_t glyphWidth = ops.call7B50(lastKey, 0);
                if (stringWidth + glyphWidth < 0x30 &&
                    length < static_cast<std::int32_t>(state.name67F8.size())) {
                    state.name67F8[static_cast<std::size_t>(length)] =
                        static_cast<char>(lastKey);
                    ++length;
                }
            }
        }

        ops.call9EB0(x, 0x88, 0x0A, 0x30, 0x5F);
        ops.call7D80(x, 0x88, 0x30, state.name67F8.data(), 0);
    }
}

} // namespace ravenloft::game
