#pragma once
#include "games/ravenloft/data/TrigTable.h"
#include <cstdint>
#include <vector>

namespace ravenloft::game {
struct Vec2Fixed { std::int32_t x=0,y=0; double xd()const{return x/65536.0;} double yd()const{return y/65536.0;} };
struct WorldObject { std::uint16_t objectId=0; Vec2Fixed position{}; std::uint16_t spawnParameter=0; };
struct PartyState { std::uint32_t level=0; Vec2Fixed position{}; int headingDegrees=0; std::vector<std::uint16_t> ownedObjects; };
struct MistTransitionResult { bool triggered=false; std::uint32_t destinationLevel=0; Vec2Fixed destination{}; int destinationHeading=0; };

class OpeningSequence {
public:
    explicit OpeningSequence(const data::TrigTable& trig):trig_(trig){}
    std::vector<WorldObject> onNpcDeath(std::uint16_t npcType,Vec2Fixed npcPosition,int currentHeading) const;
    MistTransitionResult onObjectAcquired(PartyState& party,std::uint16_t objectId) const;
    static bool hasObject(const PartyState& party,std::uint16_t objectId);
private:
    const data::TrigTable& trig_;
    WorldObject spawnDrop(std::uint16_t spawnParameter,std::uint16_t objectId,Vec2Fixed origin,int heading) const;
};
}
