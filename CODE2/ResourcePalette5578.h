#pragma once
#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ResourcePalette5578State { std::array<std::uint8_t,256> remap12A30{}; };
struct ResourcePalette5578Ops {
 std::function<void(int,int,int,int,int,int)> call32220;
 std::function<void(int,int,int)> call30D30;
 std::function<void(int,const std::array<std::uint8_t,256>&,int)> call2AA9C;
 std::function<void(const std::array<std::uint8_t,256>&)> call3059C;
};
class ResourcePalette5578 { public: static void run(ResourcePalette5578State&,const ResourcePalette5578Ops&); };
}
