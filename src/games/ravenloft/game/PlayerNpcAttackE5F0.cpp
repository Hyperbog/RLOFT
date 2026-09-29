#include "games/ravenloft/game/PlayerNpcAttackE5F0.h"
#include "games/ravenloft/game/OriginalDice1C328.h"
#include "games/ravenloft/game/PlayerAttackCooldown24B48.h"
#include "games/ravenloft/game/PlayerAttackStateE578.h"
#include "games/ravenloft/game/PlayerDamageModifierC910.h"
#include "games/ravenloft/game/HandItemPredicates.h"
#include <array>
#include <cstdint>
#include <stdexcept>

namespace ravenloft::game {
namespace {
constexpr std::array<int,26> kStrengthDamage684B{{
    7,-4,-2,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,1,1,2,7,8,9,10,11,12,14
}};
std::uint16_t u16(const std::array<std::uint8_t,65>& r,std::size_t o){
    return static_cast<std::uint16_t>(r[o]) |
           (static_cast<std::uint16_t>(r[o+1])<<8u);
}
std::uint32_t u32(const std::array<std::uint8_t,0x248>& r,std::size_t o){
    return static_cast<std::uint32_t>(r[o]) |
           (static_cast<std::uint32_t>(r[o+1])<<8u) |
           (static_cast<std::uint32_t>(r[o+2])<<16u) |
           (static_cast<std::uint32_t>(r[o+3])<<24u);
}
void put32(std::array<std::uint8_t,0x248>& r,std::size_t o,std::uint32_t v){
    for(int i=0;i<4;++i)r[o+static_cast<std::size_t>(i)]=static_cast<std::uint8_t>(v>>(i*8));
}
}

int PlayerNpcAttackE5F0::strengthDamageModifier(const data::CharacterRecord& character){
    const int strength=character.raw[0x33];
    const int exceptional=character.raw[0x3F];
    // 0xE6DA..0xE72F: only fighter-capable classes (paladin, thief, fighter,
    // ranger in the original query order) use exceptional-Strength damage.
    const bool exceptionalClass=HandItemPredicates::characterHasClass23EAC(character,1)||
                                HandItemPredicates::characterHasClass23EAC(character,5)||
                                HandItemPredicates::characterHasClass23EAC(character,0)||
                                HandItemPredicates::characterHasClass23EAC(character,2);
    if(strength!=18 || !exceptionalClass){
        if(strength<0 || static_cast<std::size_t>(strength)>=kStrengthDamage684B.size())
            throw std::runtime_error("E5F0 Strength outside original DS:684B table");
        return kStrengthDamage684B[static_cast<std::size_t>(strength)];
    }
    if(exceptional<=75)return 3;
    if(exceptional<=90)return 4;
    if(exceptional<=99)return 5;
    return 6;
}

int PlayerNpcAttackE5F0::recoveryDuration(int itemId,
                                           const data::CharacterRecord& character,
                                           const data::ObjectDatabase& objects){
    int recovery=0x12;
    if(itemId!=-1 && (itemId<0x7E || itemId>0x86)){
        if(itemId<0 || static_cast<std::size_t>(itemId)>=objects.count())
            throw std::runtime_error("E5F0 recovery item outside OBJECTS.DAT");
        const auto& object=objects.at(static_cast<std::size_t>(itemId)).raw;
        const std::uint32_t shifted=static_cast<std::uint32_t>(u16(object,0x35))<<10u;
        const std::uint16_t low=static_cast<std::uint16_t>(shifted);
        recovery=static_cast<int>(static_cast<std::uint16_t>(low>>12u))*9;
    }
    if((u32(character.raw,0x132)&0x01000010u)!=0)
        recovery/=2;
    return recovery;
}

PlayerNpcAttackResultE5F0 PlayerNpcAttackE5F0::execute(std::array<data::CharacterRecord,4>& party,
                                                        std::size_t characterIndex,
                                                        int handIndex,
                                                        int itemId,
                                                        std::span<data::RuntimePlacement22> actors,
                                                        const data::NpcDatabase& npcs,
                                                        int targetNpcIndex,
                                                        const data::ObjectDatabase& objects,
                                                        OriginalRng30CA2& rng,
                                                        const PlayerNpcAttackContextE5F0& context){
    if(characterIndex>=party.size())throw std::out_of_range("E5F0 character index");
    if(handIndex<0 || handIndex>1)throw std::out_of_range("E5F0 hand index");
    auto& character=party[characterIndex];
    PlayerNpcAttackResultE5F0 result{};
    result.itemId=itemId;

    // 0xE5FB..0xE614: a literal -1 from 0x26018 is replaced by DB94's
    // previous-render-buffer target selection before 0xE578 is called.
    if(targetNpcIndex==-1 && context.autoTargetDB94)
        targetNpcIndex=context.autoTargetDB94(characterIndex);
    result.targetNpcIndex=targetNpcIndex;

    // 0xE618 calls this even when no target survives selection.
    PlayerAttackStateE578::clear(party,characterIndex,context.characterDirty);

    // 0xE621..0xE669 target invalidation branch. It does not affect either
    // Quick Start attacker (indices 0/1), but is retained for state parity.
    if(context.global258A8==0xFF && targetNpcIndex!=-1 && (characterIndex&0x02u)!=0){
        if(itemId==-1){
            targetNpcIndex=-1;
        }else{
            if(itemId<0 || static_cast<std::size_t>(itemId)>=objects.count())
                throw std::runtime_error("E5F0 item outside OBJECTS.DAT");
            const auto word35=u16(objects.at(static_cast<std::size_t>(itemId)).raw,0x35);
            if((word35&0x03u)!=2u)targetNpcIndex=-1;
        }
        result.targetNpcIndex=targetNpcIndex;
    }

    result.recovery=recoveryDuration(itemId,character,objects);
    if(targetNpcIndex==-1){
        PlayerAttackCooldownContext24B48 cc{context.currentTick19CC4};
        PlayerAttackCooldown24B48::apply(character,handIndex,result.recovery,cc);
        result.path=PlayerNpcAttackResultE5F0::Path::NoTarget;
        return result;
    }

    if(targetNpcIndex<0 || static_cast<std::size_t>(targetNpcIndex)>=actors.size())
        throw std::runtime_error("E5F0 selected NPC index outside runtime actor array");
    auto& actor=actors[static_cast<std::size_t>(targetNpcIndex)];
    const auto& definition=npcs.at(actor.type);

    if((itemId>=0x7E && itemId<=0x86) || itemId==0x4A){
        result.path=PlayerNpcAttackResultE5F0::Path::UnsupportedSpecialWeapon;
        return result;
    }

    const auto hit=PlayerHitTestC80C::evaluate(characterIndex,character,actor,definition,
                                                itemId,true,objects,rng,context.hitTest);
    result.hitRoll=hit.roll;
    result.requiredRoll=hit.required;
    if(!hit.hit){
        PlayerAttackCooldownContext24B48 cc{context.currentTick19CC4};
        PlayerAttackCooldown24B48::apply(character,handIndex,result.recovery,cc);
        result.path=PlayerNpcAttackResultE5F0::Path::Miss;
        return result;
    }

    const int strengthBonus=strengthDamageModifier(character);
    int damage=0;
    if(itemId==-1){
        damage=OriginalDice1C328::roll(1,2,0,strengthBonus,rng);
    }else{
        if(itemId<0 || static_cast<std::size_t>(itemId)>=objects.count())
            throw std::runtime_error("E5F0 damage item outside OBJECTS.DAT");
        const auto& object=objects.at(static_cast<std::size_t>(itemId)).raw;
        damage=OriginalDice1C328::roll(object[0x38],object[0x39],object[0x3A],strengthBonus,rng);
        damage=PlayerDamageModifierC910::adjust(damage,definition,itemId,objects);
    }
    result.damage=damage;

    const auto dr=NpcDamageE284::apply(actor,definition,targetNpcIndex,damage,0,context.damageE284);
    result.died=dr.died;
    result.path=PlayerNpcAttackResultE5F0::Path::Hit;

    if(!context.currentTick19CC4)throw std::runtime_error("E5F0 requires 0x19CC4 clock boundary");
    const std::uint32_t actionTick=context.currentTick19CC4()+static_cast<std::uint32_t>(result.recovery/2);
    put32(character.raw,0x19E + static_cast<std::size_t>(handIndex)*4u,actionTick);
    put32(character.raw,0x1AA + static_cast<std::size_t>(handIndex)*4u,static_cast<std::uint32_t>(damage));

    PlayerAttackCooldownContext24B48 cc{context.currentTick19CC4};
    PlayerAttackCooldown24B48::apply(character,handIndex,result.recovery,cc);
    return result;
}

} // namespace ravenloft::game
