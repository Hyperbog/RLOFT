#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct EntityBurstB410State { std::uint8_t* defs=nullptr; std::uint8_t* records=nullptr; const std::uint8_t* types=nullptr; int activeCount=0,resourceBase=0; };
struct EntityBurstB410Ops { std::function<int(int,int,int,int)> dice1C328; std::function<void(std::uint8_t*,const std::uint8_t*,int,int,int)> effectE284; std::function<void(int,int,int,int)> play28110; std::function<void()> refresh1F1C8,update1EA78,finish28168; std::function<void(int)> remove8C88; };
class EntityBurstB410 { public: static void run(EntityBurstB410State&,const EntityBurstB410Ops&,int defIndex); };
}
