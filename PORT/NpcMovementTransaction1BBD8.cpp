#include "games/ravenloft/game/NpcMovementTransaction1BBD8.h"
#include "games/ravenloft/game/MovementVector2DF2C.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
void put16(std::array<std::uint8_t,22>& raw,std::size_t p,std::uint16_t v){raw[p]=static_cast<std::uint8_t>(v);raw[p+1]=static_cast<std::uint8_t>(v>>8);}
void put32(std::array<std::uint8_t,22>& raw,std::size_t p,std::uint32_t v){for(int i=0;i<4;++i)raw[p+i]=static_cast<std::uint8_t>(v>>(i*8));}
void sync(data::RuntimePlacement22& a){
    put32(a.raw,0x00,static_cast<std::uint32_t>(a.yFixed));
    put32(a.raw,0x04,static_cast<std::uint32_t>(a.xFixed));
    put32(a.raw,0x08,a.state);
    put16(a.raw,0x0C,a.field0C);put16(a.raw,0x0E,a.type);put16(a.raw,0x10,a.field10);
    put16(a.raw,0x12,static_cast<std::uint16_t>(a.hitPoints));put16(a.raw,0x14,a.field14);
}
std::uint8_t lowByte(std::uint32_t v,int byte){return static_cast<std::uint8_t>(v>>(byte*8));}
void setStateByte(data::RuntimePlacement22& a,int byte,std::uint8_t v){
    const auto shift=byte*8; const auto mask=~(0xFFu<<shift); a.state=(a.state&mask)|(std::uint32_t(v)<<shift);
}
std::uint8_t stateByte(const data::RuntimePlacement22& a,int byte){return lowByte(a.state,byte);}
std::uint8_t field14Lo(const data::RuntimePlacement22& a){return static_cast<std::uint8_t>(a.field14);}
std::uint8_t field14Hi(const data::RuntimePlacement22& a){return static_cast<std::uint8_t>(a.field14>>8);}
void setField14Lo(data::RuntimePlacement22& a,std::uint8_t v){a.field14=static_cast<std::uint16_t>((a.field14&0xFF00u)|v);}
void setField14Hi(data::RuntimePlacement22& a,std::uint8_t v){a.field14=static_cast<std::uint16_t>((a.field14&0x00FFu)|(std::uint16_t(v)<<8));}
int directionBits(std::uint16_t f){return (f>>3)&7;}
int phaseBits(std::uint16_t f){return (f>>6)&7;}
}

void NpcMovementTransaction1BBD8::execute(data::RuntimePlacement22& actor,
                                          const data::NpcDefinition& definition,
                                          int moverId,
                                          std::uint16_t levelWidth,
                                          const data::TrigTable& trig,
                                          bool gridSnapC02F,
                                          const NpcMovementCallbacks1BBD8& callbacks) {
    if(!callbacks.decide1B760 || !callbacks.collision2D5D0)
        throw std::invalid_argument("CODE.2 0x1BBD8 requires 0x1B760 and 0x2D5D0 boundaries");

    int retry=0;
    // 0x1BBED..0x1BC10.
    if ((stateByte(actor,3)&0x40u)!=0) {
        if(actor.hitPoints==0) callbacks.decide1B760(actor,moverId,1);
        else { --actor.hitPoints; setField14Lo(actor,static_cast<std::uint8_t>(field14Lo(actor)|0x01u)); }
    } else callbacks.decide1B760(actor,moverId,0);

    for (;;) {
        if ((field14Lo(actor)&0x01u)==0) { sync(actor); return; }

        // 0x1BC1D..0x1BC56: type 9 special scripted whole-X-cell movement.
        if(actor.type==9) {
            const auto xInt=static_cast<std::int16_t>(static_cast<std::uint32_t>(actor.xFixed)>>16u);
            const auto yInt=static_cast<std::int16_t>(static_cast<std::uint32_t>(actor.yFixed)>>16u);
            if(!(xInt==24 && yInt==7)) {
                const auto nextX=static_cast<std::int16_t>(xInt+1);
                actor.xFixed=static_cast<std::int32_t>((static_cast<std::uint32_t>(static_cast<std::uint16_t>(nextX))<<16u) |
                                                       (static_cast<std::uint32_t>(actor.xFixed)&0xFFFFu));
                actor.field0C=static_cast<std::uint16_t>(static_cast<std::uint16_t>(yInt)*levelWidth + static_cast<std::uint16_t>(nextX));
            }
            setField14Lo(actor,static_cast<std::uint8_t>(field14Lo(actor)&0xFEu));
            continue;
        }

        std::int32_t candidateX=actor.xFixed, candidateY=actor.yFixed;
        // 0x1BC5B..0x1BCDD. In C02F mode odd phase values suppress displacement.
        const bool suppressVector=gridSnapC02F && ((phaseBits(actor.field14)&1)!=0);
        if(!suppressVector) {
            const auto phase=phaseBits(actor.field14);
            const std::int32_t magnitude=static_cast<std::uint16_t>(definition.raw.at(0x29+phase*2)) |
                                         (std::int32_t(static_cast<std::uint16_t>(definition.raw.at(0x2A+phase*2)))<<8);
            int angle=(8-directionBits(actor.field14))*45; if(angle>=360)angle-=360;
            const auto v=MovementVector2DF2C::compute(trig,magnitude,angle,gridSnapC02F);
            candidateX=static_cast<std::int32_t>(static_cast<std::uint32_t>(actor.xFixed)+static_cast<std::uint32_t>(v.deltaX));
            candidateY=static_cast<std::int32_t>(static_cast<std::uint32_t>(actor.yFixed)+static_cast<std::uint32_t>(v.deltaY));
        }

        const int collision=callbacks.collision2D5D0(candidateY,candidateX,moverId);
        if(collision!=0) {
            actor.xFixed=candidateX; actor.yFixed=candidateY;
            const auto xInt=static_cast<std::uint16_t>(static_cast<std::uint32_t>(candidateX)>>16u);
            const auto yInt=static_cast<std::uint16_t>(static_cast<std::uint32_t>(candidateY)>>16u);
            actor.field0C=static_cast<std::uint16_t>(yInt*levelWidth+xInt);

            const auto frameCount=definition.raw.at(0x19);
            if(frameCount==0) throw std::runtime_error("CODE.2 0x1BBD8 movement frame count is zero");
            const auto nextPhase=static_cast<std::uint16_t>((phaseBits(actor.field14)+1u)%frameCount);
            actor.field14=static_cast<std::uint16_t>((actor.field14&0xFE3Fu)|(nextPhase<<6u));
            setField14Lo(actor,static_cast<std::uint8_t>(field14Lo(actor)&0xFEu));
            continue;
        }

        if(actor.type==8) {
            setField14Hi(actor,static_cast<std::uint8_t>(field14Hi(actor)|0xC0u));
            actor.field14=static_cast<std::uint16_t>(actor.field14&0xFE3Eu);
            setField14Lo(actor,static_cast<std::uint8_t>(field14Lo(actor)&0xFEu));
            continue;
        }

        if(retry<3) {
            ++retry;
            setField14Hi(actor,static_cast<std::uint8_t>(field14Hi(actor)&0x3Fu));
            callbacks.decide1B760(actor,moverId,retry);
            continue;
        }

        // 0x1BD84..0x1BD88: fourth rejection clears runtime +8 bit 0x10 and movement bit.
        setStateByte(actor,0,static_cast<std::uint8_t>(stateByte(actor,0)&0xEFu));
        setField14Lo(actor,static_cast<std::uint8_t>(field14Lo(actor)&0xFEu));
    }
}

} // namespace ravenloft::game
