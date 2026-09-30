#pragma once
#include <cstdint>
namespace ravenloft::game {
struct EntityMarkB6F4State { std::uint8_t* entities=nullptr; const std::uint8_t* types=nullptr; std::int16_t entityCount=0; };
class EntityMarkB6F4 { public: static void run(EntityMarkB6F4State&); };
}
