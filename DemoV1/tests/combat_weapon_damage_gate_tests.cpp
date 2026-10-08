#include "rloft/combat_weapon_damage_gate.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

rloft::ObjectCatalog fixture() {
    std::vector<std::uint8_t> b(4U+7U*0x41U,0U);
    b[0]=7U;
    const auto set=[&](unsigned id,unsigned offset,
                       unsigned value) {
        b.at(4U+id*0x41U+offset)=static_cast<std::uint8_t>(value);
    };
    set(1U,0x2FU,9U);
    set(2U,0x2FU,10U);
    set(3U,0x2FU,11U);
    set(4U,0x2FU,8U);
    set(5U,0x2FU,9U);set(5U,0x37U,0xFFU);
    set(6U,0x2FU,10U);set(6U,0x37U,1U);
    return rloft::ObjectCatalog::parse(b);
}

} // namespace

int main() {
    using rloft::CombatWeaponDamageGateModel;
    const auto objects=fixture();
    const auto run=[&](std::int32_t id,std::uint32_t flags,
                       std::int32_t damage) {
        return CombatWeaponDamageGateModel::inspect(
            objects,id,flags,damage);
    };

    // Empty hand is exact category9, not the 26018 category10
    // unarmed dispatch category. C910 is a distinct classifier.
    auto x=run(-1,0U,9);
    assert(x.damage_after_gate==9 && !x.forced_minimum);
    x=run(-1,0x8000U,9);
    assert(x.damage_after_gate==1 && x.forced_minimum);

    // Category 9 checks creature bit15, category 10 checks bit14.
    x=run(1,0x8000U,8);
    assert(x.damage_after_gate==1);
    x=run(1,0x4000U,8);
    assert(x.damage_after_gate==8);
    x=run(2,0x4000U,8);
    assert(x.damage_after_gate==1);
    x=run(2,0x8000U,8);
    assert(x.damage_after_gate==8);

    // Category11's bit17 veto is unconditional in practice
    // because MOVZX enchantment cannot be signed < -2.
    for(const auto id:{3,5,6}) {
        x=run(id,0x20000U,42);
        assert(x.damage_after_gate==1 && x.forced_minimum);
        x=run(id,0x40000U,42);
        assert(x.damage_after_gate==42 && !x.forced_minimum);
    }
    // Nonzero enchantment overrides original category9/10.
    x=run(5,0x8000U,42);
    assert(x.damage_after_gate==42);
    x=run(6,0x4000U,42);
    assert(x.damage_after_gate==42);

    // Other categories ignore these creature-immunity bits.
    x=run(4,0xFFFFFFFFU,17);
    assert(x.damage_after_gate==17);
    // Universal final minimum, including post-HARM zero.
    for(const auto damage:{-20,0,1}) {
        x=run(4,0U,damage);
        assert(x.damage_after_gate==1);
    }
    x=run(4,0U,2);
    assert(x.damage_after_gate==2 && !x.forced_minimum);

    bool rejected=false;
    try {(void)run(-2,0U,4);}
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    rejected=false;
    try {(void)run(7,0U,4);}
    catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
}
