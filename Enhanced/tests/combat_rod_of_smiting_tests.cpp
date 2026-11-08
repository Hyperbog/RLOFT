#include "rloft/combat_rod_of_smiting.hpp"
#include "rloft/combat_hit_check.hpp"

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
    // Connect both independent original primitives in a model
    // regression: even raw d20 20 creates original C80C EAX=1,
    // so the Rod may not enter its unreachable charged branch.
    rloft::CombatHitCheckInputs original_hit{};
    original_hit.base_attack_target=255U;
    original_hit.raw_d20=20U;
    const auto original_hit_result=
        rloft::CombatHitCheckModel::resolve(original_hit);
    assert(original_hit_result.hit);
    assert(original_hit_result.modified_d20==20);
    assert(original_hit_result.original_eax_return==1U);
    const auto original_rod=CombatRodOfSmitingModel::inspect(
        0x17U,original_hit_result.original_eax_return);
    assert(original_rod.additional_dice &&
           !original_rod.charge_consumed);

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
