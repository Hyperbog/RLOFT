#include "rloft/combat_rod_of_smiting.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>

int main() {
    using rloft::CombatRodOfSmitingModel;
    // For both special original NPC types, the actual reachable
    // non-critical path rolls additional 2d8 +6 PER die.
    for(const auto npc_type:{std::uint16_t{0x17U},
                             std::uint16_t{0x1FU}}) {
        const auto p=CombatRodOfSmitingModel::inspect(npc_type,1U);
        assert(p.additional_dice);
        assert(p.additional_dice->count==2U);
        assert(p.additional_dice->sides==8U);
        assert(p.additional_dice->per_die_bonus==6);
        assert(p.additional_dice->final_bonus==0);
        assert(!p.charge_consumed && p.skip_c910);
    }
    // All other creature types keep the initial rolled damage:
    // still no C910, no extra 1C328 RNG and no charge change.
    for(const auto npc_type:{std::uint16_t{0U},
                             std::uint16_t{8U},
                             std::uint16_t{0x18U},
                             std::uint16_t{0xFFFFU}}) {
        const auto p=CombatRodOfSmitingModel::inspect(npc_type,1U);
        assert(!p.additional_dice);
        assert(!p.charge_consumed && p.skip_c910);
    }
    // Passing the underlying raw d20==20 as the value of C80C
    // would wrongly activate unreachable charge branches.
    bool rejected=false;
    try {(void)CombatRodOfSmitingModel::inspect(0x17U,20U);}
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    rejected=false;
    try {(void)CombatRodOfSmitingModel::inspect(0x17U,0U);}
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
}
