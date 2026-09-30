#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ConfigSliders62ACState { std::uint32_t c050=0,resource258DC=0; std::uint8_t c025=0,c02a=0,c02d=0,c02e=0; std::int16_t value258B2=0; std::uint32_t surface12B30=0,surface12B44=0,surface12B50=0,c034=0; std::uint8_t* f050=nullptr; };
struct ConfigSliders62ACOps { std::function<int()> input1A1F4; std::function<std::int16_t()> mouseY9E33; std::function<void(std::uint8_t)> call291B0; std::function<void(std::uint32_t,int,int,int)> call2A380; std::function<void(int,int,int,int,int)> call2975C,call1A8C8,call325B0; std::function<void(std::uint32_t)> call29234; };
class ConfigSliders62AC { public: static void ten(ConfigSliders62ACState&,const ConfigSliders62ACOps&); static void fifteen(ConfigSliders62ACState&,const ConfigSliders62ACOps&); static void analog(ConfigSliders62ACState&,const ConfigSliders62ACOps&); };
}
