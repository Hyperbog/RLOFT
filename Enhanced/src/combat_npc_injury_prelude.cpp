#include "rloft/combat_npc_injury_prelude.hpp"

#include <cstdint>

namespace rloft {

CombatNpcInjuryPreludeResult
CombatNpcInjuryPreludeModel::inspect(
    const CombatNpcInjuryPreludeInputs& in) noexcept {
    CombatNpcInjuryPreludeResult out{};
    out.next_hp=in.previous_hp;
    out.next_packed_state_14=in.previous_packed_state_14;
    // Original 3ACDC NPC type classifier: type <0x22 or ==0x50.
    // E317: BYTE [npc+0B] &=0xCF, then |=0x10. As a DWORD
    // at npc+08 this clears bits28/29 then sets bit28.
    out.classification_state_applied=
        in.npc_type_id<0x22U || in.npc_type_id==0x50U;
    out.next_runtime_flags=in.previous_runtime_flags;
    if (out.classification_state_applied) {
        out.next_runtime_flags=
            (out.next_runtime_flags&~0x30000000U)|0x10000000U;
    }
    // E284:E290..E2C1: nonzero reaction-mode chooses 1, zero
    // chooses 2. NPC definition BYTE+1C controls packed bits 1/2.
    out.next_action_countdown_10=
        in.reaction_mode==0U ? 2U : 1U;
    if (in.npc_definition_reaction_byte_1c!=0U) {
        out.next_packed_state_14=
            static_cast<std::uint16_t>(
                out.next_packed_state_14|0x0006U);
    }

    // E284:E2C1..E306: NPC type 0x0F bypasses HP deduction.
    // Type 0x08 obeys an additional special floor of ten HP,
    // except while its packed state high two bits are both set.
    if (in.npc_type_id==0x0FU) {
        out.hp_subtraction_skipped=true;
        return out;
    }
    if (in.npc_type_id==0x08U) {
        if ((in.previous_packed_state_14&0xC000U)==0xC000U ||
            in.previous_hp<=10) {
            out.hp_subtraction_skipped=true;
            return out;
        }
    }

    // E2E7/E2FE: SUB WORD [npc+12], AX. Truncate the supplied
    // damage to 16 bits; preserve original two's-complement
    // wrap even for negative or >65535 input damage values.
    const auto wrapped=static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(in.previous_hp) -
        static_cast<std::uint16_t>(in.raw_damage));
    out.next_hp=static_cast<std::int16_t>(wrapped);

    if (in.npc_type_id==0x08U && out.next_hp<=0) {
        out.next_hp=10;
        out.type8_hp_floor_applied=true;
    }
    return out;
}

} // namespace rloft
