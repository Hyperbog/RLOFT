#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct MapCellEffectABC4State { const std::int32_t* table15A2C=nullptr; std::int16_t width=0,height=0; int resourceBase=0; };
struct MapCellEffectABC4Ops { std::function<void(int,int,int,int)> play28110; std::function<std::uint8_t*(int,int)> cell1E8C8; std::function<int(std::uint8_t*,int,int)> update20C0C; std::function<int(int)> query20CF4; std::function<void(int)> dispatch02A8; std::function<int(int)> alloc1C924; std::function<void(int)> wait1C77C; };
class MapCellEffectABC4 { public: static void set(MapCellEffectABC4State&,const MapCellEffectABC4Ops&,int index); static void toggle(MapCellEffectABC4State&,const MapCellEffectABC4Ops&,int index); };
}
