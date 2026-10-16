#include "rloft/combat_level22_strike.hpp"

#include "rloft/combat_attack_status_service.hpp"
#include "rloft/combat_damage_dice.hpp"
#include "rloft/combat_damage_stat_bonus.hpp"
#include "rloft/combat_player_hit_inputs.hpp"
#include "rloft/combat_weapon_damage_gate.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace rloft {
namespace {
[[nodiscard]] std::uint16_t weapon_at(
    const PartyImage& party,std::size_t actor,std::size_t hand) {
    const auto at=actor*PartyImage::kActorSize+0x40U+2U*(14U+hand);
    return static_cast<std::uint16_t>(party.bytes.at(at)) |
           (static_cast<std::uint16_t>(party.bytes.at(at+1U))<<8U);
}
[[nodiscard]] std::int16_t checked_die_argument(std::int32_t value) {
    if(value<std::numeric_limits<std::int16_t>::min() ||
       value>std::numeric_limits<std::int16_t>::max())
        throw std::out_of_range("original 1C328 native signed die argument");
    return static_cast<std::int16_t>(value);
}
} // namespace

CombatLevel22StrikeResult CombatLevel22StrikeModel::resolve(
    PartyImage& party,const InventoryRuntimeState& inventory,
    const ObjectCatalog& objects,const InventoryEncumbranceModel& burden,
    const CombatStrengthAttackModel& strength,
    const NpcDefinition& definition,const LiveNpcRecord& npc,
    OriginalRandom& random,std::size_t actor,std::size_t hand,
    std::int32_t item_id) {
    if(actor>=PartyImage::kActorCount || hand>=2U)
        throw std::out_of_range("E5F0 actor/hand outside party");
    if(item_id<0 || item_id>0xFFFF ||
       weapon_at(party,actor,hand)!=static_cast<std::uint16_t>(item_id))
        throw std::logic_error("E5F0 dispatched weapon no longer equipped");
    if(npc.npc_type_id!=9U || npc.current_hp<=0)
        throw std::logic_error("E5F0 scoped target is not a living Assassin");
    // Neither Rod 4Ah nor pseudo spell-hands 7Eh..86h can use the
    // ordinary C910 path unconditionally; their other E5F0 actions
    // remain separately modeled and are NOT approximated here.
    if(item_id==0x4A || (item_id>=0x7E && item_id<=0x86))
        throw std::logic_error("E5F0 special weapon effects not yet serviced");
    const auto& weapon=objects.at(static_cast<std::size_t>(item_id));
    if(weapon.item_category<9U || weapon.item_category>11U)
        throw std::logic_error("E5F0 item is not an ordinary attack weapon");
    // Preflight original stat/dice parameters BEFORE any RNG draw.
    const auto bonus=CombatDamageStatBonusModel::inspect(party,actor);
    const auto recipe=CombatDamageDiceModel::inspect_for_actor(
        objects,party,actor,item_id,bonus);
    if(!recipe.dice ||
       recipe.after_roll!=CombatDamageAfterRoll::WeaponCreatureGate ||
       recipe.hit_effect_code)
        throw std::logic_error("E5F0 attack requires unmodeled special effect");
    const auto dice=*recipe.dice;
    if(dice.sides==0U || dice.count==0U)
        throw std::logic_error("E5F0 ordinary weapon dice have no valid sides");
    const auto per_die=checked_die_argument(dice.per_die_bonus);
    const auto final_bonus=checked_die_argument(dice.final_bonus);
    // E578 changes the actors' effects before the C80C hit branch.
    CombatLevel22StrikeResult result{};
    CombatAttackStatusService::apply(party,result.redraw_cf5a,actor);
    // Held target bit08 at C818 returns before requesting RNG.
    const bool held=(npc.runtime_flags&0x08U)!=0U;
    if(!held)
        result.raw_d20=static_cast<std::uint8_t>(random.dice(1U,20U));
    const auto inputs=CombatPlayerHitInputsModel::build(
        party,inventory,objects,burden,strength,definition,npc,
        actor,item_id,result.raw_d20);
    result.hit=CombatHitCheckModel::resolve(inputs);
    if(!result.hit.hit)return result; // E5F0 skips damage RNG on miss.
    const auto initial=random.dice(
        dice.count,dice.sides,per_die,final_bonus);
    const auto gated=CombatWeaponDamageGateModel::inspect(
        objects,item_id,definition.behavior_flags,
        static_cast<std::int32_t>(initial));
    if(gated.damage_after_gate<1 ||
       gated.damage_after_gate>
           static_cast<std::int32_t>(std::numeric_limits<std::uint16_t>::max()))
        throw std::out_of_range("E5F0 original damage outside native word");
    result.damage_after_c910=
        static_cast<std::uint16_t>(gated.damage_after_gate);
    return result;
}

} // namespace rloft
