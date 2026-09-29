#include "games/ravenloft/game/NpcDirectionSelector1AF90.h"
#include <array>
#include <cstdlib>

namespace ravenloft::game {

int NpcDirectionSelector1AF90::select(std::int32_t deltaXFixed,
                                      std::int32_t deltaYFixed,
                                      int currentDirection,
                                      int retry,
                                      OriginalRng30CA2& rng){
    std::array<std::uint8_t,8> p{}; // DS:140B0 memset(0,8)
    const int x=deltaXFixed>>16;
    const int y=deltaYFixed>>16;
    const int sy=(y<0)?-1:((y>0)?1:0); // EDI in original
    const int sx=(x<0)?-1:((x>0)?1:0); // ESI

    if(x==0){
        if(sy==-1){p[4]=3;p[6]=2;p[2]=1;}
        else if(sy==1){p[0]=3;p[6]=2;p[2]=1;}
    }else if(y==0){
        if(sx==-1){p[6]=3;p[4]=2;p[0]=1;}
        else if(sx==1){p[2]=3;p[4]=2;p[0]=1;}
    }else if(std::abs(x)>=std::abs(y)){
        if(sx==-1){
            p[6]=4;
            if(sy==-1){p[4]=3;p[0]=2;p[2]=1;}
            else {p[0]=3;p[4]=2;p[2]=1;}
        }else{
            p[2]=4;
            if(sy==-1){p[4]=3;p[0]=2;p[6]=1;}
            else {p[0]=3;p[4]=2;p[6]=1;}
        }
    }else{
        if(sy==-1){
            p[4]=4;
            if(sx==-1){p[6]=3;p[2]=2;p[0]=1;}
            else {p[2]=3;p[6]=2;p[0]=1;}
        }else{
            p[0]=4;
            if(sx==-1){p[6]=3;p[2]=2;p[4]=1;}
            else {p[2]=3;p[6]=2;p[4]=1;}
        }
    }

    const int desiredPriority=4-retry;
    if(desiredPriority==0)
        return static_cast<int>((static_cast<std::uint32_t>(rng.next15())*8u)>>15u);
    for(int i=0;i<8;++i)if(p[static_cast<std::size_t>(i)]==desiredPriority)return i;
    return currentDirection; // original leaves arg3 unchanged when no priority matches
}

} // namespace ravenloft::game
