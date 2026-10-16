#include "rloft/combat_player_hit_inputs.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {
std::int16_t item_word(const std::uint8_t* bytes,std::size_t slot) {
    const auto at=0x40U+2U*slot;
    const auto raw=static_cast<std::uint16_t>(bytes[at]) |
        (static_cast<std::uint16_t>(bytes[at+1U])<<8U);
    return static_cast<std::int16_t>(raw);
}
} // namespace

CombatHitCheckInputs CombatPlayerHitInputsModel::build(
    const PartyImage& party,const InventoryRuntimeState& inventory,
    const ObjectCatalog& objects,const InventoryEncumbranceModel& burden,
    const CombatStrengthAttackModel& strength,
    const NpcDefinition& definition,const LiveNpcRecord& npc,
    std::size_t actor,std::int32_t dispatched_item_id,
    std::uint8_t raw_d20) {
    CombatHitCheckInputs in{};
    if(actor>=PartyImage::kActorCount || dispatched_item_id<-1)
        throw std::out_of_range("C80C player actor or weapon");
    in.live_npc_runtime_flags=static_cast<std::uint8_t>(npc.runtime_flags);
    // Original C818 returns immediate success for a held NPC before
    // burden, weapon, Strength, effects or even the 1d20 RNG draw.
    if((in.live_npc_runtime_flags&0x08U)!=0U)return in;
    if(!party.active(actor))
        throw std::logic_error("inactive actor cannot make ordinary hit check");
    const auto base=actor*PartyImage::kActorSize;
    const auto* rec=party.bytes.data()+base;
    in.base_attack_target=rec[0x10AU];
    in.npc_armor_class=definition.armor_class;
    in.strength_attack_adjustment=strength.inspect(party,actor);
    in.burden_rank=burden.actor(party,inventory,objects,actor).burden_rank;
    if(dispatched_item_id>=0)
        in.weapon_enchantment=objects.at(
            static_cast<std::size_t>(dispatched_item_id)).enchantment;
    const auto primary_effects=rec[0xF3U];
    in.bless=(primary_effects&0x01U)!=0U;
    in.aid=(primary_effects&0x08U)!=0U;
    in.stone_giant_gauntlets=item_word(rec,20U)==0x54;
    in.raw_d20=raw_d20;
    return in;
}

} // namespace rloft
