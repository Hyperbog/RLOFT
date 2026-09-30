#pragma once
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct PartyHit9070State {
 std::span<std::uint8_t> characters;
 std::span<const std::uint8_t> attackTable;
 std::span<const std::uint8_t> typeTable;
 std::int32_t playerX=0,playerY=0;
};
struct PartyHit9070Ops {
 std::function<int(int,int,int,int)> dice1C328;
 std::function<int(int)> active14A60;
 std::function<int(int,int)> traitF2D4;
 std::function<int(int,int)> resistCF2C;
 std::function<void(int,int)> damageCD44;
};
class PartyHit9070 { public: static void run(PartyHit9070State&,const PartyHit9070Ops&,std::uint8_t*entity,bool active); };
}
