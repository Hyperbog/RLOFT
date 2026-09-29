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
std::int16_t i16(const std::array<std::uint8_t,0x248>& r,std::size_t o){return static_cast<std::int16_t>(u16(r,o));}
std::uint16_t objU16(const data::ObjectDatabase& objects,int objectId,std::size_t o){
    const auto& r=objects.at(static_cast<std::size_t>(objectId)).raw;
    return static_cast<std::uint16_t>(r[o]) |
        static_cast<std::uint16_t>(static_cast<std::uint16_t>(r[o+1])<<8u);
}
int objectCategory(const data::ObjectDatabase& objects,int objectId){
    if(objectId<0)return -1;
    return objects.at(static_cast<std::size_t>(objectId)).raw[0x2F];
}
bool classOrHighThief(const data::CharacterRecord& c,int directClass){
    for(int slot=0;slot<3;++slot){
        const int cls=static_cast<std::int8_t>(c.raw[0x15+slot]);
        if(cls==directClass)return true;
        if(cls==5 && i16(c.raw,0x20+slot*2)>=10)return true;
    }
    return false;
}
}

int HandItemClassifier240A8::evaluate(std::size_t characterIndex,
                                      int objectId,
                                      int& out,
                                      const data::ObjectDatabase& objects,
                                      const HandItemClassifierContext240A8& ctx){
    if(!ctx.party || characterIndex>=ctx.party->size())throw std::runtime_error("240A8 requires four-party context");
    const auto& c=(*ctx.party)[characterIndex];
    int result=1;

    out=HandItemPredicates::predicate23E58(objectId,objects);
    if(!out)out=HandItemPredicates::predicate23F50(objectId,objects);

    if(out==1){
        if(objectId!=-1){
            const bool mage=HandItemPredicates::characterHasClass23EAC(c,4)!=0;
            const bool fighter=HandItemPredicates::characterHasClass23EAC(c,0)!=0;
            const bool thief=HandItemPredicates::characterHasClass23EAC(c,5)!=0;
            const bool paladin=HandItemPredicates::characterHasClass23EAC(c,1)!=0;
            const bool cleric=HandItemPredicates::characterHasClass23EAC(c,3)!=0;
            if(mage){
                if(!fighter && !thief && !paladin)
                    result=HandItemPredicates::predicate24060(objectId);
                if(thief && !fighter && !paladin)
                    result=HandItemPredicates::predicate24060(objectId) |
                           HandItemPredicates::predicate23FC4(objectId,objects);
            }else if(thief && !fighter && !paladin && !cleric){
                result=HandItemPredicates::predicate23FC4(objectId,objects);
            }
            if(cleric && objectCategory(objects,objectId)==0x0A)result=0;
        }

        // Literal DS:258A8/party-front-status gate at 0x2420D..0x24281.
        // 0x2420D..0x24281: rear-party restriction is bypassed only when BOTH
        // first-rank status words are exactly 0x8000. Any other combination enters
        // the object-specific restriction path.
        if(ctx.global258A8==0xFF && characterIndex>=2 &&
           !(u16((*ctx.party)[0].raw,0x10)==0x8000u &&
             u16((*ctx.party)[1].raw,0x10)==0x8000u)){
            if(objectId==-1)return 0;
            if((objU16(objects,objectId,0x35)&0x0003u)!=0x0002u &&
               !HandItemPredicates::predicate23F50(objectId,objects))return 0;
        }
        return result;
    }

    if(objectId>=0xA7 && objectId<=0xC9)
        return classOrHighThief(c,4)?1:0;
    if((objectId>=0xCA && objectId<=0xE4) || objectId==0x12D)
        return classOrHighThief(c,3)?1:0;
    return result;
}

} // namespace ravenloft::game
