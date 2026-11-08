#include "rloft/combat_damage_dice.hpp"

#include <cstdint>
#include <optional>
#include <stdexcept>
#include "rloft/combat_class_level.hpp"

namespace rloft {
namespace {

CombatDamageDiceArgs dice(
    std::uint8_t count, std::uint8_t sides,
    std::int32_t per_die_bonus=0, std::int32_t final_bonus=0) {
    return {count,sides,per_die_bonus,final_bonus};
}

} // namespace

CombatDamageDicePlan CombatDamageDiceModel::inspect(
    const ObjectCatalog& objects,
    std::int32_t dispatched_item_id,
    std::optional<std::int32_t> original_stat_bonus,
    std::optional<std::int32_t> helper_22e40_value) {
    const auto item=dispatched_item_id;
    if (item < -1) {
        throw std::out_of_range("CODE.2 E5F0 negative item ID");
    }
    CombatDamageDicePlan plan{};

    // E5F0:E76A..E775: empty hand calls 1C328(1,2,0,0).
    // It does NOT consume the ordinary weapon stat modifier.
    if (item==-1) {
        plan.dice=dice(1U,2U);
        return plan;
    }

    // E5F0:E78F..E7A2 dispatches pseudo IDs through a NINE
    // ENTRY code-pointer table stored at logical E5CC. Actual
    // original table IDs map to the PC-specific clauses below.
    switch(item) {
        case 0x7E: // E7EA: CHILL TOUCH 1d4
            plan.dice=dice(1U,4U);
            return plan;
        case 0x7F: { // E845: VAMPIRIC TOUCH, dynamic Nd6
            plan.requires_22e40_helper=true;
            if (!helper_22e40_value) return plan;
            // Original E854..E860: sign adjustment then SAR 1,
            // then INC; equivalent to floor((v-sign(v))/2)+1.
            const auto v=static_cast<std::int64_t>(*helper_22e40_value);
            const auto adjusted=v+(v<0?1:0);
            const auto half=adjusted>=0
                ? adjusted/2
                : -((-adjusted+1)/2);
            const auto count=half+1;
            // 1C328 takes signed 32-bit dice count. Our bounded
            // native dice backend currently supports u8 count.
            if (count<0 || count>255) {
                throw std::out_of_range(
                    "CODE.2 E845 dynamic dice count outside native backend");
            }
            plan.dice=dice(static_cast<std::uint8_t>(count),6U);
            return plan;
        }
        case 0x80: // E802: CLAWS OF UMBER HULK, 2d6 + effect C
            plan.dice=dice(2U,6U);
            plan.hit_effect_code=0x0CU;
            return plan;
        case 0x81: // E7A3: CAUSE LIGHT WOUNDS, 1d8
            plan.dice=dice(1U,8U);
            return plan;
        case 0x82: // E7AE: CAUSE SERIOUS WOUNDS, 2d8 +1/die
            plan.dice=dice(2U,8U,1);
            plan.hit_effect_code=0x09U;
            return plan;
        case 0x83: // E7CC: CAUSE CRITICAL WOUNDS, 3d8 +3/die
            plan.dice=dice(3U,8U,3);
            plan.hit_effect_code=0x09U;
            return plan;
        case 0x84: // E815: HARM, HP clamp then outgoing 0
            plan.dice=dice(1U,4U);
            plan.after_roll=CombatDamageAfterRoll::HarmClampsNpcHp;
            plan.hit_effect_code=0x24U;
            return plan;
        case 0x85: // E7F5: FLAME BLADE, 1d4 +4/die +2 final
            plan.dice=dice(1U,4U,4,2);
            return plan;
        case 0x86: // E83D: LICH TOUCH, 1d10
            plan.dice=dice(1U,10U);
            return plan;
        default:
            break;
    }

    // E5F0:E86E..E88F: ordinary OBJECTS record +38/+39/+3A
    // becomes 1C328(count,sides,per_die_bonus,final_bonus).
    // The final bonus was computed earlier from actor +33/+3F,
    // optional class probes and the signed table at 684Bh. It
    // is NOT stored in OBJECTS.DAT and must not be guessed.
    if (!original_stat_bonus) {
        throw std::invalid_argument(
            "CODE.2 E86E needs original stat-derived damage bonus");
    }
    const auto& def=objects.at(static_cast<std::size_t>(item));
    plan.dice=dice(
        def.damage_dice_count,
        def.damage_dice_sides,
        def.damage_per_die_bonus,
        *original_stat_bonus);
    if (item==0x4A) {
        plan.after_roll=CombatDamageAfterRoll::RodOfSmiting;
    }
    return plan;
}

CombatDamageDicePlan CombatDamageDiceModel::inspect_for_actor(
    const ObjectCatalog& objects,
    const PartyImage& party,
    std::size_t actor,
    std::int32_t dispatched_item_id,
    std::optional<std::int32_t> original_stat_bonus) {
    if (actor>=PartyImage::kActorCount) {
        throw std::out_of_range("CODE.2 E845 actor index");
    }
    if (dispatched_item_id==0x7F) {
        // E845 pushes literal class 4 then actor, calls 22E40,
        // and converts its returned SIGNED class-level word
        // into the dynamic number of d6 dice. No invented
        // spell-caster level is used.
        const auto level=CombatClassLevelModel::for_class(
            party,actor,4);
        return inspect(objects,dispatched_item_id,
                       original_stat_bonus,level);
    }
    return inspect(objects,dispatched_item_id,
                   original_stat_bonus);
}

} // namespace rloft
