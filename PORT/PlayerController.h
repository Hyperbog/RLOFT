#pragma once
#include "games/ravenloft/data/TrigTable.h"
#include "games/ravenloft/game/CollisionSystem.h"
#include "games/ravenloft/game/OpeningSequence.h"
#include <cstdint>

namespace ravenloft::game {

// Faithful opening-demo player locomotion recovered from CODE.2 0x2E120 and
// 0x2DBAC. Input values are the original post-input-filter axes (DS:C6EC turn,
// DS:C6E8 forward/back, DS:C6F0 strafe), not modern key states.
class PlayerController {
public:
    PlayerController(const data::TrigTable& trig,const CollisionSystem& collision)
        :trig_(trig),collision_(collision){}

    struct StepResult {
        bool headingChanged=false;
        bool positionChanged=false;
        bool collided=false;
        std::int32_t attemptedDeltaX=0;
        std::int32_t attemptedDeltaY=0;
    };

    StepResult tickOriginalAxes(PartyState& party,
                                std::int32_t turnAxis,
                                std::int32_t forwardBackAxis,
                                std::int32_t strafeAxis);

    std::int32_t turnAccumulator() const { return turnAccumulator_; }

private:
    const data::TrigTable& trig_;
    const CollisionSystem& collision_;
    std::int32_t turnAccumulator_=0; // CODE.2 DS:C6E4 remainder accumulator

    static int normalizeHeading(int h);
    Vec2Fixed movementVector(std::int32_t magnitude,int heading) const;
    bool tryMoveWithSlide(PartyState& party,std::int32_t dx,std::int32_t dy) const;
};

} // namespace ravenloft::game
