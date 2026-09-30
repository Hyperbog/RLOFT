#pragma once
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct EntityInteraction8CF4State { std::span<std::uint8_t> actors, entities, typeTable, diceTable; std::int16_t entityCount=0; };
struct EntityInteraction8CF4Ops { std::function<int(int,int,int,int)> dice1C328; std::function<void(std::uint8_t*,int,int,int,int)> applyE284; std::function<int(int)> valid23F50; std::function<int(int,int)> traitF2D4; };
class EntityInteraction8CF4 { public: static int run(EntityInteraction8CF4State&,const EntityInteraction8CF4Ops&,std::uint8_t*source,bool active); };
}
