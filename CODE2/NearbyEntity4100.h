#pragma once
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct NearbyEntity4100State {
 std::int32_t mapC6C8=0,flag6688=0; std::int16_t playerX255D2=0,playerY255D6=0,count258A2=0;
 std::span<const std::uint8_t> entities14D78; std::span<const std::uint16_t> mapFlags198E0;
};
struct NearbyEntity4100Ops { std::function<int()> call4040; std::function<int(std::uint16_t)> call2ACDC; };
class NearbyEntity4100 { public: static int run(NearbyEntity4100State&,const NearbyEntity4100Ops&); };
}
