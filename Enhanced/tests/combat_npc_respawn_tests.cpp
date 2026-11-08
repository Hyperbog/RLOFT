#ifdef NDEBUG
#undef NDEBUG // Assertions must remain active in Release CTest runs.
#endif
#include "rloft/combat_npc_respawn.hpp"
#include "rloft/original_random.hpp"

#include <cassert>
#include <cstdint>

int main() {
    using rloft::CombatNpcRespawnContext;
    using rloft::CombatNpcRespawnModel;
    using rloft::OriginalRandom;
    CombatNpcRespawnContext c{};
    c.map_width=32;
    c.map_height=32;
    c.map_cell_words.resize(32U*32U);
    c.party_x_cell=0;
    c.party_y_cell=0;
    c.active_npc_count=1;
    c.npc_slots.resize(1);
    c.respawn_slot=0;
    c.irq_tick_low16=42U;
    c.definition_hit_dice=2U;
    c.definition_initial_flags=0xAA551234U;
    c.npc_slots[0].npc_type_id=9U;
    c.npc_slots[0].current_hp=7;
    c.npc_slots[0].packed_runtime_state=0xFFFFU;
    c.npc_slots[0].runtime_flags=0x02000000U; // old slot is corpse

    // Seed 42 produces first candidate x=18, y=16 on 32x32.
    auto result=CombatNpcRespawnModel::inspect(c);
    assert(result && result->placed && result->attempts==1U);
    assert(result->respawned_npc.x_position_16_16==0x00128000U);
    assert(result->respawned_npc.y_position_16_16==0x00108000U);
    assert(result->respawned_npc.cached_cell_index==530U);
    assert(result->respawned_npc.runtime_flags==0xAA551234U);
    assert(result->respawned_npc.npc_type_id==9U);
    assert(result->respawned_npc.action_state_countdown==0U);
    assert(result->respawned_npc.packed_runtime_state==0xFE39U);
    OriginalRandom expected_rng(42U);
    (void)expected_rng.next15();
    (void)expected_rng.next15();
    const auto hp=expected_rng.dice(2U,6U);
    assert(result->respawned_npc.current_hp==static_cast<std::int16_t>(hp));
    assert(result->rng_state_after==expected_rng.state());
    // Read-only: inspection must not modify original NPC/world snapshot.
    assert(c.npc_slots[0].current_hp==7);
    assert(c.npc_slots[0].packed_runtime_state==0xFFFFU);

    // Low-byte occupancy blocks, including 0x40; high-byte 0x4000
    // alone does NOT block the original BYTE tests.
    c.map_cell_words[530U]=0x0040U;
    result=CombatNpcRespawnModel::inspect(c);
    assert(result && result->attempts>1U);
    c.map_cell_words[530U]=0x4000U;
    result=CombatNpcRespawnModel::inspect(c);
    assert(result && result->placed && result->attempts==1U);
    c.map_cell_words[530U]=0x003FU;
    result=CombatNpcRespawnModel::inspect(c);
    assert(result && result->attempts>1U);
    c.map_cell_words[530U]=0;

    // A living NPC in the candidate cell blocks it; a dead one
    // is skipped. Same slot may be reused only if it is dead.
    c.npc_slots[0].cached_cell_index=530U;
    c.npc_slots[0].runtime_flags=0U;
    result=CombatNpcRespawnModel::inspect(c);
    assert(result && result->attempts>1U);
    c.npc_slots[0].runtime_flags=0x02000000U;
    result=CombatNpcRespawnModel::inspect(c);
    assert(result && result->placed && result->attempts==1U);

    // In the original Level 22 30x15 map, no in-bounds player
    // can be 15 vertical cells away from any candidate y=1..13.
    c.map_width=30;
    c.map_height=15;
    c.map_cell_words.assign(30U*15U,0U);
    c.party_x_cell=4;
    c.party_y_cell=4;
    c.irq_tick_low16=17U;
    result=CombatNpcRespawnModel::inspect(c);
    assert(result && !result->placed && result->attempts==3U);
    OriginalRandom rejected(17U);
    for(int i=0;i<6;++i) (void)rejected.next15();
    assert(result->rng_state_after==rejected.state());
    assert(result->respawned_npc.current_hp==0);
    for(int party_y=0;party_y<15;++party_y) {
        c.party_y_cell=static_cast<std::int16_t>(party_y);
        result=CombatNpcRespawnModel::inspect(c);
        assert(result && !result->placed && result->attempts==3U);
        assert(result->rng_state_after==rejected.state());
    }

    // BIOS word is the seed only when original IRQ tick's low16=0.
    c.irq_tick_low16=0U;
    c.bios_timer_word_46c=17U;
    result=CombatNpcRespawnModel::inspect(c);
    assert(result && result->rng_state_after==rejected.state());
    c.bios_timer_word_46c=0U;
    result=CombatNpcRespawnModel::inspect(c);
    assert(result && result->rng_state_after!=rejected.state());

    // Invalid snapshots return unsupported, never 'no respawn'.
    c.map_cell_words.pop_back();
    assert(!CombatNpcRespawnModel::inspect(c));
    c.map_cell_words.push_back(0);
    c.respawn_slot=1U;
    assert(!CombatNpcRespawnModel::inspect(c));
}
