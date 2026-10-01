#include "CharacterVital32F4.h"

#include <cstdint>

namespace ravenloft::game {
namespace {

constexpr std::uint32_t add32(std::uint32_t a, std::uint32_t b) noexcept {
    return a + b;
}

constexpr std::uint32_t mul32(std::uint32_t a, std::uint32_t b) noexcept {
    return a * b;
}

} // namespace

void CharacterVital32F4::initialize(const CharacterVitalOps32F4& ops) {
    // 0x32F8: EBX = DS:04CC * 0x248, low 32 bits.
    const std::uint32_t characterBase =
        mul32(ops.readDword(0x04CCu), 0x248u);

    // 0x3302..0x331A: zero-extend field 680C, form index*3 exactly as
    // LEA EAX,[EDX*4] / SUB EAX,EDX, then load the raw table word.
    const std::uint32_t field =
        static_cast<std::uint32_t>(ops.readByte(add32(characterBase, 0x680Cu)));
    const std::uint32_t tableOffset = field * 3u;
    const std::uint32_t tableAddress = add32(0x082Eu, tableOffset);
    ops.writeWord(add32(characterBase, 0x6810u), ops.readWord(tableAddress));

    // 0x3321..0x333A: pushes 0,0, BYTE [table+2], BYTE [table+1].
    const std::uint32_t eax = ops.call0010(
        static_cast<std::uint32_t>(ops.readByte(add32(tableAddress, 1u))),
        static_cast<std::uint32_t>(ops.readByte(add32(tableAddress, 2u))),
        0u,
        0u);

    // 0x333D: ADD WORD [EBX+6810],AX. Only the low 16 EAX bits participate;
    // the destination addition wraps at 16 bits.
    const std::uint32_t destination = add32(characterBase, 0x6810u);
    const std::uint16_t sum = static_cast<std::uint16_t>(
        static_cast<std::uint32_t>(ops.readWord(destination)) +
        static_cast<std::uint32_t>(static_cast<std::uint16_t>(eax)));
    ops.writeWord(destination, sum);
}

} // namespace ravenloft::game
