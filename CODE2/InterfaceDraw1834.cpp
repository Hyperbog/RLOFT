#include "InterfaceDraw1834.h"

namespace ravenloft::game {
namespace {
std::int32_t scale4(std::uint32_t v) noexcept { return static_cast<std::int32_t>((v*4u)>>15u); }
std::int32_t scale2(std::uint32_t v) noexcept { return static_cast<std::int32_t>((v*2u)>>15u); }
}

void InterfaceDraw1834::run(InterfaceDrawState1834& s,
                            const InterfaceDrawOps1834& o) {
    o.call291B0(s.valueC050);

    for(std::int32_t x=0;x!=0x120;x+=0x60) {
        o.call2975C(0,0,x,o.contextAt258DC(0x3E),0);
        o.call2975C(0,0xC8,x,o.contextAt258DC(0x3E),0);
    }

    s.value12748=o.call30700();
    o.call290D0(static_cast<std::uint16_t>(s.value12748));

    for(std::int32_t y=0xF0,aux=0;y<0x140;y+=8) {
        for(std::int32_t x=aux;x<0x190;x+=0x10) {
            const std::uint32_t v=o.call30CA2(x,y,o.contextAt258DC(0x35),aux);
            o.call2975C(scale4(v),x,y,o.contextAt258DC(0x35),aux);
        }
    }

    std::int32_t y=0x1A;
    std::int32_t tag=0xE2;
    for(std::int32_t i=0;i<13;++i,++tag) {
        if(i==9 && s.value258A8!=0xFFu) continue;
        o.call1C368(s.scratch769C,tag);
        o.call2975C(s.table5FCC[static_cast<std::size_t>(i)],y,0xF6,
                    o.contextAt258DC(0x34),0);
        o.call3060C(y,0xFF,s.scratch769C,1);
        y+=0x0A;
    }

    s.value12750=o.call30700();
    o.call290D0(static_cast<std::uint16_t>(s.value12750));

    for(std::int32_t x=8;x<0x138;x+=8) {
        std::uint32_t v=o.call30CA2(0,x,o.contextAt258DC(0x35),0);
        o.call2975C(scale2(v)+9,0,x,o.contextAt258DC(0x35),0);
        v=o.call30CA2(0x180,x,o.contextAt258DC(0x35),0);
        o.call2975C(scale2(v)+5,0x180,x,o.contextAt258DC(0x35),0);
    }

    for(std::int32_t x=0x10,off=0x35;x<0x180;x+=0x10) {
        std::uint32_t v=o.call30CA2(x,0,o.contextAt258DC(off),0);
        o.call2975C(scale2(v)+0x0C,x,0,o.contextAt258DC(off),0);
        v=o.call30CA2(x,0xF0,o.contextAt258DC(off),0);
        o.call2975C(scale2(v)+0x12,x,0xF0,o.contextAt258DC(off),0);
        v=o.call30CA2(x,0x138,o.contextAt258DC(off),0);
        o.call2975C(scale2(v)+0x0E,x,0x138,o.contextAt258DC(off),0);
    }

    o.call2975C(8,0,0,o.contextAt258DC(0x35),0);
    o.call2975C(0x0B,0,0x138,o.contextAt258DC(0x35),0);
    o.call2975C(4,0x180,0,o.contextAt258DC(0x35),0);
    o.call2975C(7,0x180,0x138,o.contextAt258DC(0x35),0);
    o.call2975C(0x10,0,0xF0,o.contextAt258DC(0x35),0);
    o.call2975C(0x11,0x180,0xF0,o.contextAt258DC(0x35),0);

    o.call2975C(0,0x11D,0xF0,o.contextAt258DC(0x37),0);
    o.call2975C(1,0x11D,0x130,o.contextAt258DC(0x37),0);
    o.call2975C(s.value5FD9,0x11A,0x100,o.contextAt258DC(0x38),0);

    for(std::int32_t code=2,x=0x136;code!=8;code+=2,x+=0x1C)
        o.call2975C(code,x,0x100,o.contextAt258DC(0x38),0);

    o.call2975C(0,3,0xF3,o.contextAt258DC(0x39),0);
    o.call2975C(0,0xAE,0x120,o.contextAt258DC(0x3A),0);
    o.call2975C(0,0xA8,0xF8,o.contextAt258DC(0x3B),0);
    o.call2975C(0,0xF3,0xF5,o.contextAt258DC(0x3F),0);
}

} // namespace ravenloft::game
