#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct NearbyEntityB518State { std::uint8_t* characters=nullptr; std::uint8_t* entities=nullptr; const std::uint8_t* types=nullptr; std::int16_t entityCount=0; std::int32_t playerX=0,playerY=0; };
struct NearbyEntityB518Ops {
 std::function<void(std::int32_t*,int,int)> fill30D30; std::function<int(int,int,int,int)> dice1C328;
 std::function<int(int,int)> value22E40; std::function<std::int8_t(int)> table67B2;
 std::function<bool(int)> cellFlag40; std::function<void(std::uint8_t*,const std::uint8_t*,int,int,int)> effectE284;
};
class NearbyEntityB518 { public: static void run(NearbyEntityB518State&,const NearbyEntityB518Ops&,int character,bool alternate); };
}
