#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct PackedGrid3A78State {
 std::span<std::uint8_t> objectBytes; // DS region containing 0x218F5/0x2194A/0x2199A
 std::array<std::uint8_t,8> limits6208{};
 std::array<std::int32_t,8> counts6440{};
};
struct PackedGrid3A78Ops {
 std::function<void(std::int32_t,std::int32_t,std::int32_t,std::int32_t)> call5BDC;
};
class PackedGrid3A78 {
public:
 static void normalize3A78(PackedGrid3A78State&,const PackedGrid3A78Ops&,std::int32_t object,std::int32_t level);
 static void normalize3C04(PackedGrid3A78State&,const PackedGrid3A78Ops&,std::int32_t object,std::int32_t level);
};
}
