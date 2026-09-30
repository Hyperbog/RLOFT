#pragma once
#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct SaveSlotController79E0State { std::uint32_t c050=0,resource258DC=0; std::array<std::uint32_t,10> surfaces{}; char path6690[4]={'0','0',0,0}; };
struct SaveSlotController79E0Ops { std::function<void(int)> draw71B8,free30040; std::function<int()> input1A1F4; std::function<std::uint16_t()> buttons9E35; std::function<std::int16_t()> mouseX9E31,mouseY9E33; std::function<void(std::uint8_t)> call291B0; std::function<void(int,int,int,int,int)> draw2975C,present1A8C8; std::function<int(char*,int)> edit75CC; std::function<int(int)> save66E8; std::function<int(const char*)> exists33A5A; std::function<void(const char*)> call33A38; std::function<void()> disk7378; std::function<void(int)> call2F580; std::function<int(const char*,const char*)> copy73A0; };
class SaveSlotController79E0 { public: static int run(SaveSlotController79E0State&,const SaveSlotController79E0Ops&,char*labels,const char*currentPath,const char*auxPath,int msgAux,int msgCopy); };
}
