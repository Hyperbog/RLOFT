#include "games/ravenloft/game/CollisionSystem.h"
#include <cstdint>
#include <stdexcept>

namespace ravenloft::game {
bool CollisionSystem::blocksByScenery(std::int32_t xFixed,std::int32_t yFixed) const {
    // CODE.2 0x67BAC iterates runtime scenery, reads INIT definition +2 as an
    // unsigned fixed-point half-extent, ignores values <=1, then performs an
    // inclusive AABB comparison.  In the player/world query mode (-1), all
    // ordinary types return collision=1; only types 0x54 and 0x7A branch into
    // special behavior.  Level 22 contains only types 62,63,64.
    for(const auto& p:work_.scenery){
        if(p.type>=init_.scenery().size())throw std::runtime_error("Runtime scenery type outside INIT table");
        const auto extent=static_cast<std::int32_t>(init_.scenery()[p.type].radius);
        if(extent<=1)continue;
        const std::int64_t xmin=std::int64_t(p.xFixed)-extent;
        const std::int64_t xmax=std::int64_t(p.xFixed)+extent;
        const std::int64_t ymin=std::int64_t(p.yFixed)-extent;
        const std::int64_t ymax=std::int64_t(p.yFixed)+extent;
        if(std::int64_t(xFixed)<xmin||std::int64_t(xFixed)>xmax||
           std::int64_t(yFixed)<ymin||std::int64_t(yFixed)>ymax)continue;
        if(p.type==0x54 || p.type==0x7A){
            // Their special side-effect branches are not relevant to the opening
            // forest and are deliberately not approximated here.
            continue;
        }
        return true;
    }
    return false;
}
}
