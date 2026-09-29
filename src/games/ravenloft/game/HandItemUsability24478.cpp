#include "games/ravenloft/game/HandItemUsability24478.h"
#include "games/ravenloft/game/HandItemClassifier240A8.h"
#include "games/ravenloft/game/HandItemPredicates.h"
#include <cstdint>
#include <stdexcept>

namespace ravenloft::game {
namespace {
std::uint16_t u16(const std::array<std::uint8_t,0x248>& r,std::size_t o){
    return static_cast<std::uint16_t>(r[o]) |
        static_cast<std::uint16_t>(static_cast<std::uint16_t>(r[o+1])<<8u);
}
std::uint32_t u32(const std::array<std::uint8_t,0x248>& r,std::size_t o){
    return static_cast<std::uint32_t>(r[o]) |
        (static_cast<std::uint32_t>(r[o+1])<<8u) |
        (static_cast<std::uint32_t>(r[o+2])<<16u) |
        (static_cast<std::uint32_t>(r[o+3])<<24u);
}
std::int16_t i16(const std::array<std::uint8_t,0x248>& r,std::size_t o){return static_cast<std::int16_t>(u16(r,o));}
const std::array<std::uint8_t,65>& objectRaw(const data::ObjectDatabase& objects,int id){
    if(id<0 || static_cast<std::size_t>(id)>=objects.count())throw std::runtime_error("24478 object id outside OBJECTS.DAT");
    return objects.at(static_cast<std::size_t>(id)).raw;
}
int handKind(const data::ObjectDatabase& objects,int id){return objectRaw(objects,id)[0x35]&0x03;}
std::uint8_t weight(const data::ObjectDatabase& objects,int id){return objectRaw(objects,id)[0x30];}
bool martialGroup(const data::CharacterRecord& c){
    return HandItemPredicates::characterHasClass23EAC(c,0) ||
           HandItemPredicates::characterHasClass23EAC(c,2) ||
           HandItemPredicates::characterHasClass23EAC(c,5) ||
           HandItemPredicates::characterHasClass23EAC(c,1);
}
bool partyGate(std::size_t characterIndex,const HandItemUsabilityContext24478& ctx){
    if(ctx.global258A8!=0xFF || characterIndex<2)return true;
    return u16((*ctx.party)[0].raw,0x10)>=0x4000u &&
           u16((*ctx.party)[1].raw,0x10)>=0x4000u;
}
void keepLighter(int leftId,int rightId,const data::ObjectDatabase& objects,int& leftOk,int& rightOk){
    // Assembly uses unsigned BYTE comparisons. Ties keep left and disable right.
    if(weight(objects,rightId)>=weight(objects,leftId))rightOk=0;
    else leftOk=0;
}
}

int HandItemUsability24478::evaluate(std::size_t characterIndex,
                                     int handIndex,
                                     const data::ObjectDatabase& objects,
                                     const HandItemUsabilityContext24478& ctx){
    if(!ctx.party || characterIndex>=ctx.party->size())throw std::runtime_error("24478 requires four-party context");
    const auto& c=(*ctx.party)[characterIndex];
    const int leftId=i16(c.raw,0x5C);
    const int rightId=i16(c.raw,0x5E);
    int leftClass=0,rightClass=0;
    HandItemClassifierContext240A8 classifierContext{ctx.party,ctx.global258A8};
    int leftOk=HandItemClassifier240A8::evaluate(characterIndex,leftId,leftClass,objects,classifierContext);
    int rightOk=HandItemClassifier240A8::evaluate(characterIndex,rightId,rightClass,objects,classifierContext);
    const bool gate=partyGate(characterIndex,ctx);

    // 0x244CE..0x2451D: both hands empty.
    if(leftId==-1 && rightId==-1){
        if(!gate)leftOk=rightOk=0;
        goto cooldowns;
    }

    // 0x24522..0x24597: two spell-created hand objects.
    if(HandItemPredicates::spellHandRange23F34(leftId) && HandItemPredicates::spellHandRange23F34(rightId)){
        leftOk=rightOk=1;
        if(!gate)leftOk=rightOk=0;
        goto cooldowns;
    }

    // 0x2459C..0x245B7: these character states disable both hands.
    if((u16(c.raw,0x10)&0x7240u)!=0u)leftOk=rightOk=0;

    // Empty and spell-hand cases are independently gated for rear characters.
    if(leftId==-1 && !gate)leftOk=0;
    if(rightId==-1 && !gate)rightOk=0;
    if(HandItemPredicates::spellHandRange23F34(leftId))leftOk=gate?1:0;
    if(HandItemPredicates::spellHandRange23F34(rightId))rightOk=gate?1:0;

    if(leftClass!=0 && rightClass!=0){
        const int leftKind=handKind(objects,leftId);
        const int rightKind=handKind(objects,rightId);
        const bool martial=martialGroup(c);

        if(leftKind==rightKind){
            if(rightKind==2){leftOk=rightOk=0;goto cooldowns;}
            if(martial && HandItemPredicates::predicate23EE4(leftId) && HandItemPredicates::predicate23EE4(rightId))
                goto cooldowns;
            keepLighter(leftId,rightId,objects,leftOk,rightOk);
            goto cooldowns;
        }

        if(leftKind>rightKind){
            if(leftKind==2 || rightKind==2){leftOk=rightOk=0;goto cooldowns;}
            if(martial){
                if(HandItemPredicates::predicate23EE4(leftId) && HandItemPredicates::predicate23EE4(rightId))
                    goto cooldowns;
                // 0x248DD..0x24908: right lighter leaves both active; otherwise right is disabled.
                if(weight(objects,rightId)>=weight(objects,leftId))rightOk=0;
                goto cooldowns;
            }
            keepLighter(leftId,rightId,objects,leftOk,rightOk);
            goto cooldowns;
        }

        // leftKind < rightKind (0x2493B..0x24A08).
        if(leftKind==2 || rightKind==2){leftOk=rightOk=0;goto cooldowns;}
        if(martial){
            if(HandItemPredicates::predicate23EE4(leftId) && HandItemPredicates::predicate23EE4(rightId))
                goto cooldowns;
            // For this ordering, unequal weights leave both active; equal weights disable left.
            if(weight(objects,leftId)==weight(objects,rightId))leftOk=0;
            goto cooldowns;
        }
        keepLighter(leftId,rightId,objects,leftOk,rightOk);
        goto cooldowns;
    }

    // 0x24A0A..0x24A56: only one hand belongs to the classified family.
    if(leftClass!=0){
        if(handKind(objects,leftId)==2)rightOk=0;
    }else if(rightClass!=0){
        if(handKind(objects,rightId)==2)leftOk=0;
    }

cooldowns:
    // 0x24A58..0x24A78: independent unaligned DWORD recovery deadlines.
    if(u32(c.raw,0x10B)!=0u)leftOk=0;
    if(u32(c.raw,0x10F)!=0u)rightOk=0;
    if(handIndex==0)return leftOk;
    if(handIndex==1)return rightOk;
    // The original caller only supplies 0/1; an out-of-domain value leaves the local return
    // uninitialized. Keep the modern reconstruction deterministic without inventing hand behavior.
    return 0;
}

} // namespace ravenloft::game
