#pragma once
#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct NearbyEntityAED8State { std::uint8_t* entities=nullptr; const std::uint8_t* typeTable=nullptr; std::int16_t entityCount=0,mapWidth=0,mapHeight=0,cellX=0,cellY=0; std::int32_t playerX=0,playerY=0; };
struct NearbyEntityAED8Ops { std::function<int(int,int,int,int)> dice1C328; std::function<int(int,int)> value22E40; std::function<bool(int)> cellFlag40; std::function<void(std::uint8_t*,const std::uint8_t*,int,int,int)> effectE284; };
class NearbyEntityAED8 { public: static void run(NearbyEntityAED8State&,const NearbyEntityAED8Ops&,int character,bool alternate); };
}
