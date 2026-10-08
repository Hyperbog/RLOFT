#include "rloft/combat_npc_injury_prelude.hpp"

#include <cassert>
#include <cstdint>
#include <initializer_list>

int main() {
    using rloft::CombatNpcInjuryPreludeInputs;
    using rloft::CombatNpcInjuryPreludeModel;

    CombatNpcInjuryPreludeInputs in{};
    in.npc_type_id=0x09U;
    in.previous_hp=15;
    in.previous_packed_state_14=0xA000U;
    in.npc_definition_reaction_byte_1c=1U;
    in.reaction_mode=0U;
    in.raw_damage=4;
    in.previous_runtime_flags=0xE2000002U;
    auto r=CombatNpcInjuryPreludeModel::inspect(in);
    assert(r.next_hp==11);
    assert(r.next_packed_state_14==0xA006U);
    assert(r.classification_state_applied);
    assert(r.next_runtime_flags==0xD2000002U);
    assert(r.next_action_countdown_10==2U);
    assert(!r.hp_subtraction_skipped && !r.type8_hp_floor_applied);
    in.reaction_mode=15U;
    in.raw_damage=16;
    r=CombatNpcInjuryPreludeModel::inspect(in);
    assert(r.next_hp==-1);
    assert(r.next_action_countdown_10==1U);

    // Nonzero/zero definition byte must not turn into a mode
    // scalar or unrelated high packed flag mutation.
    in.npc_definition_reaction_byte_1c=0U;
    r=CombatNpcInjuryPreludeModel::inspect(in);
    assert(r.next_packed_state_14==0xA000U);

    // Type 0x0F has NO HP deduction but still receives the
    // countdown and flags before the E306 follow-on call.
    in.npc_type_id=0x0FU;
    in.npc_definition_reaction_byte_1c=1U;
    r=CombatNpcInjuryPreludeModel::inspect(in);
    assert(r.next_hp==15 && r.hp_subtraction_skipped);
    assert(r.next_packed_state_14==0xA006U);

    // Type 8 has the unusual 10HP floor. With current HP
    // <=10, or the C000h original packed flag, subtract is
    // bypassed before the final creature-specific logic.
    in.npc_type_id=8U;
    in.previous_hp=10;
    in.previous_packed_state_14=0U;
    in.raw_damage=5;
    r=CombatNpcInjuryPreludeModel::inspect(in);
    assert(r.next_hp==10 && r.hp_subtraction_skipped);
    in.previous_hp=20;
    in.previous_packed_state_14=0xC000U;
    r=CombatNpcInjuryPreludeModel::inspect(in);
    assert(r.next_hp==20 && r.hp_subtraction_skipped);
    in.previous_packed_state_14=0x8000U;
    r=CombatNpcInjuryPreludeModel::inspect(in);
    assert(r.next_hp==15 && !r.hp_subtraction_skipped);
    in.raw_damage=30;
    r=CombatNpcInjuryPreludeModel::inspect(in);
    assert(r.next_hp==10 && r.type8_hp_floor_applied);
    assert(!r.hp_subtraction_skipped);
    in.raw_damage=10;
    r=CombatNpcInjuryPreludeModel::inspect(in);
    assert(r.next_hp==10 && !r.type8_hp_floor_applied);

    // 3ACDC returns true for type <0x22 or exactly 0x50.
    // Classifier updates NPC BYTE+0B for nonlethal injuries too.
    in.previous_runtime_flags=0xF2000040U;
    in.previous_hp=30;
    in.raw_damage=1;
    for(const auto type:{0x00U,0x08U,0x09U,0x0FU,0x21U,0x50U}) {
        in.npc_type_id=static_cast<std::uint16_t>(type);
        r=CombatNpcInjuryPreludeModel::inspect(in);
        assert(r.classification_state_applied);
        assert(r.next_runtime_flags==0xD2000040U);
    }
    for(const auto type:{0x22U,0x23U,0x4FU,0x51U,0xFFFFU}) {
        in.npc_type_id=static_cast<std::uint16_t>(type);
        r=CombatNpcInjuryPreludeModel::inspect(in);
        assert(!r.classification_state_applied);
        assert(r.next_runtime_flags==0xF2000040U);
    }

    // Generic NPC damage is a 16-bit original subtraction,
    // not a host saturating/nonnegative subtraction.
    in.npc_type_id=0x09U;
    in.previous_hp=1;
    in.raw_damage=65536+2;
    r=CombatNpcInjuryPreludeModel::inspect(in);
    assert(r.next_hp==-1);
    // Input inspection does not mutate caller-owned state.
    assert(in.previous_hp==1 && in.raw_damage==65538);
}
