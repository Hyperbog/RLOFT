#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct OppositeBufferHit94E0State { std::uint32_t c050=0; std::array<std::uint16_t,2> counts{}; std::span<std::uint8_t> records; std::span<const std::uint8_t> typeTable; };
struct OppositeBufferHit94E0Ops { std::function<int(int)> probe2ACDC; std::function<int(const std::uint8_t*,int)> probe21BF0; };
class OppositeBufferHit94E0 { public: static int run(OppositeBufferHit94E0State&,const OppositeBufferHit94E0Ops&,int x,int y); };
}
