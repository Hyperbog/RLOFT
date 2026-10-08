#include "rloft/combat_harm_prelude.hpp"

#include "rloft/combat_npc_injury_prelude.hpp"
#include "rloft/combat_weapon_damage_gate.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
    using rloft::CombatHarmPreludeModel;
    using rloft::CombatNpcInjuryPreludeInputs;
    using rloft::CombatNpcInjuryPreludeModel;
    using rloft::CombatWeaponDamageGateModel;

    auto p=CombatHarmPreludeModel::inspect(20,3);
    assert(p.direct_clamp_applied && p.hp_after_direct_clamp==3);
    assert(p.outgoing_damage_before_c910==0);
    assert(p.visual_effect_code==0x24U);
    // If HP<=roll, HARM does NOT heal or overwrite the HP.
    p=CombatHarmPreludeModel::inspect(2,4);
    assert(!p.direct_clamp_applied && p.hp_after_direct_clamp==2);
    p=CombatHarmPreludeModel::inspect(-2,1);
    assert(!p.direct_clamp_applied && p.hp_after_direct_clamp==-2);
    p=CombatHarmPreludeModel::inspect(1,4);
    assert(!p.direct_clamp_applied && p.hp_after_direct_clamp==1);

    // Cross-model temporal ordering:
    //   E815 actual RNG -> E835 direct HP clamp -> E839 damage0
    //   E947 C910 minimum1 -> E963 E284 NPC HP subtraction.
    // The returned next HP is not simply the HARM 1d4 value.
    std::vector<std::uint8_t> raw(4U+0x85U*0x41U,0U);
    raw[0]=0x85U;
    const auto objects=rloft::ObjectCatalog::parse(raw);
    const auto gate=CombatWeaponDamageGateModel::inspect(
        objects,0x84U,0U,p.outgoing_damage_before_c910);
    assert(gate.damage_after_gate==1);
    assert(gate.minimum_clamped);
    auto injury=CombatNpcInjuryPreludeModel::inspect(
        CombatNpcInjuryPreludeInputs{
            9U,p.hp_after_direct_clamp,0U,1U,0U,
            gate.damage_after_gate
        });
    assert(injury.next_hp==0);
    assert(injury.next_action_countdown_10==2U);
    assert(injury.next_packed_state_14==6U);
    p=CombatHarmPreludeModel::inspect(20,3);
    const auto gate2=CombatWeaponDamageGateModel::inspect(
        objects,0x84U,0U,p.outgoing_damage_before_c910);
    injury=CombatNpcInjuryPreludeModel::inspect(
        CombatNpcInjuryPreludeInputs{
            9U,p.hp_after_direct_clamp,0U,0U,1U,
            gate2.damage_after_gate
        });
    assert(injury.next_hp==2);
    assert(injury.next_action_countdown_10==1U);
    assert(injury.next_packed_state_14==0U);

    for(const auto invalid:{-1,0,5,20}) {
        bool rejected=false;
        try {
            (void)CombatHarmPreludeModel::inspect(
                14,invalid);
        } catch(const std::out_of_range&) {rejected=true;}
        assert(rejected);
    }
}
