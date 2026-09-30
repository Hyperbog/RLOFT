#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct CharacterEffect9D90State { std::uint8_t* characters=nullptr; };
struct CharacterEffect9D90Ops {
 std::function<int(int,int,int,int)> dice;
 std::function<int(int,int)> value22E40;
 std::function<void(const std::uint8_t*,int)> message;
};
class CharacterEffect9D90 { public: static int run(CharacterEffect9D90State&,const CharacterEffect9D90Ops&,int effect,int character,int target); };
}
