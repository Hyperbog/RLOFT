#include "rloft/combat_original_sprite_machine.hpp"

#include <cstddef>
#include <cstdint>

namespace rloft {

bool CombatOriginalSpriteCodePatchModel::patch_2a584(
    CombatOriginalSpriteMachineState& machine,
    std::uint32_t table_pointer) noexcept {
    auto& bytes=machine.writable_code_object1;
    constexpr std::size_t kFirst=0x2A60BU;
    constexpr std::size_t kSecond=0x2A672U;
    // 2A609 and 2A670 each have MOV AL, [EBX + imm32]
    // opcode 8A 83. The original 2A590/2A595 stores update
    // the four operand bytes, not the opcode or adjacent code.
    if(bytes.size()<kSecond+4U ||
       bytes[kFirst-2U]!=0x8AU ||
       bytes[kFirst-1U]!=0x83U ||
       bytes[kSecond-2U]!=0x8AU ||
       bytes[kSecond-1U]!=0x83U)
        return false;

    // Validate both sites first, then commit both stores. Never
    // change one operand if the captured second site is invalid.
    for(std::size_t i=0U;i<4U;++i) {
        const auto octet=static_cast<std::uint8_t>(
            (table_pointer>>(i*8U))&0xFFU);
        bytes[kFirst+i]=octet;
        bytes[kSecond+i]=octet;
    }
    return true;
}

} // namespace rloft
