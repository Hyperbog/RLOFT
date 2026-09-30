#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct WorldPlacementB02CState {
 std::uint8_t c02f=0; std::int32_t facing=0; std::int16_t width=0,height=0,entityDefs=0;
 std::int32_t activeCount=0, resourceA=0, resourceB=0;
 std::uint8_t* cells=nullptr; std::uint8_t* records=nullptr; std::uint8_t* defs=nullptr; const std::uint8_t* types=nullptr;
};
struct WorldPlacementB02COps {
 std::function<std::uint32_t()> random30CA2;
 std::function<void(int&,int&,int,int)> vector2DF2C;
 std::function<int(int,int)> occupied2C9DC; std::function<int(int)> cellProbe1E930;
 std::function<void(std::uint8_t*,const std::uint8_t*,int,int,int)> effectE284;
 std::function<void(int,int,int,int)> play28110; std::function<void()> refresh1F1C8,update1EA78,finish28168;
 std::function<void(int)> remove8C88;
};
class WorldPlacementB02C { public: static void run(WorldPlacementB02CState&,const WorldPlacementB02COps&,int angleFirst,int angleLast,int stepLast,int effectMask,int requireUnique,int recordType,int resourceOffset); };
}
