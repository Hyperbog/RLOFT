#include "TextEntryLoop207C.h"

namespace ravenloft::game {

void TextEntryLoop207C::run(TextEntryLoop207CState& s,
                            const TextEntryLoop207COps& o) {
    while(o.call1A1F4()!=0) {}
    o.call1F6C(0);
    o.call291B0(s.valueC050);

    for(;;) {
        while(o.call1A1F4()==0) {}

        const std::int32_t x=s.mouseX9E31;
        const std::int32_t y=s.mouseY9E33;

        if(x>=s.boxLeft5F82 && x<s.boxRight5F86 &&
           y>=s.boxTop5F84 && y<s.boxBottom5F88) {
            o.call1F6C(1);
            o.call22D70();
            return;
        }

        if(x<8 || x>0x188 || y<4 || y>0xCE) continue;

        o.call1AACC();
        o.call2E590(s.valueEF4E);
        o.call2A020(static_cast<std::uint8_t>(s.valueEF4E^1u));

        std::int32_t columns=(0x188-x)/0x0C;
        if(columns==0) {
            o.call2E5D4();
            o.call1A288();
            continue;
        }
        if(columns>4) columns=4;

        const std::int32_t alignedY=static_cast<std::int16_t>(
            static_cast<std::uint16_t>(y)&0xFFF8u);
        const std::int32_t handle=o.call30070(
            x,alignedY,columns*0x0C,0x140-alignedY,0xFF);

        o.call2E5D4();
        o.call1CE4(x,y,columns,handle);
        o.call30040(handle);
        o.call1A288();
    }
}

} // namespace ravenloft::game
