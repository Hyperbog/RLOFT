#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct PanelRender9B4CState { std::uint8_t* characters=nullptr; const std::uint8_t* descriptors=nullptr; std::uint32_t c050=0; int resourceBase=0; };
struct PanelRender9B4COps {
 std::function<void()> begin,finish;
 std::function<void(int)> prepare;
 std::function<void(int,int,int,int,int)> draw;
 std::function<void(std::uint32_t,int,int,int,int)> present;
 std::function<void(int,int)> step;
};
class PanelRender9B4C { public: static void run(PanelRender9B4CState&,const PanelRender9B4COps&,int character,int descriptor,int allCharacters); };
}
