#pragma once
#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ConfigMenu6548State { std::uint32_t c050=0; std::array<std::uint32_t,10> surfaces{}; std::uint8_t c026=0,c027=0,c028=0,c029=0,c02b=0,c02c=0,value258B4=0; };
struct ConfigMenu6548Ops { std::function<int()> input1A1F4; std::function<std::uint16_t()> buttons9E35; std::function<std::int16_t()> mouseX9E31,mouseY9E33; std::function<void()> slider62AC,slider6444,slider6354,draw5EEC,flip1A9F8,action20994; std::function<void(int)> call28098,call290A8,free30040; std::function<void(int,int,int)> call28E58; std::function<void(std::uint32_t)> call1A510; };
class ConfigMenu6548 { public: static void run(ConfigMenu6548State&,const ConfigMenu6548Ops&); };
}
