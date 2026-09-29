#include "games/ravenloft/game/NpcType8Decision1B27C.h"
#include "games/ravenloft/game/NpcDirectionSelector1AF90.h"
#include <cstdlib>

namespace ravenloft::game {
namespace {
std::uint16_t scaled100(std::uint16_t r){return static_cast<std::uint16_t>((std::uint32_t(r)*100u)>>15u);}
int dir(std::uint16_t f){return (f>>3u)&7;}
}
void NpcType8Decision1B27C::execute(data::RuntimePlacement22& actor,
                                    int moverId,
                                    std::int32_t partyXFixed,
                                    std::int32_t partyYFixed,
                                    OriginalRng30CA2& rng,
                                    const NpcType8RelocateCallbacks1B1C4& relocateCallbacks){
    const auto deltaY=static_cast<std::int32_t>(static_cast<std::uint32_t>(partyYFixed)-static_cast<std::uint32_t>(actor.yFixed));
    const auto deltaX=static_cast<std::int32_t>(static_cast<std::uint32_t>(partyXFixed)-static_cast<std::uint32_t>(actor.xFixed));
    const int chosen=NpcDirectionSelector1AF90::select(deltaX,deltaY,dir(actor.field14),0,rng)&7;
    actor.field14=static_cast<std::uint16_t>((actor.field14&0xFFC7u)|(chosen<<3u));
    actor.field14=static_cast<std::uint16_t>(actor.field14|0x0001u);

    if((actor.field14&0xC000u)==0xC000u){
        NpcType8Relocate1B1C4::execute(actor,moverId,rng,relocateCallbacks);
        actor.field14=static_cast<std::uint16_t>(actor.field14&0xFFFEu);
        return;
    }

    if(scaled100(rng.next15())>95u){
        actor.field14=static_cast<std::uint16_t>((actor.field14|0xC000u)&0xFE3Eu);
        return;
    }

    const int ax=static_cast<std::int16_t>(static_cast<std::uint32_t>(actor.xFixed)>>16u);
    const int ay=static_cast<std::int16_t>(static_cast<std::uint32_t>(actor.yFixed)>>16u);
    const int px=static_cast<std::int16_t>(static_cast<std::uint32_t>(partyXFixed)>>16u);
    const int py=static_cast<std::int16_t>(static_cast<std::uint32_t>(partyYFixed)>>16u);
    if(std::abs(ax-px)<=1 && std::abs(ay-py)<=1){
        actor.field14=static_cast<std::uint16_t>((actor.field14|0x0002u)&0x3FFAu);
        if(scaled100(rng.next15())>90u)
            actor.field14=static_cast<std::uint16_t>((actor.field14&0x3FFFu)|0x4000u);
        return;
    }

    if(scaled100(rng.next15())<25u){
        actor.field14=static_cast<std::uint16_t>((actor.field14|0x8002u)&0xBFFAu);
    }
}
} // namespace ravenloft::game
