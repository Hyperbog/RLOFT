#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct NearbySpawn9368State { std::int16_t playerX=0,playerY=0; std::int32_t resource258D4=0; };
struct NearbySpawn9368Ops { std::function<bool(int)> ownerFlag198E0; std::function<void(int,int,int,int)> spawn28110; };
class NearbySpawn9368 { public: static void run(NearbySpawn9368State&,const NearbySpawn9368Ops&,const std::uint8_t*entity); };
}
