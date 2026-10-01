#include "CharacterSheet38A4.h"

#include <bit>
#include <cstdint>

namespace ravenloft::game {
namespace {
constexpr std::uint32_t add32(std::uint32_t a,std::uint32_t b) noexcept{return a+b;}
constexpr std::uint32_t mul32(std::uint32_t a,std::uint32_t b) noexcept{return a*b;}
constexpr std::uint32_t u32(std::int32_t v) noexcept{return std::bit_cast<std::uint32_t>(v);}
constexpr std::int32_t sx8(std::uint8_t v) noexcept{return static_cast<std::int8_t>(v);}
constexpr std::int32_t sx16(std::uint16_t v) noexcept{return static_cast<std::int16_t>(v);}
constexpr std::uint32_t base(const CharacterSheetOps38A4&o) noexcept{return mul32(o.readDword(0x4CCu),0x248u);}
}

void CharacterSheet38A4::draw(std::uint32_t x,std::uint32_t y,const CharacterSheetOps38A4&o){
    o.call6C60(0u,x,y,0x14u,7u); x+=4u;
    std::uint32_t ebx=0;
    for(;ebx<6u;++ebx,x+=0x0Au){
        if(ebx==o.readDword(0x7124u)){o.writeByte(0x5A00u,0x5Au);o.call7DF4(0x59F8u);}
        o.call004C(0x55F8u,ebx+0x2Fu);
        o.call7E44(x,y+1u,0x55F8u,0u);
        const std::uint32_t b=base(o), a=add32(b,ebx*2u);
        if(ebx==0u && o.readByte(add32(b,0x6836u))!=0u){
            const std::uint32_t pct=o.readByte(add32(b,0x6836u));
            const std::uint32_t rem=static_cast<std::uint32_t>(static_cast<std::int32_t>(pct)%100);
            o.callAFEE4(0x55F8u,0x54u,o.readByte(add32(a,0x682Au)),rem);
        }else o.callAFEE3(0x55F8u,0x4Cu,o.readByte(add32(a,0x682Au)));
        o.call7E04(x,y+0x2Fu,0x55F8u,0u);
        if(ebx==o.readDword(0x7124u)){o.writeByte(0x5A00u,8u);o.call7DF4(0u);}
    }

    x+=0x0Au;
    o.call004C(0x55F8u,ebx+0x2Fu);
    o.call7E44(x,y+1u,0x55F8u,0u);
    o.call7E04(x,y+0x2Fu,o.call88D8(u32(sx16(o.readWord(add32(base(o),0x6810u))))),0u);

    x+=0x0Au;
    const std::uint32_t label36=ebx+0x30u; ++ebx;
    o.call004C(0x55F8u,label36);
    o.call7E44(x,y+1u,0x55F8u,0u);
    {
        const std::uint32_t b=base(o);
        const std::uint32_t idx=o.readByte(add32(b,0x682Cu));
        const std::int32_t mod=sx8(o.readByte(add32(0x083Fu,idx)));
        const std::int32_t f=sx8(o.readByte(add32(b,0x6813u)));
        o.call7E04(x,y+0x2Fu,o.call88D8(u32(f)+u32(mod)),0u);
    }

    x+=0x0Au;
    const std::uint32_t label37=ebx+0x30u; ebx+=2u;
    o.call004C(0x55F8u,label37);
    if(sx8(o.readByte(add32(base(o),0x680Eu)))!=-1)o.call004C(0x55F8u,ebx+0x2Fu);
    o.call7E44(x,y+1u,0x55F8u,0u);
    o.call72A0(0x55F8u,o.call88D8(u32(sx16(o.readWord(add32(base(o),0x6818u))))));
    if(sx8(o.readByte(add32(base(o),0x680Eu)))!=-1){
        o.callB01A(0x55F8u,0x60u);
        o.callB01A(0x55F8u,o.call88D8(u32(sx16(o.readWord(add32(base(o),0x681Au))))));
    }
    if(sx8(o.readByte(add32(base(o),0x680Fu)))!=-1){
        o.callB01A(0x55F8u,0x60u);
        o.callB01A(0x55F8u,o.call88D8(u32(sx16(o.readWord(add32(base(o),0x681Cu))))));
    }
    o.call7E04(x,y+0x2Fu,0x55F8u,0u);

    x+=0x0Au; ++ebx;
    if(o.readDword(0x7124u)==0x0Au){o.writeByte(0x5A00u,0x5Au);o.call7DF4(0x59F8u);}
    ebx+=0x2Fu;
    o.call004C(0x55F8u,ebx);
    o.call7E44(x,y+1u,0x55F8u,0u);
    o.call7E04(x,y+0x2Fu,o.call88D8(u32(sx16(o.readWord(add32(base(o),0x6816u))))),0u);
    if(o.readDword(0x7124u)==0x0Au){o.writeByte(0x5A00u,8u);o.call7DF4(0u);}
}

} // namespace ravenloft::game
