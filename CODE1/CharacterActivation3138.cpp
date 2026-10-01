#include "CharacterActivation3138.h"

#include <array>
#include <cstdint>

namespace ravenloft::game {
namespace {
std::uint32_t add32(std::uint32_t a, std::uint32_t b) { return a + b; }
std::uint32_t mul32(std::uint32_t a, std::uint32_t b) { return a * b; }
}

void CharacterActivation3138::run(
    CharacterActivationState3138& state,
    const CharacterActivationOps3138& ops) {
    (void)state;
    const std::array<std::int32_t,9> slots{
        -1,-1,0x2F,0x30,-1,-1,-1,-1,-1
    };

    ops.writeDword(0x04D0u, ops.readDword(0x04D0u) + 1u);
    ops.call1464(6, slots, -1, -1, 0x2D);
    ops.writeDword(0x04D0u, ops.readDword(0x04D0u) - 1u);

    for (;;) {
        const std::uint32_t saved = ops.readDword(0x04D0u);
        ops.writeDword(0x04D0u, 0u);
        const std::int32_t selection = ops.call043C();
        ops.writeDword(0x04D0u, saved);

        if (selection == 3) {
            ops.call15B0(-1,-1,-1,-1);
            ops.call3020();
            const std::uint32_t character = ops.readDword(0x04CCu);
            ops.writeByte(add32(character, 0x04C4u), 1u);
            const std::uint32_t base = add32(mul32(character, 0x248u), 0x6808u);
            ops.writeByte(base, 0u);
            ops.writeByte(add32(base, 1u), 0u);
            return;
        }
        if (selection == 4) {
            ops.call15B0(-1,-1,-1,-1);
            const std::uint32_t character = ops.readDword(0x04CCu);
            ops.writeByte(add32(character, 0x04C4u), 0u);
            const std::uint32_t address = add32(mul32(character, 0x248u), 0x6809u);
            ops.writeByte(address, static_cast<std::uint8_t>(ops.readByte(address) | 0x80u));
            return;
        }
    }
}

} // namespace ravenloft::game
