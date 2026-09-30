#pragma once
#include <array>
#include <cstdint>
#include <span>
namespace ravenloft::game {
struct SentinelGrid3DE4State {
 std::span<std::uint8_t> objectBytes;
 std::array<std::int32_t,8> counts6420{};
 std::array<std::array<std::uint8_t,8>,8> flags62B0{};
 std::array<std::array<std::uint8_t,8>,8> flags6350{};
};
class SentinelGrid3DE4 {
public:
 static void apply3DE4(SentinelGrid3DE4State&,std::int32_t object,std::int32_t level);
 static void apply3EA4(SentinelGrid3DE4State&,std::int32_t object,std::int32_t level);
};
}
