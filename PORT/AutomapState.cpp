#include "games/ravenloft/game/AutomapState.h"
#include <algorithm>
#include <stdexcept>

namespace ravenloft::game {
namespace {
std::uint16_t rd16(const std::vector<std::uint8_t>& b,std::size_t p){
    if(p+2>b.size())throw std::runtime_error("Automap annotation truncated");
    return std::uint16_t(b[p]) | (std::uint16_t(b[p+1])<<8);
}
}
AutomapState::AutomapState(const data::AutomapInitialState& s)
    :width_(s.width),height_(s.height),cells_(s.cells){
    if(cells_.size()!=std::size_t(width_)*height_)throw std::runtime_error("Automap cell count mismatch");
    // CODE.2 persisted note list at DS 0x5760: int16 x,y,len followed by len bytes;
    // a zero length terminates the list. LEVELS.DAT ships with no notes.
    std::size_t p=0;
    while(p<s.annotations.size()){
        if(p+6>s.annotations.size())throw std::runtime_error("Automap note header truncated");
        const auto x=static_cast<std::int16_t>(rd16(s.annotations,p));
        const auto y=static_cast<std::int16_t>(rd16(s.annotations,p+2));
        const auto n=static_cast<std::int16_t>(rd16(s.annotations,p+4)); p+=6;
        if(n==0)break;
        if(n<0||p+std::size_t(n)>s.annotations.size())throw std::runtime_error("Automap note text truncated");
        notes_.push_back({x,y,std::string(reinterpret_cast<const char*>(s.annotations.data()+p),std::size_t(n))});
        p+=std::size_t(n);
    }
}
std::uint8_t AutomapState::cell(std::size_t x,std::size_t y) const{
    if(x>=width_||y>=height_)throw std::out_of_range("Automap cell");
    return cells_[y*width_+x];
}
std::size_t AutomapState::discoveredCount() const{
    return std::count_if(cells_.begin(),cells_.end(),[](std::uint8_t v){return v!=0xFFu;});
}
} // namespace ravenloft::game
