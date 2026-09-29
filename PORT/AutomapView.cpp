#include "games/ravenloft/game/AutomapView.h"
#include <algorithm>
#include <stdexcept>

namespace ravenloft::game {

AutomapView::AutomapView(const AutomapState& map,const PartyState& party)
    :width_(map.width()),height_(map.height()),glyphs_(AutomapGlyphSelector::buildGrid(map)){
    if(width_==0||height_==0) throw std::runtime_error("Automap dimensions are zero");
    const auto px=party.position.x>>16;
    const auto py=party.position.y>>16;
    partyCellX_=static_cast<std::size_t>(std::clamp<std::int32_t>(px,0,std::int32_t(width_)-1));
    partyCellY_=static_cast<std::size_t>(std::clamp<std::int32_t>(py,0,std::int32_t(height_)-1));

    for(std::size_t y=0;y<height_;++y){
        for(std::size_t x=0;x<width_;++x){
            if(map.cell(x,y)==0xFFu) continue;
            if(!bounds_.valid){ bounds_={x,y,x,y,true}; }
            else {
                bounds_.minX=std::min(bounds_.minX,x); bounds_.maxX=std::max(bounds_.maxX,x);
                bounds_.minY=std::min(bounds_.minY,y); bounds_.maxY=std::max(bounds_.maxY,y);
            }
        }
    }
}

std::uint8_t AutomapView::glyph(std::size_t x,std::size_t y) const{
    if(x>=width_||y>=height_) throw std::out_of_range("Automap glyph");
    return glyphs_[y*width_+x];
}

} // namespace ravenloft::game
