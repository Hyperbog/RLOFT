#include "CharacterRolls334C.h"

#include <cstdint>

namespace ravenloft::game {
namespace {
constexpr std::uint32_t add32(std::uint32_t a, std::uint32_t b) noexcept {
    return a + b;
}
constexpr std::uint32_t mul32(std::uint32_t a, std::uint32_t b) noexcept {
    return a * b;
}
}

void CharacterRolls334C::initialize(const CharacterRollOps334C& ops) {
    // 0x3350..0x336C: memset(current record + 0x32, 0, 14).
    std::uint32_t characterBase = mul32(ops.readDword(0x04CCu), 0x248u);
    ops.call84A0(add32(characterBase, 0x682Au), 0u, 0x0Eu);

    // 0x336F..0x339B: six calls to 0x0010(3,6,0,0). Only AL is retained,
    // and ADD CL,3 wraps at eight bits before every other byte is written.
    for (std::uint32_t ebx = 0; ebx < 6u; ++ebx) {
        const std::uint32_t eax = ops.call0010(3u, 6u, 0u, 0u);
        const std::uint8_t cl = static_cast<std::uint8_t>(
            static_cast<std::uint8_t>(eax) + static_cast<std::uint8_t>(3u));
        characterBase = mul32(ops.readDword(0x04CCu), 0x248u);
        ops.writeByte(add32(characterBase, add32(0x682Au, ebx * 2u)), cl);
    }

    // 0x339D..0x33FE. EAX at 0x339D is the current character base from the
    // final loop iteration. The tests are byte comparisons exactly as encoded.
    if (ops.readByte(add32(characterBase, 0x682Au)) < 0x12u)
        return;
    if (ops.readByte(add32(characterBase, 0x680Cu)) == 2u)
        return;

    const std::uint8_t class0 = ops.readByte(add32(characterBase, 0x680Du));
    const bool qualifies =
        class0 == 1u || class0 == 2u || class0 == 0u ||
        ops.readByte(add32(characterBase, 0x680Eu)) == 0u ||
        ops.readByte(add32(characterBase, 0x680Fu)) == 0u;
    if (!qualifies)
        return;

    const std::uint32_t eax = ops.call0010(1u, 0x64u, 0u, 0u);
    characterBase = mul32(ops.readDword(0x04CCu), 0x248u);
    ops.writeByte(add32(characterBase, 0x6836u),
                  static_cast<std::uint8_t>(eax));
}

} // namespace ravenloft::game
