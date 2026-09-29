#include "games/ravenloft/game/PlayerAttackTimers1E4A0.h"

namespace ravenloft::game {
namespace {
std::uint16_t u16(const std::array<std::uint8_t,0x248>& r,std::size_t o){
    return static_cast<std::uint16_t>(r[o])|(static_cast<std::uint16_t>(r[o+1])<<8u);
}
std::uint32_t u32(const std::array<std::uint8_t,0x248>& r,std::size_t o){
    return static_cast<std::uint32_t>(r[o])|(static_cast<std::uint32_t>(r[o+1])<<8u)|
           (static_cast<std::uint32_t>(r[o+2])<<16u)|(static_cast<std::uint32_t>(r[o+3])<<24u);
}
void zero32(std::array<std::uint8_t,0x248>& r,std::size_t o){r[o]=r[o+1]=r[o+2]=r[o+3]=0;}
}

void PlayerAttackTimers1E4A0::tick(std::array<data::CharacterRecord,4>& party,
                                    std::uint32_t currentTick,
                                    std::array<std::uint8_t,4>* dirtyFlags){
    for(std::size_t ci=0;ci<party.size();++ci){
        auto& c=party[ci];
        if((u16(c.raw,0x10)&0x8000u)!=0)continue;
        for(std::size_t o=0x10B;o<0x113;o+=4){
            const auto deadline=u32(c.raw,o);
            if(deadline!=0 && currentTick>=deadline){
                zero32(c.raw,o);if(dirtyFlags)(*dirtyFlags)[ci]=1;
            }
        }
        for(std::size_t o=0x19E;o<0x1AA;o+=4){
            const auto deadline=u32(c.raw,o);
            if(deadline!=0 && currentTick>=deadline){
                zero32(c.raw,o);if(dirtyFlags)(*dirtyFlags)[ci]=1;
            }
        }
    }
}

} // namespace ravenloft::game
