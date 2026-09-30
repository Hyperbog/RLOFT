#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ConfirmDialog7D44State { std::uint32_t c050=0,resource258DC=0; };
struct ConfirmDialog7D44Ops { std::function<void(std::uint8_t)> call291B0; std::function<std::uint32_t(int,int,int,int)> call30CA2; std::function<void(int,int,int,int,int)> draw2975C,draw30508,present1A8C8; std::function<void(std::uint32_t)> call1A510; std::function<void()> flip1A9F8; std::function<int()> input1A1F4; std::function<std::int16_t()> mouseX9E31,mouseY9E33; };
class ConfirmDialog7D44 { public: static int run(ConfirmDialog7D44State&,const ConfirmDialog7D44Ops&,int textResource); };
}
