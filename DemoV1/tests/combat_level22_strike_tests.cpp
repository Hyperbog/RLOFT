#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_level22_strike.hpp"
#include "rloft/level22_encounter.hpp"
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
std::vector<std::uint8_t> code2_fixture() {
    constexpr std::size_t base=0x75F24U;
    std::vector<std::uint8_t> code2(base+0x7E8CU+210U,0U);
    const std::int8_t strength[26]={
        2,-5,-3,-3,-2,-2,-1,-1,0,0,0,0,0,
        0,0,0,0,1,1,3,3,4,4,5,6,7};
    for(unsigned i=0;i<26;++i)
        code2[base+0x6832U+i]=
            static_cast<std::uint8_t>(strength[i]);
    for(unsigned row=0;row<21;++row)for(unsigned i=0;i<5;++i) {
        const auto off=base+0x7E8CU+row*10U+i*2U;
        const auto weight=static_cast<std::uint16_t>((i+1U)*200U);
        code2[off]=static_cast<std::uint8_t>(weight);
        code2[off+1U]=static_cast<std::uint8_t>(weight>>8U);
    }
    return code2;
}
rloft::ObjectCatalog weapons() {
    std::vector<std::uint8_t> bytes(4U+0x55U*0x41U,0U);
    bytes[0]=0x55U;
    const auto base=4U+1U*0x41U;
    bytes[base+0x2FU]=9U;
    bytes[base+0x38U]=1U; // original 1d4
    bytes[base+0x39U]=4U;
    return rloft::ObjectCatalog::parse(bytes);
}
rloft::PartyImage party() {
    rloft::PartyImage p{};
    p.bytes.fill(0xFFU);
    p.bytes[0x10U]=0U;
    p.bytes[0x11U]=0U;
    p.bytes[0x15U]=0U; // signed class lane
    p.bytes[0x33U]=10U;
    p.bytes[0x32U]=10U;
    p.bytes[0x3FU]=0U;
    p.bytes[0x3EU]=0U;
    p.bytes[0x10AU]=1U;
    p.bytes[0x40U+14U*2U]=1U;
    p.bytes[0x40U+14U*2U+1U]=0U;
    p.bytes[0x132U]=0x20U;
    p.bytes[0x133U]=0x02U;
    return p;
}
}
int main() {
    const auto code2=code2_fixture();
    const auto burden=rloft::InventoryEncumbranceModel::from_code2(code2);
    const auto strength=rloft::CombatStrengthAttackModel::from_code2(code2);
    const auto objects=weapons();
    rloft::InventoryRuntimeState inventory{};
    rloft::NpcDefinition definition{};
    definition.armor_class=3;
    rloft::LiveNpcRecord assassin{};
    assassin.npc_type_id=9U;
    assassin.current_hp=12;
    auto p=party();
    rloft::OriginalRandom rng(1U);

    // Real 1C328 dice: seed1 first d20=11, second d4 is rolled
    // ONLY after hit. E578 clears sourced actor effects.
    const auto hit=rloft::CombatLevel22StrikeModel::resolve(
        p,inventory,objects,burden,strength,definition,assassin,
        rng,0U,0U,1);
    assert(hit.hit.hit);
    assert(hit.raw_d20==11U);
    assert(hit.damage_after_c910>=1U && hit.damage_after_c910<=4U);
    assert(rng.state()!=1U);
    assert((p.bytes[0x132U]&0x20U)==0U);
    assert((p.bytes[0x133U]&0x02U)==0U);
    assert(hit.redraw_cf5a[0U]==1U);

    // Seed2 first original d20 is one. C80C misses even a
    // trivially low threshold, and does NOT consume damage dice.
    auto miss_party=party();
    rloft::OriginalRandom miss_rng(2U);
    const auto miss=rloft::CombatLevel22StrikeModel::resolve(
        miss_party,inventory,objects,burden,strength,
        definition,assassin,miss_rng,0U,0U,1);
    assert(!miss.hit.hit);
    assert(miss.raw_d20==1U);
    assert(miss.damage_after_c910==0U);
    rloft::OriginalRandom single(2U);
    (void)single.next15();
    assert(miss_rng.state()==single.state());

    // Original held status skips d20 RNG, then rolls only weapon dice.
    auto held_party=party();
    auto held=assassin;
    held.runtime_flags=0x08U;
    rloft::OriginalRandom held_rng(1U);
    const auto held_hit=rloft::CombatLevel22StrikeModel::resolve(
        held_party,inventory,objects,burden,strength,
        definition,held,held_rng,0U,0U,1);
    assert(held_hit.hit.auto_hit_held);
    assert(held_hit.raw_d20==0U);
    rloft::OriginalRandom one_die(1U);
    (void)one_die.next15();
    assert(held_rng.state()==one_die.state());

    // Original category9 creature bit15 zeros provisional damage
    // but C910 still clamps to one (a non-obvious source quirk).
    auto restricted_party=party();
    auto restricted_def=definition;
    restricted_def.behavior_flags=0x8000U;
    rloft::OriginalRandom restricted_rng(1U);
    const auto restricted=rloft::CombatLevel22StrikeModel::resolve(
        restricted_party,inventory,objects,burden,strength,
        restricted_def,assassin,restricted_rng,0U,0U,1);
    assert(restricted.hit.hit);
    assert(restricted.damage_after_c910==1U);

    // Source-only strike result feeds the REAL type9 E284 death
    // handler, with its four hardcoded CODE.2 2195C drop arguments.
    // This is a complete arithmetic -> world-state regression, not
    // a synthetic direct HP assignment or fabricated item bundle.
    std::vector<std::uint8_t> npc_bytes(4U+10U*0x45U,0U);
    npc_bytes[0]=10U;
    npc_bytes[4U+9U*0x45U+0x1CU]=1U;
    npc_bytes[4U+9U*0x45U+0x21U]=3U;
    npc_bytes[4U+9U*0x45U+0x23U]=200U;
    const auto npcs=rloft::NpcCatalog::parse(npc_bytes);
    rloft::LevelWorldState world{};
    auto doomed=assassin;
    doomed.current_hp=1;
    doomed.x_position_16_16=0x000D8000U;
    doomed.y_position_16_16=0x00078000U;
    world.npcs.push_back(doomed);
    rloft::LevelState level{};
    level.width=30;
    level.height=15;
    for(unsigned i=0;i<3U;++i) {
        level.header[0x292U+i*2U]=0xFFU;
        level.header[0x293U+i*2U]=0xFFU;
    }
    rloft::RendererTables renderer{};
    const auto killed=rloft::Level22EncounterModel::damage_assassin(
        world,level,0U,hit.damage_after_c910,0U,0U,
        npcs,renderer,{});
    assert(killed.died);
    assert(killed.experience_award==200U);
    assert(killed.spawned_loose_items==4U);
    assert(world.npcs[0].current_hp<=0);
    assert(world.loose_items.size()==4U);
    assert(world.loose_items[0].object_type_id==0x88U);
    assert(world.loose_items[1].object_type_id==0x87U);
    assert(world.loose_items[2].object_type_id==0xE9U);
    assert(world.loose_items[3].object_type_id==0x74U);

    // Wrong/unequipped hand and corpse cannot consume original RNG.
    auto invalid_party=party();
    rloft::OriginalRandom invalid_rng(1U);
    bool failed=false;
    try{(void)rloft::CombatLevel22StrikeModel::resolve(
        invalid_party,inventory,objects,burden,strength,
        definition,assassin,invalid_rng,0U,1U,1);}
    catch(const std::logic_error&){failed=true;}
    assert(failed && invalid_rng.state()==1U);
    failed=false;
    held.current_hp=0;
    try{(void)rloft::CombatLevel22StrikeModel::resolve(
        invalid_party,inventory,objects,burden,strength,
        definition,held,invalid_rng,0U,0U,1);}
    catch(const std::logic_error&){failed=true;}
    assert(failed && invalid_rng.state()==1U);

    // Source uses a separate Rod and pseudo-spell branch. Refuse
    // those rather than pretending ordinary weapon damage applies.
    invalid_party.bytes[0x40U+14U*2U]=0x4AU;
    failed=false;
    try{(void)rloft::CombatLevel22StrikeModel::resolve(
        invalid_party,inventory,objects,burden,strength,
        definition,assassin,invalid_rng,0U,0U,0x4A);}
    catch(const std::logic_error&){failed=true;}
    assert(failed && invalid_rng.state()==1U);
}
