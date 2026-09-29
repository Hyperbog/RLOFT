#include "games/ravenloft/game/PlayerHitTestC80C.h"
#include "games/ravenloft/game/CharacterDefenseHelpers.h"
#include "games/ravenloft/game/OriginalDice1C328.h"
#include <array>
#include <cstdint>
#include <stdexcept>

namespace ravenloft::game {
namespace {
constexpr std::array<int,26> kStrengthHit6832{{
    2,-5,-3,-3,-2,-2,-1,-1,0,0,0,0,0,0,0,0,0,1,1,3,3,4,4,5,6,7
}};
std::int16_t i16(const std::array<std::uint8_t,0x248>& r,std::size_t o){
    return static_cast<std::int16_t>(static_cast<std::uint16_t>(r[o]) |
           (static_cast<std::uint16_t>(r[o+1])<<8u));
}
}

int PlayerHitTestC80C::strengthHitAdjustmentC7C4(const data::CharacterRecord& character){
    const int strength=character.raw[0x33];
    const int exceptional=character.raw[0x3F];
    // Preserve 0xC7DA's original condition. The disassembly contains the
    // literal self-compare at C7DF, so an 18 Strength with nonzero exceptional
    // value takes the exceptional table below.
    if(strength!=18 || exceptional==0){
        if(strength<0 || static_cast<std::size_t>(strength)>=kStrengthHit6832.size())
            throw std::runtime_error("C7C4 Strength outside original DS:6832 table");
        return kStrengthHit6832[static_cast<std::size_t>(strength)];
    }
    if(exceptional<=50)return 1;
    if(exceptional<=99)return 2;
    return 3;
}

PlayerHitTestResultC80C PlayerHitTestC80C::evaluate(std::size_t characterIndex,
                                                     const data::CharacterRecord& character,
                                                     const data::RuntimePlacement22& actor,
                                                     const data::NpcDefinition& definition,
                                                     int itemId,
                                                     bool useStrengthAdjustment,
                                                     const data::ObjectDatabase& objects,
                                                     OriginalRng30CA2& rng,
                                                     const PlayerHitTestContextC80C& context){
    (void)characterIndex; // retained in the API because the original accepts the index.
    PlayerHitTestResultC80C result{};
    if((actor.state & 0x00000008u)!=0){ // runtime byte +0x08 bit 0x08
        result.hit=true;result.forcedActorHit=true;return result;
    }

    int adjust=0;
    if(itemId!=-1){
        if(itemId<0 || static_cast<std::size_t>(itemId)>=objects.count())
            throw std::runtime_error("C80C item outside OBJECTS.DAT");
        adjust=objects.at(static_cast<std::size_t>(itemId)).raw[0x37];
    }
    if(useStrengthAdjustment){
        result.strengthAdjustment=strengthHitAdjustmentC7C4(character);
        adjust-=result.strengthAdjustment;
    }

    result.encumbranceTier=CharacterDefenseHelpers::encumbranceTierFE44(character,objects,context.containerWeightF5F8);
    if(result.encumbranceTier==2)adjust+=1;
    if(result.encumbranceTier==3)adjust+=2;
    if(result.encumbranceTier==4)adjust+=4;

    result.required=static_cast<int>(character.raw[0x10A])-
                    static_cast<int>(static_cast<std::int8_t>(definition.raw[0x21]))+adjust;
    if((actor.state & 0x00000080u)!=0)result.required-=4;

    int rollModifier=0;
    if((character.raw[0x133]&0x01u)!=0)++rollModifier;
    if((character.raw[0x133]&0x08u)!=0)++rollModifier;
    if(i16(character.raw,0x68)==0x54)rollModifier+=3;

    result.roll=OriginalDice1C328::roll(1,20,rollModifier,0,rng);
    // These comparisons are on the already-modified 0x1C328 return, exactly
    // as 0xC8E5..0xC900 performs them (>=20 wins; exactly 1 loses).
    if(result.roll>=20)result.hit=true;
    else if(result.roll==1)result.hit=false;
    else result.hit=result.roll>=result.required;
    return result;
}

} // namespace ravenloft::game
