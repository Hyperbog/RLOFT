#include "rloft/combat_hit_check.hpp"

#include <cassert>
#include <stdexcept>

int main() {
    using rloft::CombatHitCheckInputs;
    using rloft::CombatHitCheckModel;

    CombatHitCheckInputs input{};
    // Attack target 14 vs AC 2 requires an unmodified 12.
    input.base_attack_target = 14U;
    input.npc_armor_class = 2;
    input.raw_d20 = 11U;
    auto result = CombatHitCheckModel::resolve(input);
    assert(!result.hit && result.original_eax_return==0U);
    assert(result.threshold == 12);
    assert(result.modified_d20 == 11);
    input.raw_d20 = 12U;
    assert(CombatHitCheckModel::resolve(input).hit);

    // CODE.2 checks the adjusted result BEFORE threshold; a raw
    // 19 plus Bless becomes automatic hit at 20, regardless of AC.
    input.base_attack_target = 255U;
    input.raw_d20 = 19U;
    input.bless = true;
    result = CombatHitCheckModel::resolve(input);
    assert(result.roll_bonus == 1);
    assert(result.modified_d20 == 20);
    assert(result.hit);
    // Returning the 20 here instead of 1 would wrongly enable
    // shipped but unreachable Rod of Smiting charge branches.
    assert(result.original_eax_return==1U);

    // Conversely a natural 1 is NOT automatically missed when
    // the original effect bonus turns it into 2.
    input.base_attack_target = 2U;
    input.npc_armor_class = 0;
    input.raw_d20 = 1U;
    result = CombatHitCheckModel::resolve(input);
    assert(result.modified_d20 == 2);
    assert(result.hit);
    input.bless = false;
    result = CombatHitCheckModel::resolve(input);
    assert(result.modified_d20 == 1);
    assert(!result.hit);

    // C818 held-NPC bit returns immediately: not even an input
    // 1d20 or valid burden/attack definition is required.
    input.live_npc_runtime_flags = 0x08U;
    input.raw_d20 = 0U;
    input.burden_rank = 255U;
    result = CombatHitCheckModel::resolve(input);
    assert(result.hit && result.auto_hit_held);
    assert(result.original_eax_return==1U);
    assert(result.modified_d20 == 0);
    input.live_npc_runtime_flags = 0U;

    input.base_attack_target = 14U;
    input.npc_armor_class = 2;
    input.burden_rank = 4U;
    input.raw_d20 = 12U;
    input.apply_strength_adjustment = true;
    input.strength_attack_adjustment = 3;
    input.weapon_enchantment = 1U;
    input.aid = true;
    input.stone_giant_gauntlets = true;
    input.live_npc_runtime_flags = 0x80U;
    result = CombatHitCheckModel::resolve(input);
    // Threshold 14 - 2 + 1 - 3 + 4 - 4 = 10.
    assert(result.threshold == 10);
    assert(result.burden_penalty == 4);
    assert(result.roll_bonus == 4);
    assert(result.modified_d20 == 16);
    assert(result.hit);

    input.apply_strength_adjustment = false;
    result = CombatHitCheckModel::resolve(input);
    assert(result.threshold == 13);

    input.burden_rank = 5U;
    bool rejected=false;
    try { (void)CombatHitCheckModel::resolve(input); }
    catch (const std::out_of_range&) { rejected=true; }
    assert(rejected);
    input.burden_rank=0U;
    input.raw_d20=21U;
    rejected=false;
    try { (void)CombatHitCheckModel::resolve(input); }
    catch (const std::out_of_range&) { rejected=true; }
    assert(rejected);
}
