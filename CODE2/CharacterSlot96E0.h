#pragma once
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct CharacterSlot96E0State { std::span<std::uint8_t> characters; std::int32_t playerX=0,playerY=0; };
struct CharacterSlot96E0Ops { std::function<void(int,int,int,int,int,int,int)> fallback2195C; };
class CharacterSlot96E0 { public: static int run(CharacterSlot96E0State&,const CharacterSlot96E0Ops&,int first,int second,int a,int b,int chooseFirst,int character); };
}
