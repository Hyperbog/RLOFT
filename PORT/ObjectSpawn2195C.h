#pragma once

#include "games/ravenloft/data/TrigTable.h"
#include "games/ravenloft/data/WorkLevel.h"
#include <cstdint>
#include <functional>
#include <vector>

namespace ravenloft::game {

// External state/call boundaries used by CODE.2 0x2195C.  The opening demo
// supplies the already-converted 0x2D5D0 collision routine and leaves the two
// unrelated object-system callbacks explicit.
struct ObjectSpawnContext2195C {
    const data::TrigTable* trig = nullptr;
    std::uint16_t levelWidth25619 = 0;
    std::int32_t facingDegreesC658 = 0;
    bool gridSnapC02F = false;

    // CODE.2 0x2D5D0(candidateY,candidateX,0x190). Any nonzero return accepts
    // the scattered point; zero falls back to the supplied source position.
    std::function<int(std::int32_t,std::int32_t,int)> collision2D5D0;

    // CODE.2 0x1A344(-1,0), called only when argument 7 is nonzero.
    std::function<void(int,int)> boundary1A344;
    // CODE.2 0x21880, called only for object id 0x89.
    std::function<void()> boundary21880;
};

struct ObjectSpawnResult2195C {
    std::size_t index = 0;
    bool scattered = false;
    bool collisionFallback = false;
};

// Direct reconstruction of CODE.2 0x2195C..0x21ADA.  The seven parameters
// retain their literal original order because only arguments 1/2/3/6 have
// complete semantic names at this stage.
class ObjectSpawn2195C {
public:
    static ObjectSpawnResult2195C execute(
        int scatterParameter,
        std::uint16_t objectId,
        int placementFlagSource,
        std::int32_t sourceXFixed,
        std::int32_t sourceYFixed,
        int scatterFromFacing,
        int invokeBoundary1A344,
        std::vector<data::RuntimePlacement22>& runtimeObjects,
        const ObjectSpawnContext2195C& context);
};

} // namespace ravenloft::game
