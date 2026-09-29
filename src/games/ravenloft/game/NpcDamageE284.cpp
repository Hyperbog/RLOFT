#include "games/ravenloft/game/NpcDamageE284.h"
#include "games/ravenloft/game/NpcTypeFilter2ACDC.h"
#include <array>

namespace ravenloft::game {
namespace {
void put16(std::array<std::uint8_t,22>& raw,std::size_t p,std::uint16_t v){raw[p]=static_cast<std::uint8_t>(v);raw[p+1]=static_cast<std::uint8_t>(v>>8u);}
void put32(std::array<std::uint8_t,22>& raw,std::size_t p,std::uint32_t v){for(int i=0;i<4;++i)raw[p+i]=static_cast<std::uint8_t>(v>>(i*8));}
void sync(data::RuntimePlacement22& a){
    put32(a.raw,0x00,static_cast<std::uint32_t>(a.yFixed));put32(a.raw,0x04,static_cast<std::uint32_t>(a.xFixed));
    put32(a.raw,0x08,a.state);put16(a.raw,0x0C,a.field0C);put16(a.raw,0x0E,a.type);put16(a.raw,0x10,a.field10);
    put16(a.raw,0x12,static_cast<std::uint16_t>(a.hitPoints));put16(a.raw,0x14,a.field14);
}
std::uint8_t stateByte(const data::RuntimePlacement22& a,int byte){return static_cast<std::uint8_t>(a.state>>(byte*8));}
void setStateByte(data::RuntimePlacement22& a,int byte,std::uint8_t v){
    const unsigned shift=static_cast<unsigned>(byte*8);a.state=(a.state&~(0xFFu<<shift))|(std::uint32_t(v)<<shift);
}
std::uint16_t le16(const std::array<std::uint8_t,69>& r,std::size_t o){return static_cast<std::uint16_t>(r[o])|static_cast<std::uint16_t>(std::uint16_t(r[o+1])<<8u);}
void queue(const NpcDamageContextE284& c,std::uint32_t id,int flag){if(c.queueSound28110)c.queueSound28110(id,0xFF,0,flag);}
}

NpcDamageResultE284 NpcDamageE284::apply(data::RuntimePlacement22& actor,
                                         const data::NpcDefinition& def,
                                         int npcIndex,
                                         int damage,
                                         int mode,
                                         const NpcDamageContextE284& c){
    NpcDamageResultE284 result{};

    // 0xE290..0xE2C1: mode selects the scheduler delay marker. NPCS +0x1C
    // also forces the runtime animation/action bits 1 and 2.
    if(def.raw[0x1C]!=0) actor.field14=static_cast<std::uint16_t>(actor.field14|0x0006u);
    actor.field10=static_cast<std::uint16_t>(mode==0?2:1);

    // 0xE2C1..0xE306. Type 15 bypasses HP subtraction. Type 8 cannot be
    // damaged in phase C000 and otherwise clamps to 10 HP once above 10.
    if(actor.type!=0x0Fu){
        if(actor.type==8u){
            if((actor.field14&0xC000u)!=0xC000u && actor.hitPoints>10){
                const int next=static_cast<int>(actor.hitPoints)-damage;
                actor.hitPoints=static_cast<std::int16_t>(next<=0?10:next);
            }
        }else{
            actor.hitPoints=static_cast<std::int16_t>(static_cast<int>(actor.hitPoints)-damage);
        }
    }

    // 0xE306..0xE31F. For the shared ordinary type family, damage promotes
    // runtime +0x0B action bits to 0x10 while preserving unrelated bits.
    if(NpcTypeFilter2ACDC::includes(actor.type)){
        auto b=stateByte(actor,3);b=static_cast<std::uint8_t>((b&0xCFu)|0x10u);setStateByte(actor,3,b);
    }

    if(actor.hitPoints>0){
        // 0xE553..0xE570: hit/reaction sound uses NPCS +0x41 and sound base
        // 258C0, with final queue argument 1.
        queue(c,c.soundBase258C0+le16(def.raw,0x41),1);
        sync(actor);
        return result;
    }

    result.died=true;

    // 0xE3C9..0xE443: exact opening assassin death script. Argument order is
    // 2195C(scatter,id,0,x,y,1,0); BF44=1 is presentation/event state and is
    // deliberately outside this pure NPC damage core.
    if(actor.type==9u){
        if(c.spawn2195C){
            c.spawn2195C(0x6E,136,0,actor.xFixed,actor.yFixed,1,0);
            c.spawn2195C(0x78,135,0,actor.xFixed,actor.yFixed,1,0);
            c.spawn2195C(0x82,233,0,actor.xFixed,actor.yFixed,1,0);
            c.spawn2195C(0x8C,116,0,actor.xFixed,actor.yFixed,1,0);
        }
        if(c.setEventStateBF44)c.setEventStateBF44(1);
        result.openingAssassinDropsSpawned=true;
    }else if(c.scriptedDeath){
        // Types 0x18/0x21/0x12/0x50/0x08 have separate scripted branches in
        // E284. Keeping the boundary explicit avoids inventing their semantics.
        c.scriptedDeath(actor.type);
    }

    // 0xE4C9..0xE508: death sound uses +0x43. A zero definition sound still
    // queues the base with flag 1; nonzero uses base+sound with flag 0.
    const auto deathSound=le16(def.raw,0x43);
    if(deathSound!=0)queue(c,c.soundBase258BC+deathSound,0);
    else queue(c,c.soundBase258BC,1);

    // 0xE50B..0xE539. The actor is marked suppressed unless its type is one
    // of DS:258AA and the matching E0F8 predicate succeeds.
    bool preserve=false;
    if(c.specialType258AA && c.specialType258AA(actor.type))
        preserve=c.boundaryE0F8 && c.boundaryE0F8(npcIndex)!=0;
    if(!preserve)setStateByte(actor,3,static_cast<std::uint8_t>(stateByte(actor,3)|0x02u));

    if(c.boundaryDDA4)c.boundaryDDA4(le16(def.raw,0x23));
    sync(actor);
    return result;
}

} // namespace ravenloft::game
