#include "games/ravenloft/game/OpeningSequence.h"
#include <algorithm>
#include <array>

namespace ravenloft::game {
namespace {
int truncDiv3(int n){ return n/3; } // C/C++ integer division truncates toward zero, matching CODE.2 idiv.
int norm360(int d){ d%=360; if(d<0)d+=360; return d; }
}
WorldObject OpeningSequence::spawnDrop(std::uint16_t p,std::uint16_t id,Vec2Fixed origin,int heading) const {
    // CODE.2 0x2195C: when arg6 is set, derive an angle from (p-160)/3,
    // generate a 1.5-world-unit vector, then add it to the source actor position.
    const int angle=norm360(heading-truncDiv3(static_cast<int>(p)-160));
    constexpr std::int32_t distance=0x18000; // 1.5 in 16.16
    Vec2Fixed pos=origin;
    pos.y += -trig_.sinMul16_16(distance,angle);
    pos.x +=  trig_.cosMul16_16(distance,angle);
    return {id,pos,p};
}
std::vector<WorldObject> OpeningSequence::onNpcDeath(std::uint16_t npcType,Vec2Fixed p,int heading) const {
    if(npcType!=9) return {};
    // Exact CODE.2 0xE3C9 special case.
    return {
        spawnDrop(0x6e,136,p,heading),
        spawnDrop(0x78,135,p,heading),
        spawnDrop(0x82,233,p,heading),
        spawnDrop(0x8c,116,p,heading)
    };
}
bool OpeningSequence::hasObject(const PartyState& party,std::uint16_t id){return std::find(party.ownedObjects.begin(),party.ownedObjects.end(),id)!=party.ownedObjects.end();}
MistTransitionResult OpeningSequence::onObjectAcquired(PartyState& party,std::uint16_t id) const {
    if(!hasObject(party,id)) party.ownedObjects.push_back(id);
    constexpr std::array<std::uint16_t,4> required{135,136,116,233};
    bool all=true; for(auto q:required) all &= hasObject(party,q);
    if(!all) return {};
    // CODE.2 0x21F70 final transition state. The three occult/holy objects are explicitly
    // removed by 0xF540; assassin parchment 233 is not removed here and remains owned.
    for(auto q:{std::uint16_t(135),std::uint16_t(136),std::uint16_t(116)})
        party.ownedObjects.erase(std::remove(party.ownedObjects.begin(),party.ownedObjects.end(),q),party.ownedObjects.end());
    party.level=23;
    party.position={static_cast<std::int32_t>(9*65536+0x8000),static_cast<std::int32_t>(58*65536+0x8000)};
    party.headingDegrees=180;
    return {true,23,party.position,180};
}
}
