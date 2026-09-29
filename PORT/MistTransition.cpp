#include "games/ravenloft/game/MistTransition.h"
#include <algorithm>
#include <stdexcept>

namespace ravenloft::game {
namespace {
constexpr int XTarget = 23;
constexpr int YTarget = 7;
constexpr int FinalHeading = 0xD5; // 213 degrees, CODE.2 0x2222D.
int wrap360(int h){ h%=360; if(h<0)h+=360; return h; }
}

MistTransition::MistTransition(PartyState& party, OriginalWorldClock1E75C& worldClock)
    : party_(party), worldClock_(worldClock) {
    if (party_.level != 22) throw std::runtime_error("Mist transition must begin in level 22");
    const int x = integerPart(party_.position.x);
    const int y = integerPart(party_.position.y);
    xTargetHeading_ = x > XTarget ? 90 : 270;   // CODE.2 0x21F9D / 0x2204A.
    yTargetHeading_ = y > YTarget ? 180 : 0;    // CODE.2 0x220FB / 0x221A7.
    party_.headingDegrees = wrap360(party_.headingDegrees);
}

int MistTransition::integerPart(std::int32_t fixed){ return fixed >> 16; }
std::int32_t MistTransition::withIntegerHalf(int integer){ return integer*65536 + 0x8000; }

bool MistTransition::stepTurnX(int& h,int target){
    h=wrap360(h);
    if(h==target) return true;
    if(target==90){
        if(h>=85 && h<=95){ h=90; return true; }
        if(h>90 && h<270) h-=10;
        else if(h<=80) h+=10;
        else { h=90; return true; }
    }else{ // target 270: exact branch structure at 0x2204F..0x220C4.
        if(h>=265 && h<=275){ h=270; return true; }
        if(h>270 || h<90){
            if(h>=270) h-=10;
            else h=275;
        }else{
            if(h>265) h=275;
            else h+=10;
        }
        h=wrap360(h);
    }
    return h==target;
}

bool MistTransition::stepTurnY(int& h,int target){
    h=wrap360(h);
    if(h==target) return true;
    if(target==180){
        if(h>=175 && h<=185){ h=180; return true; }
        if(h>180) h-=10;
        else if(h<=170) h+=10;
        else { h=180; return true; }
    }else{ // target 0: original deliberately counts downward through 270..0.
        if(h==0){ return true; }
        if(h>=5 && h<=355){
            if(h>=10) h-=10;
            else { h=0; return true; }
        }else { h=0; return true; }
        h=wrap360(h);
    }
    return h==target;
}

bool MistTransition::stepTurnFinal(int& h){
    h=wrap360(h);
    if(h==FinalHeading) return true;
    if(h>=210 && h<=220){ h=FinalHeading; return true; }
    if(h>=180 && h<FinalHeading){
        h+=10;
        if(h>=210){ h=FinalHeading; return true; }
    }else{
        h=wrap360(h-10);
        if(h<=220){ h=FinalHeading; return true; }
    }
    return h==FinalHeading;
}

void MistTransition::loadSvalichWoods(){
    // CODE.2 0x22366..0x2239C.
    party_.position={withIntegerHalf(9),withIntegerHalf(58)};
    party_.headingDegrees=180;
    party_.level=23;
    levelLoaded_=true;
}
void MistTransition::finalizeInventory(){
    // CODE.2 0x223B5..0x223D6. Parchment 233 deliberately remains.
    for(const std::uint16_t id:{std::uint16_t(135),std::uint16_t(136),std::uint16_t(116)})
        party_.ownedObjects.erase(std::remove(party_.ownedObjects.begin(),party_.ownedObjects.end(),id),party_.ownedObjects.end());
}

bool MistTransition::tick(){
    // Internal phase changes which do not correspond to a rendered iteration are
    // collapsed in the same call; visible position/heading changes return promptly.
    for(;;){
        switch(phase_){
        case Phase::TurnForX:
            if(stepTurnX(party_.headingDegrees,xTargetHeading_)) phase_=Phase::MoveX;
            return false;
        case Phase::MoveX:{
            int x=integerPart(party_.position.x);
            party_.position.x=withIntegerHalf(x);
            if(x==XTarget){ phase_=Phase::TurnForY; continue; }
            x += (x>XTarget ? -1 : 1);
            party_.position.x=withIntegerHalf(x);
            return false;
        }
        case Phase::TurnForY:
            if(stepTurnY(party_.headingDegrees,yTargetHeading_)) phase_=Phase::MoveY;
            return false;
        case Phase::MoveY:{
            int y=integerPart(party_.position.y);
            party_.position.y=withIntegerHalf(y);
            if(y==YTarget){ phase_=Phase::TurnIntoMist; continue; }
            y += (y>YTarget ? -1 : 1);
            party_.position.y=withIntegerHalf(y);
            return false;
        }
        case Phase::TurnIntoMist:
            if(stepTurnFinal(party_.headingDegrees)) phase_=Phase::StartMistEffect;
            return false;
        case Phase::StartMistEffect:
            // CODE.2 0x222A6 invokes the original effect constructor with the fixed
            // parameter tuple. Effects rendering will consume this phase later.
            phase_=Phase::WaitMistEffect;
            return false;
        case Phase::WaitMistEffect:
            // CODE.2 0x222C0..0x222E5: compare CF4C high word with 2. If
            // unequal, increment CF4E, normalize through 0x31F94, then run
            // one display/update iteration before checking again.
            if(!worldClock_.mistWaitStep222C0()) return false;
            phase_=Phase::TransferFrames;
            transferFrame_=0;
            continue;
        case Phase::TransferFrames:
            // The original runs 22 display iterations (ESI 0..21) and changes level
            // on iteration 20 before the final two frames complete.
            if(transferFrame_==20 && !levelLoaded_) loadSvalichWoods();
            ++transferFrame_;
            if(transferFrame_>=22){
                finalizeInventory();
                phase_=Phase::Complete;
                return true;
            }
            return false;
        case Phase::Complete:
            return true;
        }
    }
}

} // namespace ravenloft::game
