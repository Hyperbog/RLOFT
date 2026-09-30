#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct PlayerShiftAE30State { std::int32_t x=0,y=0,facing=0,resourceBase=0; };
struct PlayerShiftAE30Ops { std::function<void(int,int,int,int)> play28110; std::function<void(int&,int&,int,int)> vector2DF2C; std::function<int(int,int,int)> collision2D5D0; std::function<void()> refresh1F1C8,update1EA78; };
class PlayerShiftAE30 { public: static void run(PlayerShiftAE30State&,const PlayerShiftAE30Ops&); };
}
