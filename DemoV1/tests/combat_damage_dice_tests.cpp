#include "rloft/combat_damage_dice.hpp"

#include <cassert>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <utility>
#include <initializer_list>
#include <vector>

namespace {
rloft::ObjectCatalog fixture() {
    std::vector<std::uint8_t> data(4U+0x100U*0x41U,0U);
    data[0]=0U;
    data[1]=1U; // 256 synthetic definitions
    const auto put=[&](unsigned id,unsigned off,unsigned v) {
        data.at(4U+id*0x41U+off)=static_cast<std::uint8_t>(v);
    };
    put(1U,0x38U,2U);put(1U,0x39U,6U);put(1U,0x3AU,3U);
    put(0x4AU,0x38U,1U);put(0x4AU,0x39U,8U);
    put(0x4AU,0x3AU,3U);
    put(0x2EU,0x38U,1U);put(0x2EU,0x39U,6U);
    return rloft::ObjectCatalog::parse(data);
}

void assert_dice(
    const rloft::CombatDamageDicePlan& plan,
    std::uint8_t count,std::uint8_t sides,
    std::int32_t per_die,std::int32_t final_bonus) {
    assert(plan.dice);
    assert(plan.dice->count==count);
    assert(plan.dice->sides==sides);
    assert(plan.dice->per_die_bonus==per_die);
    assert(plan.dice->final_bonus==final_bonus);
}

} // namespace

int main() {
    using rloft::CombatDamageDiceModel;
    using rloft::CombatDamageAfterRoll;
    const auto objects=fixture();
    const auto normal=CombatDamageAfterRoll::WeaponCreatureGate;

    auto plan=CombatDamageDiceModel::inspect(objects,-1);
    assert_dice(plan,1U,2U,0,0);
    assert(plan.after_roll==normal && !plan.hit_effect_code);

    // Exact original E5CC jump-table entries, NOT numeric ordering
    // of strength or successive code addresses. 7F requires 22E40.
    plan=CombatDamageDiceModel::inspect(objects,0x7E);
    assert_dice(plan,1U,4U,0,0);
    plan=CombatDamageDiceModel::inspect(objects,0x7F);
    assert(plan.requires_22e40_helper && !plan.dice);
    for (const auto [helper,expected] :
         {std::pair<int,int>{0,1},{1,1},{2,2},{3,2},
          {-1,1},{509,255}}) {
        plan=CombatDamageDiceModel::inspect(
            objects,0x7F,std::nullopt,helper);
        assert_dice(plan,static_cast<std::uint8_t>(expected),6U,0,0);
        assert(plan.requires_22e40_helper);
    }
    bool rejected=false;
    try {
        (void)CombatDamageDiceModel::inspect(
            objects,0x7F,std::nullopt,511);
    } catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    rejected=false;
    try {
        (void)CombatDamageDiceModel::inspect(
            objects,0x7F,std::nullopt,-4);
    } catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);

    plan=CombatDamageDiceModel::inspect(objects,0x80);
    assert_dice(plan,2U,6U,0,0);
    assert(plan.hit_effect_code && *plan.hit_effect_code==0x0CU);
    plan=CombatDamageDiceModel::inspect(objects,0x81);
    assert_dice(plan,1U,8U,0,0);
    assert(!plan.hit_effect_code);
    plan=CombatDamageDiceModel::inspect(objects,0x82);
    assert_dice(plan,2U,8U,1,0);
    assert(plan.hit_effect_code && *plan.hit_effect_code==9U);
    plan=CombatDamageDiceModel::inspect(objects,0x83);
    assert_dice(plan,3U,8U,3,0);
    assert(plan.hit_effect_code && *plan.hit_effect_code==9U);
    plan=CombatDamageDiceModel::inspect(objects,0x84);
    assert_dice(plan,1U,4U,0,0);
    assert(plan.after_roll==CombatDamageAfterRoll::HarmClampsNpcHp);
    assert(plan.hit_effect_code && *plan.hit_effect_code==0x24U);
    plan=CombatDamageDiceModel::inspect(objects,0x85);
    assert_dice(plan,1U,4U,4,2);
    plan=CombatDamageDiceModel::inspect(objects,0x86);
    assert_dice(plan,1U,10U,0,0);

    // Ordinary OBJECTS +38/+39/+3A: the final modifier comes
    // from actor/class computation at E6C9..E768, not the item.
    rejected=false;
    try {(void)CombatDamageDiceModel::inspect(objects,1);}
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    plan=CombatDamageDiceModel::inspect(objects,1,-4);
    assert_dice(plan,2U,6U,3,-4);
    assert(plan.after_roll==normal && !plan.hit_effect_code);
    assert(objects.at(1U).damage_dice_count==2U);
    assert(objects.at(1U).damage_dice_sides==6U);
    assert(objects.at(1U).damage_per_die_bonus==3U);

    // Rod of Smiting 4Ah diverts to its target-dependent
    // after-roll branch instead of the general C910 path.
    plan=CombatDamageDiceModel::inspect(objects,0x4A,0);
    assert_dice(plan,1U,8U,3,0);
    assert(plan.after_roll==CombatDamageAfterRoll::RodOfSmiting);
    plan=CombatDamageDiceModel::inspect(objects,0x2E,3);
    assert_dice(plan,1U,6U,0,3);

    rejected=false;
    try {(void)CombatDamageDiceModel::inspect(objects,-2);}
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    rejected=false;
    try {(void)CombatDamageDiceModel::inspect(objects,0x100,0);}
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);

    // Pure parameter selection never advances original RNG.
    const auto old=objects.at(1U);
    (void)CombatDamageDiceModel::inspect(objects,1,8);
    assert(objects.at(1U).damage_dice_count==old.damage_dice_count);
}
