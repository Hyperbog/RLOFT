#include "games/ravenloft/game/NpcScheduler1BDAC.h"
#include "games/ravenloft/game/NpcMovementTransaction1BBD8.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {

void put16(std::array<std::uint8_t,22>& raw,std::size_t p,std::uint16_t v){
    raw[p]=static_cast<std::uint8_t>(v);
    raw[p+1]=static_cast<std::uint8_t>(v>>8u);
}
void put32(std::array<std::uint8_t,22>& raw,std::size_t p,std::uint32_t v){
    for(int i=0;i<4;++i)raw[p+i]=static_cast<std::uint8_t>(v>>(i*8));
}
void sync(data::RuntimePlacement22& a){
    put32(a.raw,0x00,static_cast<std::uint32_t>(a.yFixed));
    put32(a.raw,0x04,static_cast<std::uint32_t>(a.xFixed));
    put32(a.raw,0x08,a.state);
    put16(a.raw,0x0C,a.field0C);
    put16(a.raw,0x0E,a.type);
    put16(a.raw,0x10,a.field10);
    put16(a.raw,0x12,static_cast<std::uint16_t>(a.hitPoints));
    put16(a.raw,0x14,a.field14);
}
std::uint8_t stateByte(const data::RuntimePlacement22& a,int byte){
    return static_cast<std::uint8_t>(a.state>>(byte*8));
}
void setStateByte(data::RuntimePlacement22& a,int byte,std::uint8_t v){
    const unsigned shift=static_cast<unsigned>(byte*8);
    a.state=(a.state&~(0xFFu<<shift))|(static_cast<std::uint32_t>(v)<<shift);
}
int actionState(std::uint16_t f){return static_cast<int>((f>>1u)&3u);}
void clearActionState(data::RuntimePlacement22& a){a.field14=static_cast<std::uint16_t>(a.field14&0xFFF9u);}
std::uint16_t definitionCooldown(const data::NpcDefinition& d){
    return static_cast<std::uint16_t>(d.raw[0x25]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(d.raw[0x26])<<8u);
}
int animationPhase(std::uint16_t f){return static_cast<int>((f>>6u)&7u);}
void setAnimationPhase(data::RuntimePlacement22& a,int p){
    a.field14=static_cast<std::uint16_t>((a.field14&0xFE3Fu)|((p&7)<<6u));
}
void applyDefinitionCooldown(data::RuntimePlacement22& a,const data::NpcDefinition& d){
    std::uint16_t delay=definitionCooldown(d);
    if((stateByte(a,0)&0x80u)!=0)
        delay=static_cast<std::uint16_t>(delay*3u);
    a.field10=delay;
}

} // namespace

void NpcScheduler1BDAC::execute(std::span<data::RuntimePlacement22> actors,
                                const data::NpcDatabase& definitions,
                                OriginalRng30CA2& rng,
                                const NpcSchedulerContext1BDAC& c){
    // 0x1BDB0..0x1BDB7: DS:BF38 gates the complete primary-NPC pass.
    if(!c.enabledBF38)return;

    for(std::size_t i=0;i<actors.size();++i){
        auto& actor=actors[i];
        if(actor.type>=definitions.count())
            throw std::runtime_error("CODE.2 0x1BDAC actor type outside NPCS.DAT");
        const auto& def=definitions.at(actor.type);

        // 0x1BDDA..0x1BDDE: runtime +0x0B bit 0x02 suppresses this actor.
        if((stateByte(actor,3)&0x02u)!=0){sync(actor);continue;}

        // 0x1BDE4 / 0x1BEDC: nonzero +0x10 is a scheduler countdown only.
        if(actor.field10!=0){
            --actor.field10;
            sync(actor);
            continue;
        }

        // 0x1BDEF..0x1BDF9: clear transient runtime +0x08 bit 0x08.
        if((stateByte(actor,0)&0x08u)!=0)
            setStateByte(actor,0,static_cast<std::uint8_t>(stateByte(actor,0)&0xF7u));

        const int action=actionState(actor.field14);
        switch(action){
            case 1: {
                // Jump-table target 0x1BE19: promote action 1 -> 2 before
                // entering D550, then load the definition cooldown.
                clearActionState(actor);
                actor.field14=static_cast<std::uint16_t>(actor.field14|0x0004u);
                // D550/C9A0 still observes the original packed runtime record
                // for +0x08 bit 0x80, so publish the scheduler's just-cleared
                // transient state byte before crossing the original call boundary.
                sync(actor);
                auto attack=c.attack;
                NpcAttackDispatcherD550::execute(actor,def,static_cast<int>(i),rng,attack);
                applyDefinitionCooldown(actor,def);
                break;
            }
            case 2:
                // Jump-table target 0x1BE4A: fixed seven-tick recovery, with
                // the original +0x08 bit 0x80 tripling rule.
                actor.field10=7;
                clearActionState(actor);
                if((stateByte(actor,0)&0x80u)!=0)
                    actor.field10=static_cast<std::uint16_t>(actor.field10*3u);
                break;
            case 3:
                // Jump-table target 0x1BE6C differs from state 0 only by
                // clearing the action bits before normal movement handling.
                clearActionState(actor);
                [[fallthrough]];
            case 0: {
                // 0x1BE70..0x1BEC5.
                if((stateByte(actor,3)&0x70u)!=0){
                    if(!c.trig || !c.collision2D5D0)
                        throw std::invalid_argument("CODE.2 0x1BDAC movement path requires trig and 0x2D5D0 boundary");
                    NpcMovementCallbacks1BBD8 callbacks;
                    callbacks.decide1B760=[&](data::RuntimePlacement22& a,int moverId,int retry){
                        auto decision=c.decision;
                        NpcDecision1B760::execute(a,def,moverId,retry,rng,decision);
                    };
                    callbacks.collision2D5D0=c.collision2D5D0;
                    NpcMovementTransaction1BBD8::execute(actor,def,static_cast<int>(i),
                                                         c.levelWidth,*c.trig,c.gridSnapC02F,callbacks);
                }else if((stateByte(actor,3)&0x04u)!=0){
                    if(!c.behavior1B4F8)
                        throw std::invalid_argument("CODE.2 0x1BDAC runtime +0x0B bit 0x04 requires 0x1B4F8 boundary");
                    c.behavior1B4F8(actor,def,static_cast<int>(i));
                }else if(def.viewCount!=0){
                    const int next=(animationPhase(actor.field14)+1)%def.viewCount;
                    setAnimationPhase(actor,next);
                }
                applyDefinitionCooldown(actor,def);
                break;
            }
            default: break;
        }
        sync(actor);
    }
}

} // namespace ravenloft::game
