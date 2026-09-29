#include "games/ravenloft/game/PlayerHandAttack26018.h"
#include "games/ravenloft/game/HandItemPredicates.h"
#include <cstdint>
#include <stdexcept>

namespace ravenloft::game {
namespace {
std::int16_t i16(const std::array<std::uint8_t,0x248>& r,std::size_t o){
    return static_cast<std::int16_t>(static_cast<std::uint16_t>(r[o])|
           (static_cast<std::uint16_t>(r[o+1])<<8u));
}
std::uint32_t u32(const std::array<std::uint8_t,0x248>& r,std::size_t o){
    return static_cast<std::uint32_t>(r[o])|(static_cast<std::uint32_t>(r[o+1])<<8u)|
           (static_cast<std::uint32_t>(r[o+2])<<16u)|(static_cast<std::uint32_t>(r[o+3])<<24u);
}
}

PlayerHandAttackResult26018 PlayerHandAttack26018::execute(std::array<data::CharacterRecord,4>& party,
                                                             std::size_t characterIndex,
                                                             int handIndex,
                                                             std::span<data::RuntimePlacement22> actors,
                                                             const data::NpcDatabase& npcs,
                                                             int targetNpcIndex,
                                                             const data::ObjectDatabase& objects,
                                                             OriginalRng30CA2& rng,
                                                             const PlayerHandAttackContext26018& context){
    if(characterIndex>=party.size())throw std::out_of_range("26018 character index");
    if(handIndex<0 || handIndex>1)throw std::out_of_range("26018 hand index");
    if(context.boundary19CCC)context.boundary19CCC(0);

    PlayerHandAttackResult26018 result{};
    auto& character=party[characterIndex];
    const auto hand=static_cast<std::size_t>(handIndex);
    if(u32(character.raw,0x10B+hand*4u)!=0){
        result.path=PlayerHandAttackResult26018::Path::CoolingDown;
        return result;
    }

    result.itemId=i16(character.raw,0x5C+hand*2u);
    if(result.itemId==-1)result.itemType=0x0A;
    else{
        if(result.itemId<0 || static_cast<std::size_t>(result.itemId)>=objects.count())
            throw std::runtime_error("26018 equipped item outside OBJECTS.DAT");
        result.itemType=objects.at(static_cast<std::size_t>(result.itemId)).raw[0x2F];
    }

    if(HandItemPredicates::predicate23F50(result.itemId,objects)){
        result.path=PlayerHandAttackResult26018::Path::Throwable;
        return result;
    }
    if(result.itemType>=9 && result.itemType<=11){
        result.path=PlayerHandAttackResult26018::Path::Melee;
        result.melee=PlayerNpcAttackE5F0::execute(party,characterIndex,handIndex,result.itemId,
                                                   actors,npcs,targetNpcIndex,
                                                   objects,rng,context.melee);
        return result;
    }
    result.path=PlayerHandAttackResult26018::Path::OtherUnsupported;
    return result;
}

} // namespace ravenloft::game
