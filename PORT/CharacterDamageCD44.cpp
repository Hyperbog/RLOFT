#include "games/ravenloft/game/CharacterDamageCD44.h"
#include <cstdint>
#include <stdexcept>

namespace ravenloft::game {
namespace {
std::uint16_t u16(const std::array<std::uint8_t,0x248>& r,std::size_t o){
    return static_cast<std::uint16_t>(r[o]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(r[o+1])<<8u);
}
void put16(std::array<std::uint8_t,0x248>& r,std::size_t o,std::uint16_t v){
    r[o]=static_cast<std::uint8_t>(v&0xFFu);
    r[o+1]=static_cast<std::uint8_t>((v>>8u)&0xFFu);
}
std::uint32_t u32(const std::array<std::uint8_t,0x248>& r,std::size_t o){
    return static_cast<std::uint32_t>(r[o]) |
           (static_cast<std::uint32_t>(r[o+1])<<8u) |
           (static_cast<std::uint32_t>(r[o+2])<<16u) |
           (static_cast<std::uint32_t>(r[o+3])<<24u);
}
std::int32_t i32(const std::array<std::uint8_t,0x248>& r,std::size_t o){
    return static_cast<std::int32_t>(u32(r,o));
}
void put32(std::array<std::uint8_t,0x248>& r,std::size_t o,std::uint32_t v){
    r[o]=static_cast<std::uint8_t>(v&0xFFu);
    r[o+1]=static_cast<std::uint8_t>((v>>8u)&0xFFu);
    r[o+2]=static_cast<std::uint8_t>((v>>16u)&0xFFu);
    r[o+3]=static_cast<std::uint8_t>((v>>24u)&0xFFu);
}
void putI32(std::array<std::uint8_t,0x248>& r,std::size_t o,std::int32_t v){
    put32(r,o,static_cast<std::uint32_t>(v));
}
std::int32_t subWrapI32(std::int32_t a,std::int32_t b){
    const auto ua=static_cast<std::uint32_t>(a);
    const auto ub=static_cast<std::uint32_t>(b);
    return static_cast<std::int32_t>(ua-ub);
}
}

CharacterDamageResultCD44 CharacterDamageCD44::apply(std::size_t characterIndex,
                                                       int damage,
                                                       std::array<data::CharacterRecord,4>& party,
                                                       const CharacterDamageContextCD44& ctx){
    if(characterIndex>=party.size())throw std::runtime_error("CharacterDamageCD44 character index out of range");
    auto& c=party[characterIndex];
    auto& r=c.raw;
    CharacterDamageResultCD44 result;
    result.originalDamage=damage;
    int remaining=damage;

    // 0xCD55 / 0xCD60: incoming-damage display uses slot 2 of the three-entry
    // arrays at +0x19E/+0x1AA, expiring 0x36 ticks after the current clock.
    putI32(r,0x1B2,static_cast<std::int32_t>(damage));
    const std::uint32_t now=ctx.currentTick19CC4?ctx.currentTick19CC4():0u;
    put32(r,0x1A6,now+0x36u);

    // +0x117 is damaged by the full incoming amount but does NOT absorb it.
    std::int32_t field117=i32(r,0x117);
    if(field117!=0){
        field117=subWrapI32(field117,static_cast<std::int32_t>(damage));
        if(field117<=0){
            field117=0;
            r[0x132]&=0xFEu;
        }
        putI32(r,0x117,field117);
    }

    // +0x1EA absorbs first.
    std::int32_t pool1EA=i32(r,0x1EA);
    if(pool1EA!=0){
        if(remaining>=pool1EA){
            remaining-=pool1EA;
            pool1EA=0;
        }else{
            pool1EA-=remaining;
            remaining=0;
        }
        putI32(r,0x1EA,pool1EA);
    }

    // +0x113 absorbs second.
    std::int32_t pool113=i32(r,0x113);
    if(pool113!=0){
        if(remaining>=pool113){
            remaining-=pool113;
            pool113=0;
        }else{
            pool113-=remaining;
            remaining=0;
        }
        putI32(r,0x113,pool113);
    }
    result.remainingDamage=remaining;

    // Original WORD subtraction uses SI and then performs a signed <= 0 test.
    std::uint16_t hp=u16(r,0x1C);
    hp=static_cast<std::uint16_t>(hp-static_cast<std::uint16_t>(remaining));
    put16(r,0x1C,hp);
    if(ctx.cf5aCharacterFlags)(*ctx.cf5aCharacterFlags)[characterIndex]=1;

    if(static_cast<std::int16_t>(hp)<=0){
        result.died=true;
        put16(r,0x1C,0);
        // OR high status byte with 0x40, then AND that byte with 0xE3.
        std::uint16_t status=u16(r,0x10);
        status=static_cast<std::uint16_t>((status|0x4000u)&0xE3FFu);
        put16(r,0x10,status);
        c.status=status;
        c.currentHitPoints=0;

        if(ctx.onDeathAD24)ctx.onDeathAD24(characterIndex);

        const std::size_t pair=characterIndex/2u;
        if(pair<2 && ctx.d681PairActive && ctx.d8a8PairCharacter &&
           (*ctx.d681PairActive)[pair]!=0 &&
           static_cast<std::size_t>((*ctx.d8a8PairCharacter)[pair])==characterIndex){
            if(ctx.cf5aCharacterFlags){
                (*ctx.cf5aCharacterFlags)[pair*2u]=1;
                (*ctx.cf5aCharacterFlags)[pair*2u+1u]=1;
            }
            (*ctx.d681PairActive)[pair]=0;
        }

        if(ctx.query14A60 && ctx.query14A60(0x578)==static_cast<int>(characterIndex) &&
           ctx.query1C1DC && ctx.query1C1DC(0x46)==0 && ctx.bf44){
            *ctx.bf44=0x6A;
        }
    }else{
        c.currentHitPoints=hp;
        c.status=u16(r,0x10);
    }

    bool allDown=true;
    for(auto& member:party){
        const std::uint16_t status=u16(member.raw,0x10);
        if((status&0xD040u)==0){ allDown=false; break; }
    }
    if(allDown){
        result.allPartyDown=true;
        if(ctx.allPartyDownCBC8)ctx.allPartyDownCBC8();
    }
    return result;
}

} // namespace ravenloft::game
