#include "games/ravenloft/game/OriginalKeyboardMovement.h"
#include <algorithm>
#include <cstdlib>

namespace ravenloft::game {

void OriginalKeyboardMovement::apply(int channel,int sign){
    auto v=std::int32_t(channels_[channel])+std::int32_t(kImpulse[channel])*sign;
    v=std::clamp(v,-std::int32_t(kMaximum[channel]),std::int32_t(kMaximum[channel]));
    channels_[channel]=static_cast<std::int16_t>(v);
}

void OriginalKeyboardMovement::decayExcept(int activeChannel){
    for(int i=0;i<6;++i){
        if(i==activeChannel) continue;
        auto v=std::int32_t(channels_[i]);
        if(v<0) v=std::min<std::int32_t>(0,v+kDecay[i]);
        else if(v>0) v=std::max<std::int32_t>(0,v-kDecay[i]);
        channels_[i]=static_cast<std::int16_t>(v);
    }
}

bool OriginalKeyboardMovement::keyEvent(std::uint16_t key,const HeldQuery& held){
    const auto isHeld=[&](std::uint8_t scan){ return held ? held(scan) : false; };
    int channel=-1,sign=0;
    switch(key){
        case 0x0147: // Home: turn-left; with Up/Down held enters combo channels.
            if(isHeld(0x48)){channel=4;sign=-1;}
            else if(isHeld(0x50)){channel=5;sign=-1;}
            else {channel=1;sign=-1;}
            break;
        case 0x0149: // PgUp: turn-right; with Up/Down held enters combo channels.
            if(isHeld(0x48)){channel=4;sign=+1;}
            else if(isHeld(0x50)){channel=5;sign=+1;}
            else {channel=1;sign=+1;}
            break;
        case 0x0148: // Up
            if(isHeld(0x47)){channel=4;sign=-1;}
            else if(isHeld(0x49)){channel=4;sign=+1;}
            else {channel=0;sign=-1;}
            break;
        case 0x0150: // Down
            if(isHeld(0x47)){channel=5;sign=-1;}
            else if(isHeld(0x49)){channel=5;sign=+1;}
            else {channel=0;sign=+1;}
            break;
        case 0x014B: // Left
        case 0x014F: // End duplicates left lateral movement in the original dispatch.
            channel=2;sign=-1;break;
        case 0x014D: // Right
        case 0x0151: // PgDn duplicates right lateral movement.
            channel=2;sign=+1;break;
        default:
            return false;
    }
    apply(channel,sign);
    decayExcept(channel);
    return true;
}

void OriginalKeyboardMovement::idleTick(){ decayExcept(-1); }

OriginalAxes OriginalKeyboardMovement::axes() const{
    auto c=channels_;
    // CODE.2 special channels 4/5 are converted into simultaneous forward+turn.
    if(c[4]!=0){ c[0]=static_cast<std::int16_t>(-std::abs(std::int32_t(c[4]))); c[1]=c[4]; }
    if(c[5]!=0){ c[0]=static_cast<std::int16_t>(-std::abs(std::int32_t(c[5]))); c[1]=c[5]; }
    OriginalAxes out;
    out.strafe=c[2];
    out.turn=(std::int32_t(c[1])*20)>>3;
    out.forwardBack=(std::int32_t(c[3]+c[0])*20)>>3;
    return out;
}

} // namespace ravenloft::game
