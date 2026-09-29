#include "games/ravenloft/game/NpcAttackDispatcherD550.h"
#include "games/ravenloft/game/NpcHitTestC9A0.h"
#include "games/ravenloft/game/NpcTargetExclusionD4E8.h"
#include "games/ravenloft/game/OriginalDice1C328.h"
#include <cstdint>
#include <cstdlib>
#include <stdexcept>

namespace ravenloft::game {
namespace {

std::uint16_t u16(const data::CharacterRecord& c,std::size_t o){
    return static_cast<std::uint16_t>(c.raw[o]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(c.raw[o+1])<<8u);
}
std::int16_t i16(const data::CharacterRecord& c,std::size_t o){return static_cast<std::int16_t>(u16(c,o));}
std::uint32_t u32(const data::CharacterRecord& c,std::size_t o){
    return static_cast<std::uint32_t>(c.raw[o]) |
           (static_cast<std::uint32_t>(c.raw[o+1])<<8u) |
           (static_cast<std::uint32_t>(c.raw[o+2])<<16u) |
           (static_cast<std::uint32_t>(c.raw[o+3])<<24u);
}
std::int32_t i32(const data::CharacterRecord& c,std::size_t o){return static_cast<std::int32_t>(u32(c,o));}
void put32(data::CharacterRecord& c,std::size_t o,std::uint32_t v){
    c.raw[o]=static_cast<std::uint8_t>(v);c.raw[o+1]=static_cast<std::uint8_t>(v>>8u);
    c.raw[o+2]=static_cast<std::uint8_t>(v>>16u);c.raw[o+3]=static_cast<std::uint8_t>(v>>24u);
}
int direction(std::uint16_t f){return (f>>3u)&7;}
std::int32_t absWrap(std::int32_t v){
    // x86 CDQ/XOR/SUB absolute-value sequence; INT_MIN remains INT_MIN by wrap.
    const auto sign=v<0?0xFFFFFFFFu:0u;
    return static_cast<std::int32_t>((static_cast<std::uint32_t>(v)^sign)-sign);
}
bool downForD550(const data::CharacterRecord& c){return u16(c,0x10)>=0x4000u;}
bool allPartyDown(const std::array<data::CharacterRecord,4>& party){
    for(const auto& c:party) if((u16(c,0x10)&0xD040u)==0) return false;
    return true;
}
void markDirty(std::size_t index,const NpcAttackContextD550& c){
    if(c.damageContext.cf5aCharacterFlags && index<c.damageContext.cf5aCharacterFlags->size())
        (*c.damageContext.cf5aCharacterFlags)[index]=1;
}
int cee4(std::size_t index,const NpcAttackContextD550& c){return c.predicateCEE4?c.predicateCEE4(index):0;}
void queue(const NpcAttackContextD550& c,std::uint32_t id){if(c.queueSound28110)c.queueSound28110(id,0xFF,0,1);}

} // namespace

NpcAttackResultD550 NpcAttackDispatcherD550::execute(data::RuntimePlacement22& actor,
                                                      const data::NpcDefinition& npc,
                                                      int npcIndex,
                                                      OriginalRng30CA2& rng,
                                                      const NpcAttackContextD550& c){
    if(!c.party) throw std::invalid_argument("CODE.2 0xD550 requires party records");
    auto& party=*c.party;
    NpcAttackResultD550 result{};

    // 0xD55F..0xD5C3: type 15 bypasses ordinary targeting. Every living,
    // non-CEE4 character takes HP + the two CD44 absorption pools, which makes
    // the original damage routine exhaust the pools and then the HP exactly.
    if(actor.type==0x0F){
        result.path=NpcAttackResultD550::Path::Type15;
        for(std::size_t i=0;i<party.size();++i){
            if(downForD550(party[i])||cee4(i,c))continue;
            const int damage=static_cast<int>(i16(party[i],0x1C))+
                             i32(party[i],0x1EA)+i32(party[i],0x113);
            CharacterDamageCD44::apply(i,damage,party,c.damageContext);
        }
        queue(c,c.soundBase258C0+npc.attackSound);
        return result;
    }

    // 0xD5C8..0xD620: type 8 with high field14 phase 0x8000 launches the
    // special 0x8A80 path and returns before all ordinary attack processing.
    if(actor.type==8 && (actor.field14&0xC000u)==0x8000u){
        result.path=NpcAttackResultD550::Path::Type8Special;
        int angle=(8-direction(actor.field14))*45;
        if(angle>=360)angle-=360;
        if(c.type8Special8A80)c.type8Special8A80(0,angle,-1,actor.yFixed,actor.xFixed);
        return result;
    }

    // 0xD625..0xD652: fixed-point rectangular reach gate.
    const auto dy=static_cast<std::int32_t>(static_cast<std::uint32_t>(c.partyYFixed)-static_cast<std::uint32_t>(actor.yFixed));
    const auto dx=static_cast<std::int32_t>(static_cast<std::uint32_t>(c.partyXFixed)-static_cast<std::uint32_t>(actor.xFixed));
    if(absWrap(dy)>0x15000 || absWrap(dx)>0x15000) return result;

    // 0xD658..0xD6BA. DS:687E exact eight-byte table from CODE.2 object 2.
    static constexpr std::uint8_t kTargetOrder687E[8]={5,0,5,0,5,1,5,1};
    const int index=((c.facingDegreesC658/45)+direction(actor.field14))&7;
    int target=kTargetOrder687E[index];

    if(!c.timer30700) throw std::invalid_argument("CODE.2 0xD550 ordinary path requires 0x30700 timer boundary");
    rng.seed(c.timer30700()); // 0x30700 -> 0x30CC4
    if(target>=4 && target<=7){
        // CS:D540 jump table: {D6AF,D6A2,D6AF,D6A2} for values 4..7.
        const int r=static_cast<int>((static_cast<std::uint32_t>(rng.next15())*2u)>>15u);
        target=((target&1)==0)?r+1:r;
    }

    // 0xD6BB..0xD735. When targetMode258A8 != FF, the original cycles all
    // four candidates; otherwise it only retries the target's paired slot.
    if(c.targetMode258A8!=0xFFu){
        const int start=target;
        while(NpcTargetExclusionD4E8::evaluate(party.at(static_cast<std::size_t>(target)),npc)!=0){
            target=(target+3)%4; // (target+3) idiv 4 remainder
            if(target==start){
                queue(c,c.soundBase258BC+9u);
                result.path=NpcAttackResultD550::Path::OrdinaryMiss;
                return result;
            }
        }
    }else{
        if(downForD550(party[0]) && downForD550(party[1]) && target<2) target+=2;
        if(NpcTargetExclusionD4E8::evaluate(party.at(static_cast<std::size_t>(target)),npc)!=0){
            target^=1;
            if(NpcTargetExclusionD4E8::evaluate(party.at(static_cast<std::size_t>(target)),npc)!=0){
                queue(c,c.soundBase258BC+9u);
                result.path=NpcAttackResultD550::Path::OrdinaryMiss;
                return result;
            }
        }
    }
    result.target=target;

    const bool thresholdProtection=c.query14A60 && c.query14A60(0x584)==target;
    NpcHitTestContextC9A0 hitCtx;
    hitCtx.characterArmorClassEFA0=c.characterArmorClassEFA0;
    hitCtx.effect871C=c.effect871C;
    hitCtx.rollD20=[&](){return OriginalDice1C328::roll(1,20,0,0,rng);};
    if(NpcHitTestC9A0::evaluate(static_cast<std::size_t>(target),party[target],npc,actor,hitCtx)==0){
        if(c.missFeedback)c.missFeedback(actor.type);
        queue(c,c.soundBase258BC+9u);
        result.path=NpcAttackResultD550::Path::OrdinaryMiss;
        return result;
    }

    result.hit=true;
    result.path=NpcAttackResultD550::Path::OrdinaryHit;
    result.damage=OriginalDice1C328::roll(npc.damageDice,npc.damageDie,npc.damageBonus,0,rng);
    int protectedEffect=0;
    auto& victim=party[static_cast<std::size_t>(target)];

    // 0xD78B..0xD7A7 occurs before the 0x584 threshold branch: this protection
    // flag can therefore still be consumed even when thresholdProtection skips
    // the later secondary effects and/or ordinary HP damage.
    if((npc.traits3A&0x20u)!=0 && (victim.raw[0x133]&0x40u)!=0) protectedEffect=1;

    // thresholdProtection (DS query 0x584) skips the remaining secondary attack effects.
    if(!thresholdProtection){
        // NPCS +0x3A bit 0x02: level-drain branch.
        if((npc.traits3A&0x02u)!=0){
            if((victim.raw[0x133]&0x40u)!=0) protectedEffect=1;
            else if(!cee4(static_cast<std::size_t>(target),c) && c.levelDrainD1E0)
                c.levelDrainD1E0(static_cast<std::size_t>(target));
        }

        // NPCS +0x3A bit 0x04: temporary Strength drain. Type-8 attacks only
        // enter this subpath while actor field14 high phase is exactly 0x4000.
        if((npc.traits3A&0x04u)!=0 &&
           !(actor.type==8 && (actor.field14&0xC000u)!=0x4000u)){
            if((victim.raw[0x133]&0x40u)!=0) protectedEffect=1;
            else if(!cee4(static_cast<std::size_t>(target),c)){
                if(c.adjustAbility1202C)c.adjustAbility1202C(static_cast<std::size_t>(target),-1);
                const int durationDice=OriginalDice1C328::roll(2,4,0,0,rng);
                const std::uint32_t now=c.currentTick19CC4?c.currentTick19CC4():0u;
                // 0xD875..0xD89F: exact arithmetic is roll * 10,800.
                put32(victim,0x1EE,now+static_cast<std::uint32_t>(durationDice*10800));
            }
        }

        // NPCS +0x39 bit 0x20 status branch.
        if((npc.traits39&0x20u)!=0){
            bool blocked=(actor.type==0x11 && victim.raw[0x14]==5u) ||
                         i16(victim,0x5C)==0x86 || i16(victim,0x5E)==0x86 ||
                         (c.queryEffectF2D4 && c.queryEffectF2D4(static_cast<std::size_t>(target),0x45)!=0) ||
                         cee4(static_cast<std::size_t>(target),c)!=0;
            if(!blocked){
                victim.raw[0x11]|=0x10u;
                const std::size_t pair=static_cast<std::size_t>(target)/2u;
                if(c.damageContext.d681PairActive && c.damageContext.d8a8PairCharacter &&
                   pair<2 && (*c.damageContext.d681PairActive)[pair]!=0 &&
                   (*c.damageContext.d8a8PairCharacter)[pair]==target){
                    if(c.damageContext.cf5aCharacterFlags){
                        (*c.damageContext.cf5aCharacterFlags)[pair*2u]=1;
                        (*c.damageContext.cf5aCharacterFlags)[pair*2u+1u]=1;
                    }
                    (*c.damageContext.d681PairActive)[pair]=0;
                }
            }
        }

        if((npc.traits39&0x01u)!=0 && !cee4(static_cast<std::size_t>(target),c)) victim.raw[0x11]|=0x08u;
        if((npc.traits3A&0x10u)!=0 && static_cast<std::int8_t>(victim.raw[0x15])!=1 &&
           !cee4(static_cast<std::size_t>(target),c)) victim.raw[0x11]|=0x04u;
    }

    // 0xD993..0xD9B8: query 0x584 prevents an ordinary hit from taking the
    // protected character to 10 HP or lower; it does not clamp the damage.
    if(!thresholdProtection || static_cast<int>(i16(victim,0x1C))-result.damage>10)
        CharacterDamageCD44::apply(static_cast<std::size_t>(target),result.damage,party,c.damageContext);

    // 0xD9BB..0xD9F3: protection consumption/effect branch. The leading zero
    // is the fifth E284 argument intentionally left on the stack around 1C328.
    if(protectedEffect){
        const int r=OriginalDice1C328::roll(2,6,0,0,rng);
        if(c.protectedEffectE284)c.protectedEffectE284(actor,npc,npcIndex,r,0);
        put32(victim,0x16E,0u);
        victim.raw[0x133]&=0xBFu;
        result.protectedEffectConsumed=true;
    }

    markDirty(static_cast<std::size_t>(target),c);
    queue(c,c.soundBase258C0+npc.attackSound);

    // D550 performs its own all-party-down test after the attack sound, in
    // addition to the identical test inside the converted CD44 routine.
    if(allPartyDown(party) && c.damageContext.allPartyDownCBC8)
        c.damageContext.allPartyDownCBC8();
    return result;
}

} // namespace ravenloft::game
