#include "games/ravenloft/game/NpcOverlapSystem.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
bool inWrappedInclusive(std::int32_t value, std::int32_t center, std::uint32_t radius) {
    const auto v=static_cast<std::uint32_t>(value);
    const auto c=static_cast<std::uint32_t>(center);
    return v >= c-radius && v <= c+radius;
}
}

bool NpcOverlapSystem::overlaps(std::int32_t candidateYFixed,
                                std::int32_t candidateXFixed,
                                int moverId) const {
    for(std::size_t i=0;i<work_.npcs.size();++i){
        if(static_cast<int>(i)==moverId) continue;
        const auto& p=work_.npcs[i];
        // Runtime byte +0x0B bit 0x02 (state bit 25) excludes the actor.
        if((p.state & 0x02000000u)!=0) continue;
        if(p.type>=npcs_.count()) throw std::runtime_error("Runtime NPC type outside NPCS.DAT");
        const auto radius=static_cast<std::uint32_t>(npcs_.at(p.type).collisionHalfExtent)+0x3E8u;
        if(inWrappedInclusive(candidateXFixed,p.xFixed,radius) &&
           inWrappedInclusive(candidateYFixed,p.yFixed,radius)) return true;
    }
    return false;
}

} // namespace ravenloft::game
