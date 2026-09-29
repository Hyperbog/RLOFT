#include "games/ravenloft/game/PlayerAttackStateE578.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
void zero32(std::array<std::uint8_t,0x248>& r,std::size_t o){
    r[o]=r[o+1]=r[o+2]=r[o+3]=0;
}
}

void PlayerAttackStateE578::clear(std::array<data::CharacterRecord,4>& party,
                                   std::size_t characterIndex,
                                   std::array<std::uint8_t,4>* dirtyFlags){
    if(characterIndex>=party.size())throw std::out_of_range("E578 character index");
    for(std::size_t i=0;i<party.size();++i){
        if((party[i].raw[0x132]&0x20u)!=0){
            party[i].raw[0x132]=static_cast<std::uint8_t>(party[i].raw[0x132]&0xDFu);
            if(dirtyFlags)(*dirtyFlags)[i]=1;
        }
    }
    auto& c=party[characterIndex];
    if((c.raw[0x133]&0x02u)!=0){
        zero32(c.raw,0x15A);
        c.raw[0x133]=static_cast<std::uint8_t>(c.raw[0x133]&0xFDu);
        if(dirtyFlags)(*dirtyFlags)[characterIndex]=1;
    }
}

} // namespace ravenloft::game
