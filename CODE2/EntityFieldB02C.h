#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct EntityFieldB02CState {
 std::uint8_t c02f=0; std::int32_t facing=0, count=0; std::int16_t mapWidth=0,mapHeight=0,typeCount=0;
 std::uint8_t* records=nullptr; std::uint8_t* worldFlags=nullptr; std::uint8_t* entities=nullptr; const std::uint8_t* typeTable=nullptr; const std::uint8_t* objectTable=nullptr;
 int resourceBase=0,alternateResourceBase=0;
};
struct EntityFieldB02COps {
 std::function<std::uint32_t()> random30CA2; std::function<void(int&,int&,int,int)> vector2DF2C; std::function<int(int,int)> blocked2C9DC;
 std::function<int(int)> probe1E930; std::function<void(std::uint8_t*,const std::uint8_t*,int,int,int)> effectE284;
 std::function<void(int,int,int,int)> play28110; std::function<void()> refresh1F1C8,update1EA78,finish28168;
 std::function<void(int)> remove8C88;
};
class EntityFieldB02C { public: static void run(EntityFieldB02CState&,const EntityFieldB02COps&,int start,int end,int steps,int mask,bool dedupe,int kind,int resourceOffset,int x,int y); };
}
