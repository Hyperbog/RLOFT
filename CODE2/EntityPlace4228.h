#pragma once
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct EntityPlace4228State {
 std::int16_t entityCount258A2=0; std::int32_t playerX255D0=0,playerY255D4=0;
 std::span<std::uint8_t> entities14D78; std::span<const std::int16_t> targets258AA;
};
struct EntityPlace4228Ops { std::function<std::uint16_t(std::int32_t,std::int32_t,int)> call2D5D0; };
class EntityPlace4228 { public: static int run(EntityPlace4228State&,const EntityPlace4228Ops&); };
}
