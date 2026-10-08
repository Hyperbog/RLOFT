#include "rloft/combat_harm_prelude.hpp"

#include <cstdint>
#include <stdexcept>

namespace rloft {

CombatHarmPreludeResult CombatHarmPreludeModel::inspect(
    std::int16_t npc_hp_before,
    std::int32_t actual_1d4_roll) {
    // E815 rolls original 1C328(1,4,0,0) before E825:
    // it is not legal to invent a result without consuming RNG.
    if (actual_1d4_roll<1 || actual_1d4_roll>4) {
        throw std::out_of_range(
            "CODE.2 HARM requires actual 1d4 roll, not a placeholder");
    }
    // E825 MOVSX npc HP WORD+12; E831 CMP EDX,EAX (signed).
    // Only when the current SIGNED HP exceeds the rolled
    // signed DWORD do E835 bytes MOV [npc+12],AX lower HP.
    const bool clamp=npc_hp_before>actual_1d4_roll;
    CombatHarmPreludeResult result{};
    result.hp_after_direct_clamp=clamp
        ? static_cast<std::int16_t>(actual_1d4_roll)
        : npc_hp_before;
    result.direct_clamp_applied=clamp;
    // E829 hit effect24h, E839 XOR ESI,ESI. The later
    // E947 normal C910 path may still force ONE damage;
    // pre-gate zero must not be mistaken for final zero.
    result.outgoing_damage_before_c910=0;
    result.visual_effect_code=0x24U;
    return result;
}

} // namespace rloft
