#pragma once
#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ConfigPanel5EECState {
 std::uint32_t c050=0; std::int32_t resource258DC=0;
 std::array<std::int32_t,10> art6110{};
 std::uint8_t c025=0,c026=0,c027=0,c028=0,c029=0,c02a=0,c02b=0,c02c=0,c02e=0,c02f=0;
};
struct ConfigPanel5EECOps {
 std::function<void(std::uint8_t)> call291B0;
 std::function<void(int,int,int,int)> call3060C;
 std::function<void(int,int,int,int,int)> call2975C;
};
class ConfigPanel5EEC { public: static void draw(const ConfigPanel5EECState&,const ConfigPanel5EECOps&); };
}
