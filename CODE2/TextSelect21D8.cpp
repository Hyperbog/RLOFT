#include "TextSelect21D8.h"

namespace ravenloft::game {
namespace {
std::int16_t s16(std::span<std::uint8_t> s,std::size_t p) noexcept {
 return static_cast<std::int16_t>(static_cast<std::uint16_t>(s[p])|
   (static_cast<std::uint16_t>(s[p+1u])<<8u));
}
}

void TextSelect21D8::run(TextSelectState21D8& s,const TextSelectOps21D8& o) {
    while(o.call1A1F4()!=0) {}
    for(;;) {
        while(o.call1A1F4()==0) {}
        const std::int32_t mx=s.mouseX9E31, my=s.mouseY9E33;

        if(mx>=s.exitLeft5F8A && mx<s.exitRight5F8E &&
           my>=s.exitTop5F8C && my<s.exitBottom5F90) {
            o.call22D70();
            return;
        }
        if(mx<0x20 || mx>0x16F || my<0x10 || my>0xE3) continue;

        const std::int32_t worldX=mx+s.viewX12744*8;
        const std::int32_t worldY=my+s.viewY12740*5;

        std::size_t p=0, selected=static_cast<std::size_t>(-1);
        while(s16(s.records5760,p+4u)!=0) {
            const std::size_t text=p+6u;
            const std::int32_t x=s16(s.records5760,p);
            const std::int32_t y=s16(s.records5760,p+2u);
            const std::int32_t count=s16(s.records5760,p+4u);
            const std::int32_t extent=o.call302CC(
                s.records5760.data()+text,1,count);

            if(x<worldX && x+0x0B>worldX &&
               y<worldY && y+extent>worldY) selected=p;

            p=text+static_cast<std::size_t>(count);
        }
        if(selected==static_cast<std::size_t>(-1)) continue;

        const std::size_t count=static_cast<std::size_t>(s16(s.records5760,selected+4u));
        const std::size_t end=o.call1C08();
        const std::size_t after=selected+6u+count;
        const std::ptrdiff_t move=static_cast<std::ptrdiff_t>(end)-static_cast<std::ptrdiff_t>(after);
        o.call30CD4(selected,after,move);
        o.call30D30(selected+static_cast<std::size_t>(move),0,count+6u);

        o.call139C(s.viewX12744,s.viewY12740,0x30,0x2F,1);
        o.call1A510(s.valueC050);
        s.valueC050^=1u;
    }
}

} // namespace ravenloft::game
