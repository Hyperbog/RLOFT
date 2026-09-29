#include "games/ravenloft/game/NpcDecision1B760.h"
#include "games/ravenloft/game/NpcDirectionSelector1AF90.h"
#include "games/ravenloft/game/NpcType8Decision1B27C.h"
#include "games/ravenloft/game/PartyNpcPredicate1B6EC.h"
#include <cstdlib>
#include <stdexcept>

namespace ravenloft::game {
namespace {
void put16(std::array<std::uint8_t,22>& raw,std::size_t p,std::uint16_t v){raw[p]=static_cast<std::uint8_t>(v);raw[p+1]=static_cast<std::uint8_t>(v>>8);}
void put32(std::array<std::uint8_t,22>& raw,std::size_t p,std::uint32_t v){for(int i=0;i<4;++i)raw[p+i]=static_cast<std::uint8_t>(v>>(i*8));}
void sync(data::RuntimePlacement22& a){
    put32(a.raw,0x00,static_cast<std::uint32_t>(a.yFixed));
    put32(a.raw,0x04,static_cast<std::uint32_t>(a.xFixed));
    put32(a.raw,0x08,a.state);put16(a.raw,0x0C,a.field0C);put16(a.raw,0x0E,a.type);
    put16(a.raw,0x10,a.field10);put16(a.raw,0x12,static_cast<std::uint16_t>(a.hitPoints));put16(a.raw,0x14,a.field14);
}
struct SyncGuard{data::RuntimePlacement22& a;~SyncGuard(){sync(a);}};
std::uint8_t stateByte(const data::RuntimePlacement22& a,int byte){return static_cast<std::uint8_t>(a.state>>(byte*8));}
void setStateByte(data::RuntimePlacement22& a,int byte,std::uint8_t v){const auto sh=byte*8; a.state=(a.state&~(0xFFu<<sh))|(std::uint32_t(v)<<sh);}
int direction(std::uint16_t f){return (f>>3u)&7;}
int phase(std::uint16_t f){return (f>>6u)&7;}
void setDirection(data::RuntimePlacement22& a,int d){a.field14=static_cast<std::uint16_t>((a.field14&0xFFC7u)|((d&7)<<3u));}
void setHighPhase(data::RuntimePlacement22& a,int p){a.field14=static_cast<std::uint16_t>((a.field14&0x3FFFu)|((p&3)<<14u));}
std::uint16_t scaled(std::uint16_t r,std::uint32_t n){return static_cast<std::uint16_t>((std::uint32_t(r)*n)>>15u);}
std::int16_t highSigned(std::int32_t f){return static_cast<std::int16_t>(static_cast<std::uint32_t>(f)>>16u);}
void seedFromBios(OriginalRng30CA2& rng,const NpcDecisionContext1B760& c){
    if(!c.biosTick311E0)throw std::invalid_argument("CODE.2 0x1B760 normal path requires BIOS tick boundary 0x311E0");
    const auto first=c.biosTick311E0();
    rng.seedFromBiosLowWord(first,c.biosTick311E0);
}
bool predicate(const NpcDecisionContext1B760& c,const data::NpcDefinition& d){
    if(!c.party)throw std::invalid_argument("CODE.2 0x1B760 requires party records for 0x1B6EC");
    return PartyNpcPredicate1B6EC::evaluate(*c.party,d);
}
}

void NpcDecision1B760::execute(data::RuntimePlacement22& actor,
                                const data::NpcDefinition& definition,
                                int moverId,
                                int retry,
                                OriginalRng30CA2& rng,
                                const NpcDecisionContext1B760& c){
    SyncGuard guard{actor};

    // 0x1B76A..0x1B780: type 8 delegates completely and returns before normal reseeding.
    if(actor.type==8){
        NpcType8Decision1B27C::execute(actor,moverId,c.partyXFixed,c.partyYFixed,rng,c.type8Relocate);
        return;
    }

    // 0x1B785..0x1B802: type 15 cell-gated animation/attack behavior.
    if(actor.type==0x0F){
        if(actor.field0C>=c.levelCells.size())throw std::out_of_range("CODE.2 0x1B760 type-15 cell index");
        if((c.levelCells[actor.field0C]&0x0040u)==0)return;
        if(scaled(rng.next15(),1000u)>=975u){
            actor.field14=static_cast<std::uint16_t>((actor.field14&0xFFF9u)|0x0002u);
            return;
        }
        actor.field14=static_cast<std::uint16_t>(actor.field14&0xFFF9u);
        if(definition.viewCount==0)throw std::runtime_error("CODE.2 0x1B760 type-15 viewCount is zero");
        const auto next=static_cast<unsigned>((phase(actor.field14)+1u)%definition.viewCount)&7u;
        actor.field14=static_cast<std::uint16_t>((actor.field14&0xFE3Fu)|(next<<6u));
        return;
    }

    // 0x1B803..0x1B840: state +0B bit 0x40 random-wander/countdown path.
    if((stateByte(actor,3)&0x40u)!=0){
        setDirection(actor,scaled(rng.next15(),8u)&7u);
        actor.field14=static_cast<std::uint16_t>(actor.field14|0x0001u);
        actor.hitPoints=static_cast<std::int16_t>(scaled(rng.next15(),25u));
        return;
    }

    // 0x1B841: normal path seeds from the BIOS time source at entry.
    seedFromBios(rng,c);

    std::int32_t dx=0,dy=0;
    if(predicate(c,definition)){
        const auto rx=static_cast<std::int32_t>((static_cast<std::int64_t>(c.levelWidth)*rng.next15())>>15u);
        const auto ry=static_cast<std::int32_t>((static_cast<std::int64_t>(c.levelHeight)*rng.next15())>>15u);
        dx=static_cast<std::int32_t>(static_cast<std::uint32_t>(c.partyXFixed)-static_cast<std::uint32_t>(rx<<16));
        dy=static_cast<std::int32_t>(static_cast<std::uint32_t>(c.partyYFixed)-static_cast<std::uint32_t>(ry<<16));
    }else{
        const auto dxInt=static_cast<std::int32_t>(highSigned(c.partyXFixed))-static_cast<std::int32_t>(highSigned(actor.xFixed));
        const auto dyInt=static_cast<std::int32_t>(highSigned(c.partyYFixed))-static_cast<std::int32_t>(highSigned(actor.yFixed));
        dx=static_cast<std::int32_t>(static_cast<std::uint32_t>(dxInt)<<16u);
        dy=static_cast<std::int32_t>(static_cast<std::uint32_t>(dyInt)<<16u);
    }

    int desired=NpcDirectionSelector1AF90::select(dx,dy,direction(actor.field14),retry,rng)&7;
    if((stateByte(actor,0)&0x10u)!=0)desired=(desired+4)&7;
    if(predicate(c,definition))desired=(desired+4)&7;

    if(dx==0||dy==0)actor.field14=static_cast<std::uint16_t>(actor.field14&0x3FFFu);
    const auto ax=std::abs(static_cast<std::int64_t>(dx));
    const auto ay=std::abs(static_cast<std::int64_t>(dy));

    if(ax<=0xA0000LL && ay<=0xA0000LL){
        if((stateByte(actor,3)&0x20u)!=0){
            setStateByte(actor,3,static_cast<std::uint8_t>((stateByte(actor,3)&0xCFu)|0x10u));
        }
        actor.field14=static_cast<std::uint16_t>(actor.field14|0x0001u);
        if((stateByte(actor,0)&0x10u)==0){
            actor.field14=static_cast<std::uint16_t>(actor.field14&0x3FFFu);
            if(scaled(rng.next15(),1000u)>990u && c.queueSound28110){
                c.queueSound28110(c.soundBase258C0+definition.ambientSound,0xFF,0,1);
            }
            if(ax<=0x15000LL && ay<=0x15000LL && retry==0 && direction(actor.field14)==desired){
                actor.field14=static_cast<std::uint16_t>((actor.field14&0xFFF9u)|0x0002u);
                setStateByte(actor,3,static_cast<std::uint8_t>((stateByte(actor,3)&0xCFu)|0x10u));
                return; // original early return intentionally skips the final reseed
            }
        }
    }else{
        if(ax>=0xD0000LL || ay>=0xD0000LL){
            if((stateByte(actor,3)&0x10u)!=0){
                setStateByte(actor,3,static_cast<std::uint8_t>((stateByte(actor,3)&0xCFu)|0x20u));
                actor.field14=static_cast<std::uint16_t>(actor.field14&0xFFFEu);
            }
        }else actor.field14=static_cast<std::uint16_t>(actor.field14|0x0001u);
    }

    // 0x1BA43: type-9 endpoint immediately snaps direction to the selected direction.
    if(actor.type==9 && highSigned(actor.xFixed)==24 && highSigned(actor.yFixed)==7 && direction(actor.field14)!=desired)
        setDirection(actor,desired);

    // 0x1BA81: nonzero high phase decrements and returns before the final reseed.
    if((actor.field14&0xC000u)!=0){
        actor.field14=static_cast<std::uint16_t>(actor.field14|0x0001u);
        setHighPhase(actor,((actor.field14>>14u)-1u)&3u);
        return;
    }

    // 0x1BAB2: retry phases turn immediately and get a random high phase (types other than 0x21).
    if(retry!=0 && actor.type!=0x21){
        setDirection(actor,desired);
        setHighPhase(actor,(scaled(rng.next15(),2u)+1u)&3u);
        actor.field14=static_cast<std::uint16_t>(actor.field14|0x0001u);
    }

    // 0x1BAF2: type 0x21 preserves its current direction and cancels movement.
    if(actor.type==0x21){
        desired=direction(actor.field14);
        actor.field14=static_cast<std::uint16_t>(actor.field14&0xFFFEu);
    }

    const int current=direction(actor.field14);
    if(current!=desired){
        if(actor.type==9){
            setDirection(actor,desired);
        }else{
            static constexpr int turnStep[8]={0,7,7,7,1,1,1,1}; // DS:A7F8
            const int diff=(current+8-desired)&7;
            setDirection(actor,(current+turnStep[diff])&7);
            actor.field14=static_cast<std::uint16_t>(actor.field14&0xFFFEu);
        }
    }else if(!predicate(c,definition)){
        if((actor.field14&0x0001u)!=0){
            if((stateByte(actor,3)&0x20u)!=0)actor.field14=static_cast<std::uint16_t>(actor.field14&0xFFFEu);
        }else if(retry==0 && (stateByte(actor,3)&0x30u)!=0){
            actor.field14=static_cast<std::uint16_t>((actor.field14&0xFFF9u)|0x0002u);
            setStateByte(actor,3,static_cast<std::uint8_t>((stateByte(actor,3)&0xCFu)|0x10u));
        }
    }

    // 0x1BBBD: normal completion reseeds again from BIOS time.
    seedFromBios(rng,c);
}

} // namespace ravenloft::game
