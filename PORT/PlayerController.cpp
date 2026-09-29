#include "games/ravenloft/game/PlayerController.h"
#include <cstdlib>

namespace ravenloft::game {

int PlayerController::normalizeHeading(int h){ h%=360; if(h<0)h+=360; return h; }

Vec2Fixed PlayerController::movementVector(std::int32_t magnitude,int heading) const {
    // CODE.2 0x2DF2C: X = cos(angle)*magnitude; Y = -sin(angle)*magnitude.
    return {trig_.cosMul16_16(magnitude,heading),-trig_.sinMul16_16(magnitude,heading)};
}

bool PlayerController::tryMoveWithSlide(PartyState& party,std::int32_t dx,std::int32_t dy) const {
    // CODE.2 0x2DBAC: full candidate, first axis only, second axis only, restore.
    const auto old=party.position;
    auto blocked=[this](std::int32_t x,std::int32_t y){return collision_.blocksByScenery(x,y);};

    if(!blocked(old.x+dx,old.y+dy)){
        party.position={old.x+dx,old.y+dy};
        return true;
    }
    if(dx!=0 && !blocked(old.x+dx,old.y)){
        party.position={old.x+dx,old.y};
        return true;
    }
    if(dy!=0 && !blocked(old.x,old.y+dy)){
        party.position={old.x,old.y+dy};
        return true;
    }
    party.position=old;
    return false;
}

PlayerController::StepResult PlayerController::tickOriginalAxes(PartyState& party,
                                                                 std::int32_t turnAxis,
                                                                 std::int32_t forwardBackAxis,
                                                                 std::int32_t strafeAxis){
    StepResult out;

    // CODE.2 0x2E12C..0x2E195: accumulate sub-degree input in units of 1/20 degree,
    // consume whole-degree steps with IDIV truncation toward zero, and wrap 0..359.
    turnAccumulator_ += turnAxis;
    const auto turnDegrees=turnAccumulator_/20;
    turnAccumulator_ -= turnDegrees*20;
    if(turnDegrees!=0){
        party.headingDegrees=normalizeHeading(party.headingDegrees-turnDegrees);
        out.headingChanged=true;
    }

    auto moveAxis=forwardBackAxis;
    auto moveHeading=party.headingDegrees;

    // CODE.2 0x2E1ED..0x2E239 converts the lateral axis into an equivalent
    // heading-relative move, preserving the executable's sign convention.
    if(strafeAxis!=0){
        moveAxis=std::abs(strafeAxis);
        moveHeading=normalizeHeading(moveHeading+(strafeAxis<0?-90:90));
    }

    if(moveAxis==0)return out;

    std::int32_t magnitude=0;
    if(moveAxis<0){
        // 0x2E242 path: negative axis is forward, multiplier 0x96 (150).
        magnitude=150*(-moveAxis);
    }else{
        // 0x2E2B4 path: positive axis is backward, multiplier 0x64 (100),
        // using a heading rotated by 180 degrees.
        magnitude=100*moveAxis;
        moveHeading=normalizeHeading(moveHeading+180);
    }

    const auto delta=movementVector(magnitude,moveHeading);
    out.attemptedDeltaX=delta.x;
    out.attemptedDeltaY=delta.y;
    const auto before=party.position;
    const bool accepted=tryMoveWithSlide(party,delta.x,delta.y);
    out.positionChanged=party.position.x!=before.x||party.position.y!=before.y;
    out.collided=!accepted;
    return out;
}

} // namespace ravenloft::game
