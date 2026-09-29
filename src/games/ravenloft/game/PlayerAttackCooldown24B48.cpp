#include "games/ravenloft/game/PlayerAttackCooldown24B48.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
void put32(std::array<std::uint8_t,0x248>& r,std::size_t o,std::uint32_t v){
    for(int i=0;i<4;++i)r[o+static_cast<std::size_t>(i)]=static_cast<std::uint8_t>(v>>(i*8));
}
}

void PlayerAttackCooldown24B48::apply(data::CharacterRecord& character,
                                       int handIndex,
                                       int duration,
                                       const PlayerAttackCooldownContext24B48& context){
    if(handIndex<0 || handIndex>1)throw std::out_of_range("24B48 hand index");
    if(duration==0)return;
    if(!context.currentTick19CC4)throw std::runtime_error("24B48 requires 0x19CC4 clock boundary");
    put32(character.raw,0x10B+static_cast<std::size_t>(handIndex)*4u,
          context.currentTick19CC4()+static_cast<std::uint32_t>(duration));
}

} // namespace ravenloft::game
