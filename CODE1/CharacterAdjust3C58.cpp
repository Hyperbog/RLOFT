#include "CharacterAdjust3C58.h"

#include <bit>
#include <cstdint>

namespace ravenloft::game {
namespace {
constexpr std::uint32_t add32(std::uint32_t a,std::uint32_t b) noexcept{return a+b;}
constexpr std::uint32_t mul32(std::uint32_t a,std::uint32_t b) noexcept{return a*b;}
constexpr std::int32_t s32(std::uint32_t v) noexcept{return std::bit_cast<std::int32_t>(v);}
constexpr std::uint32_t u32(std::int32_t v) noexcept{return std::bit_cast<std::uint32_t>(v);}
constexpr std::int32_t sx16(std::uint16_t v) noexcept{return static_cast<std::int16_t>(v);}
constexpr std::uint32_t base(const CharacterAdjustOps3C58&o) noexcept{return mul32(o.readDword(0x4CCu),0x248u);}
void redraw(const CharacterAdjustOps3C58&o){
    o.call6588(o.readByte(0x7138u));
    CharacterSheet38A4::draw(0x5Bu,0x88u,o.sheet);
    o.call55DC(o.readDword(0x7138u),0x5Bu,0x88u,0x7Au,0x30u);
    while(o.call53B4()!=0u){}
}
}

void CharacterAdjust3C58::run(const CharacterAdjustOps3C58&o){
    o.writeDword(0x924u,o.readDword(0x928u));
    o.writeDword(0x92Cu,o.readDword(0x930u));
    o.writeDword(0x934u,o.readDword(0x938u));
    o.writeDword(0x928u,0x2Cu); o.writeDword(0x938u,0x2Du); o.writeDword(0x940u,3u);
    o.call07E4(6u,9u); o.call56DC();
    o.writeDword(0x7124u,0xFFFFFFFFu);

    bool finished=false;
    while(!finished){
        const std::uint32_t selection=o.call043C();
        if(selection==0u){finished=true;continue;}
        const std::uint32_t row=o.readDword(0x7124u);
        const std::uint32_t rowAddr=add32(base(o),row*2u);

        if(selection==7u){
            if(row==0xFFFFFFFFu)continue;
            if(s32(row)<6){
                if(row==0u){
                    if((o.readByte(0x4D4u)&1u)!=0u){
                        if(o.readByte(add32(rowAddr,0x682Au))==0x12u){
                            const std::uint8_t c=o.readByte(add32(base(o),0x680Du));
                            if(c==1u||c==2u||c==0u){const auto a=add32(base(o),0x6836u);o.writeByte(a,static_cast<std::uint8_t>(o.readByte(a)+1u));}
                        }else{const auto a=add32(rowAddr,0x682Au);o.writeByte(a,static_cast<std::uint8_t>(o.readByte(a)+1u));}
                    }else{
                        o.writeByte(add32(base(o),0x682Au+row*2u),0x14u);
                        o.writeByte(add32(base(o),0x6836u),0x64u);
                    }
                }else if((o.readByte(0x4D4u)&1u)!=0u){const auto a=add32(rowAddr,0x682Au);o.writeByte(a,static_cast<std::uint8_t>(o.readByte(a)+1u));}
                else o.writeByte(add32(rowAddr,0x682Au),0x14u);
            }else if(row==0x0Au){
                const auto a=add32(base(o),0x6816u);
                if((o.readByte(0x4D4u)&1u)!=0u)o.writeWord(a,static_cast<std::uint16_t>(o.readWord(a)+1u));
                else o.writeWord(a,0x03E8u);
            }
            CharacterNormalize3404::apply(o.normalize); redraw(o); continue;
        }

        if(selection==8u){
            const std::int32_t dx=sx16(o.readWord(0x73E6u))-0x60;
            const std::int32_t r=dx/10;
            o.writeDword(0x7124u,((r>=0&&r<6)||r==10)?u32(r):0xFFFFFFFFu);
            redraw(o); continue;
        }

        if(selection==9u){
            if(row==0xFFFFFFFFu)continue;
            if(s32(row)<6){
                if(row==0u){
                    if((o.readByte(0x4D4u)&1u)!=0u){
                        const auto value=add32(rowAddr,0x682Au),extra=add32(base(o),0x6836u);
                        if(o.readByte(value)==0x12u && o.readByte(extra)!=1u)o.writeByte(extra,static_cast<std::uint8_t>(o.readByte(extra)-1u));
                        else o.writeByte(value,static_cast<std::uint8_t>(o.readByte(value)-1u));
                    }else{o.writeByte(add32(rowAddr,0x682Au),0u);o.writeByte(add32(base(o),0x6836u),0u);}
                }else if((o.readByte(0x4D4u)&1u)!=0u){const auto a=add32(rowAddr,0x682Au);o.writeByte(a,static_cast<std::uint8_t>(o.readByte(a)-1u));}
                else o.writeByte(add32(rowAddr,0x682Au),0u);
            }else if(row==0x0Au){
                const auto a=add32(base(o),0x6816u);
                if((o.readByte(0x4D4u)&1u)!=0u)o.writeWord(a,static_cast<std::uint16_t>(o.readWord(a)-1u));
                else o.writeWord(a,0u);
            }
            CharacterNormalize3404::apply(o.normalize); redraw(o);
        }
    }

    o.writeDword(0x7124u,0xFFFFFFFFu);
    o.call6588(o.readByte(0x7138u));
    const std::uint32_t saved=o.call7868(0u,0u,0x7Au,0x30u,0xFFu);
    o.call9EB0(0u,0u,0x7Au,0x30u,0xFEu);
    CharacterSheet38A4::draw(0u,0u,o.sheet);
    if(o.readDword(0x954u)!=0u){o.call7830(o.readDword(0x954u));o.writeDword(0x954u,0u);}
    if(o.readDword(0x958u)!=0u){o.call7830(o.readDword(0x958u));o.writeDword(0x958u,0u);}
    o.writeDword(0x958u,o.call7868(0u,0u,0x7Au,0x30u,0xFFu));
    o.writeDword(0x954u,o.call7868(0u,0u,0x7Au,0x30u,0xFFu));
    o.call67C8(0u,0u,0u,saved); o.call7830(saved);

    o.writeDword(0x924u,o.readDword(0x928u));
    o.writeDword(0x92Cu,o.readDword(0x930u));
    o.writeDword(0x934u,o.readDword(0x938u));
    o.writeDword(0x928u,0x16u);o.writeDword(0x938u,0x2Eu);o.writeDword(0x940u,0x0Eu);
    o.call07E4(6u,9u);
}

} // namespace ravenloft::game
