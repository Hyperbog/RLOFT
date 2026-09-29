#pragma once
#include "games/ravenloft/data/InitDatabase.h"
#include "games/ravenloft/data/WorkLevel.h"
#include <cstdint>

namespace ravenloft::game {

// Opening-demo collision primitives recovered from CODE.2.  This currently
// implements the scenery overlap path at 0x67BAC exactly for ordinary scenery
// types (including the level-22 tree types 62..64).  Other world/NPC collision
// paths remain separate so we do not blend partially understood rules together.
class CollisionSystem {
public:
    CollisionSystem(const data::WorkLevelSnapshot& work,const data::InitDatabase& init)
        :work_(work),init_(init){}

    bool blocksByScenery(std::int32_t xFixed,std::int32_t yFixed) const;

private:
    const data::WorkLevelSnapshot& work_;
    const data::InitDatabase& init_;
};

} // namespace ravenloft::game
