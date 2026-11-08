#include "rloft/combat_hit_check.hpp"

#include <stdexcept>

namespace rloft {

CombatHitCheckResult CombatHitCheckModel::resolve(
    const CombatHitCheckInputs& inputs) {
    CombatHitCheckResult result{};
    // Original C818..C827 reads live family-A byte +8, bit 0x08
    // before touching weapon, burden, character effects or RNG.
    if ((inputs.live_npc_runtime_flags & 0x08U) != 0U) {
        result.hit = true;
        result.original_eax_return = 1U;
        result.auto_hit_held = true;
        return result;
    }

    if (inputs.burden_rank > 4U) {
        throw std::out_of_range("CODE.2 FE44 burden rank exceeds 4");
    }
    if (inputs.raw_d20 == 0U || inputs.raw_d20 > 20U) {
        throw std::out_of_range("CODE.2 C80C requires a 1d20 roll");
    }

    // Original C828..C87A: accumulator first receives the item
    // enchantment, then SUB strength helper, then burden rank penalty.
    if (inputs.burden_rank == 2U) result.burden_penalty = 1;
    if (inputs.burden_rank == 3U) result.burden_penalty = 2;
    if (inputs.burden_rank == 4U) result.burden_penalty = 4;
    const int attack_accumulator =
        static_cast<int>(inputs.weapon_enchantment) -
        (inputs.apply_strength_adjustment
             ? inputs.strength_attack_adjustment : 0) +
        result.burden_penalty;

    result.threshold =
        static_cast<int>(inputs.base_attack_target) -
        static_cast<int>(inputs.npc_armor_class) +
        attack_accumulator;
    // C898..C8A0: slowed live NPC (byte +8, bit 0x80)
    // reduces threshold by four.
    if ((inputs.live_npc_runtime_flags & 0x80U) != 0U) {
        result.threshold -= 4;
    }

    // C8A1..C8D5: actor effect +131 low-byte flags 0x01/0x08
    // and equipped gauntlets (item[20] == 0x54).
    result.roll_bonus =
        static_cast<int>(inputs.bless) +
        static_cast<int>(inputs.aid) +
        (inputs.stone_giant_gauntlets ? 3 : 0);
    result.modified_d20 =
        static_cast<int>(inputs.raw_d20) +
        result.roll_bonus;

    // C8E5..C908 checks >=20 BEFORE ==1 and threshold compare.
    // Do not replace these with tabletop natural-one/natural-twenty.
    result.hit =
        result.modified_d20 >= 20 ||
        (result.modified_d20 != 1 &&
         result.modified_d20 >= result.threshold);
    result.original_eax_return=result.hit ? 1U : 0U;
    return result;
}

} // namespace rloft
