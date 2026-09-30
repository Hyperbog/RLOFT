#pragma once
#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct LoadSlotController7B94State { std::uint32_t c050=0,resource258DC=0; std::array<std::uint32_t,10> surfaces{}; char path6690[4]={'0','0',0,0}; };
struct LoadSlotController7B94Ops { std::function<void(int)> draw71B8,free30040,call2F580; std::function<int()> input1A1F4; std::function<std::uint16_t()> buttons9E35; std::function<std::int16_t()> mouseX9E31,mouseY9E33; std::function<void(std::uint8_t)> call291B0; std::function<void(int,int,int,int,int)> draw2975C,present1A8C8; std::function<int(const char*)> exists33A5A; std::function<void()> disk7378; std::function<int(const char*,const char*)> copy73A0; std::function<int()> load7938; };
class LoadSlotController7B94 { public: static int run(LoadSlotController7B94State&,const LoadSlotController7B94Ops&,const char*currentPath,const char*auxPath,int msgAux,int msgCopy); };
}
