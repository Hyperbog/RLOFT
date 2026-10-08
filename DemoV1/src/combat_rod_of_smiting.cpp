#include "rloft/combat_rod_of_smiting.hpp"

#include <cstdint>
#include <stdexcept>

namespace rloft {

CombatRodOfSmitingPlan CombatRodOfSmitingModel::inspect(
    std::uint16_t npc_type_id,
    std::uint32_t actual_c80c_return_value) {
    // CODE.2 C80C:C81E/C8EA/C8F9/C900 returns literal 1/0.
    // It NEVER exposes the raw or modified d20 to E5F0.
    if (actual_c80c_return_value>1U) {
        throw std::out_of_range(
            "CODE.2 E8D2 receives C80C boolean, not a d20 roll");
    }
    if (actual_c80c_return_value==0U) {
        throw std::invalid_argument(
            "CODE.2 E8BB Rod branch is only entered on C80C hit");
    }
    CombatRodOfSmitingPlan plan{};
    if (npc_type_id==0x17U || npc_type_id==0x1FU) {
        // E8C4..E910: C80C's stored return value is 1 and can
        // never equal 0x14. The reachable branch calls
        // 1C328(2,8,6,0), after the initial ordinary damage
        // dice have already been drawn at E86E..E899.
        plan.additional_dice=CombatDamageDiceArgs{2U,8U,6,0};
    }
    // The +31 signed index / DGROUP 67EC charged-rod branch
    // E8D9/E919 requires result==20 and is dead on this path.
    // Neither charged damage nor charge decrement can occur.
    return plan;
}

} // namespace rloft
