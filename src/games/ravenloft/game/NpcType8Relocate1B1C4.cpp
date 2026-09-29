#include "games/ravenloft/game/NpcType8Relocate1B1C4.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
void put16(std::array<std::uint8_t,22>& raw,std::size_t p,std::uint16_t v){raw[p]=static_cast<std::uint8_t>(v);raw[p+1]=static_cast<std::uint8_t>(v>>8);}
void put32(std::array<std::uint8_t,22>& raw,std::size_t p,std::uint32_t v){for(int i=0;i<4;++i)raw[p+i]=static_cast<std::uint8_t>(v>>(i*8));}
void sync(data::RuntimePlacement22& a){put32(a.raw,0,static_cast<std::uint32_t>(a.yFixed));put32(a.raw,4,static_cast<std::uint32_t>(a.xFixed));put16(a.raw,0x14,a.field14);}
std::uint16_t scaled(std::uint16_t r,std::uint32_t n){return static_cast<std::uint16_t>((std::uint32_t(r)*n)>>15u);}
}
void NpcType8Relocate1B1C4::execute(data::RuntimePlacement22& actor,
                                    int moverId,
                                    OriginalRng30CA2& rng,
                                    const NpcType8RelocateCallbacks1B1C4& callbacks){
    if(!callbacks.collision2D5D0)throw std::invalid_argument("CODE.2 0x1B1C4 requires 0x2D5D0 boundary");
    const auto phase=static_cast<unsigned>((actor.field14>>6u)&7u);
    const auto next=static_cast<std::uint16_t>((phase+1u)%6u);
    actor.field14=static_cast<std::uint16_t>((actor.field14&0xFE3Fu)|(next<<6u));
    if((actor.field14&0x01C0u)!=0){sync(actor);return;}

    for(;;){
        const std::uint16_t xInt=static_cast<std::uint16_t>(scaled(rng.next15(),12u)+0x42u);
        const std::uint16_t xFrac=scaled(rng.next15(),0xFFFFu);
        const std::uint16_t yInt=static_cast<std::uint16_t>(scaled(rng.next15(),14u)+0x2Du);
        const std::uint16_t yFrac=scaled(rng.next15(),0xFFFFu);
        const auto y=static_cast<std::int32_t>((std::uint32_t(yInt)<<16u)|yFrac);
        const auto x=static_cast<std::int32_t>((std::uint32_t(xInt)<<16u)|xFrac);
        if(callbacks.collision2D5D0(y,x,moverId)==0)continue;
        actor.yFixed=y;actor.xFixed=x;
        actor.field14=static_cast<std::uint16_t>(actor.field14&0x3FFFu); // byte +15 &= 0x3F
        sync(actor);return;
    }
}
} // namespace ravenloft::game
