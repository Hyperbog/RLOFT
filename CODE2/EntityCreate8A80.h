#pragma once
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct EntityCreate8A80State { std::span<std::uint8_t> entities; std::int32_t count=0,playerX=0,playerY=0; std::int16_t scale25619=0; std::uint8_t bf38=0; };
struct EntityCreate8A80Ops { std::function<void(int)> initE578; std::function<std::uint32_t()> random30CA2; std::function<void(int,int,int,int)> spawn28110; std::function<int(int,int)> probe2C9DC; std::function<void(std::uint8_t*,int)> adjust22E90; std::function<void(int)> call19CCC; std::function<void()> call1F1C8,call1EA04,call1EA78,call19CF4; };
class EntityCreate8A80 { public: static std::uint8_t* run(EntityCreate8A80State&,const EntityCreate8A80Ops&,int type,int x,int y,int id,int orientation,int owner,int resource); };
}
