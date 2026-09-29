#include "games/ravenloft/game/OriginalRng30CA2.h"

namespace ravenloft::game {
std::uint16_t OriginalRng30CA2::next15(){
    state_=state_*0x41C64E6Du+0x3039u;
    return static_cast<std::uint16_t>((state_>>16u)&0x7FFFu);
}
std::uint16_t OriginalRng30CA2::seedFromBiosLowWord(std::uint32_t firstBiosTick,
                                                     const std::function<std::uint32_t()>& rereadBiosTick){
    std::uint16_t w=static_cast<std::uint16_t>(firstBiosTick);
    if(w==0 && rereadBiosTick)w=static_cast<std::uint16_t>(rereadBiosTick());
    seed(static_cast<std::uint32_t>(w));
    return w;
}
} // namespace ravenloft::game
