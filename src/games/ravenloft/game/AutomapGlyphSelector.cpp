#include "games/ravenloft/game/AutomapGlyphSelector.h"
#include <array>

namespace ravenloft::game {
namespace {
constexpr std::array<std::uint8_t,16> kConnectivityGlyph{{
    0x06,0x0F,0x0E,0x05,0x0C,0x04,0x00,0x0A,
    0x0D,0x01,0x03,0x09,0x02,0x07,0x08,0x0B
}};
}

std::uint8_t AutomapGlyphSelector::select(const AutomapState& map,std::size_t x,std::size_t y){
    const auto current=map.cell(x,y);
    if(current==0xFFu) return 0xFFu;
    if(current>=0x20u) return current;

    std::uint8_t ordinaryMask=0;
    std::uint8_t specialMask=0;
    const auto add=[&](std::uint8_t v,std::uint8_t bit){
        if(ordinaryNeighbor(v)) ordinaryMask=static_cast<std::uint8_t>(ordinaryMask|bit);
        if(specialNeighbor(v)) specialMask=static_cast<std::uint8_t>(specialMask|bit);
    };

    // CODE.2 bit assignment: west=1, north=2, south=4, east=8.
    if(x>0) add(map.cell(x-1,y),0x01);
    if(x+1<map.width()) add(map.cell(x+1,y),0x08);
    if(y>0) add(map.cell(x,y-1),0x02);
    if(y+1<map.height()) add(map.cell(x,y+1),0x04);

    switch(current){
        case 1:
            return kConnectivityGlyph[ordinaryMask];
        case 2:
            if(ordinaryMask==0x06) return 0x29;
            if(ordinaryMask==0x09) return 0x2B;
            return kConnectivityGlyph[ordinaryMask];
        case 3:
            if(specialMask==0x01) return 0x21;
            if(specialMask==0x02) return 0x23;
            if(specialMask==0x04) return 0x22;
            if(specialMask==0x08) return 0x20;
            if((specialMask&0x09)!=0) return 0x20;
            if((specialMask&0x06)!=0) return 0x22;
            return kConnectivityGlyph[ordinaryMask];
        case 4:
            if(ordinaryMask==0x06) return 0x28;
            if(ordinaryMask==0x09) return 0x2A;
            return kConnectivityGlyph[ordinaryMask];
        case 5:
            if((specialMask&0x06)==0x06) return 0x33;
            if((specialMask&0x09)==0x09) return 0x33;
            return kConnectivityGlyph[ordinaryMask];
        default:
            // This is the executable fall-through for sub-0x20 values outside
            // the five explicit semantic classes.
            return ordinaryMask;
    }
}

std::vector<std::uint8_t> AutomapGlyphSelector::buildGrid(const AutomapState& map){
    std::vector<std::uint8_t> out;
    out.reserve(std::size_t(map.width())*map.height());
    for(std::size_t y=0;y<map.height();++y)
        for(std::size_t x=0;x<map.width();++x)
            out.push_back(select(map,x,y));
    return out;
}

} // namespace ravenloft::game
