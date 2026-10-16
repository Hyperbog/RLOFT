#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_player_hit_inputs.hpp"
#include "rloft/combat_hit_check.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
constexpr std::size_t kCode2Base=0x75F24U;
void u16(std::vector<std::uint8_t>& b,std::size_t at,std::uint16_t x) {
    b.at(at)=static_cast<std::uint8_t>(x);
    b.at(at+1U)=static_cast<std::uint8_t>(x>>8U);
}
std::vector<std::uint8_t> original_table_fixture() {
    std::vector<std::uint8_t> code2(kCode2Base+0x7E8CU+210U,0U);
    const std::int8_t attacks[26]={
        2,-5,-3,-3,-2,-2,-1,-1,0,0,0,0,0,
        0,0,0,0,1,1,3,3,4,4,5,6,7};
    for(std::size_t i=0;i<26U;++i)
        code2[kCode2Base+0x6832U+i]=
            static_cast<std::uint8_t>(attacks[i]);
    for(std::size_t row=0;row<21U;++row)
        for(std::size_t i=0;i<5U;++i)
            u16(code2,kCode2Base+0x7E8CU+row*10U+i*2U,
                static_cast<std::uint16_t>(100U*(i+1U)));
    return code2;
}
std::vector<std::uint8_t> object_fixture() {
    std::vector<std::uint8_t> bytes(4U+0x55U*0x41U,0U);
    bytes[0]=0x55U;
    // Original OBJECTS record byte 37 is weapon enchantment.
    bytes[4U+0x20U*0x41U+0x37U]=1U;
    return bytes;
}
} // namespace

int main() {
    const auto original=original_table_fixture();
    auto str=rloft::CombatStrengthAttackModel::from_code2(original);
    assert(str.score(0U,0U)==2);
    assert(str.score(1U,0U)==-5);
    assert(str.score(16U,0U)==0);
    assert(str.score(18U,0U)==1);
    assert(str.score(19U,0U)==3);
    assert(str.score(25U,0U)==7);
    assert(str.score(18U,1U)==1);
    assert(str.score(18U,50U)==1);
    assert(str.score(18U,51U)==2);
    assert(str.score(18U,99U)==2);
    assert(str.score(18U,100U)==3);
    bool invalid=false;
    try{(void)str.score(26U,0U);}catch(const std::out_of_range&){invalid=true;}
    assert(invalid);
    invalid=false;
    try{(void)str.inspect(rloft::PartyImage{},4U);}
    catch(const std::out_of_range&){invalid=true;}
    assert(invalid);
    invalid=false;
    try{(void)rloft::CombatStrengthAttackModel::from_code2({});}
    catch(const std::runtime_error&){invalid=true;}
    assert(invalid);

    auto burden=rloft::InventoryEncumbranceModel::from_code2(original);
    auto objects=rloft::ObjectCatalog::parse(object_fixture());
    rloft::PartyImage party{};
    party.bytes.fill(0xFFU);
    party.bytes[0x10U]=0U; party.bytes[0x11U]=0U;
    party.bytes[0x33U]=18U;party.bytes[0x3FU]=51U;
    party.bytes[0x32U]=18U;party.bytes[0x3EU]=0U;
    party.bytes[0x10AU]=17U;
    party.bytes[0xF3U]=0x09U; // Bless and Aid
    // Genuine equipment slot 20: Stone Giant Gauntlets 54h.
    party.bytes[0x40U+20U*2U]=0x54U;
    party.bytes[0x41U+20U*2U]=0U;
    rloft::InventoryRuntimeState inventory{};
    rloft::NpcDefinition assassin{};
    assassin.armor_class=3;
    rloft::LiveNpcRecord live{};
    live.npc_type_id=9U;
    const auto in=rloft::CombatPlayerHitInputsModel::build(
        party,inventory,objects,burden,str,assassin,live,0U,0x20,15U);
    assert(in.base_attack_target==17U);
    assert(in.npc_armor_class==3);
    assert(in.weapon_enchantment==1U);
    assert(in.strength_attack_adjustment==2);
    assert(in.burden_rank==0U);
    assert(in.bless && in.aid && in.stone_giant_gauntlets);
    assert(in.raw_d20==15U);
    const auto hit=rloft::CombatHitCheckModel::resolve(in);
    assert(hit.threshold==13);
    assert(hit.roll_bonus==5);
    assert(hit.modified_d20==20 && hit.hit);

    live.runtime_flags=0x08U; // held NPC is hit before 1d20
    const auto held=rloft::CombatPlayerHitInputsModel::build(
        party,inventory,objects,burden,str,assassin,live,0U,99999,0U);
    assert(held.live_npc_runtime_flags==8U);
    assert(rloft::CombatHitCheckModel::resolve(held).auto_hit_held);
    live.runtime_flags=0U;
    // No unknown original Strength table memory is ever guessed.
    party.bytes[0x33U]=26U;
    invalid=false;
    try{
        (void)rloft::CombatPlayerHitInputsModel::build(
            party,inventory,objects,burden,str,assassin,live,0U,0x20,15U);
    }catch(const std::out_of_range&){invalid=true;}
    assert(invalid);
}
