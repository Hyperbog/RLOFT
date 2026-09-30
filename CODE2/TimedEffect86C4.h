#pragma once
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct TimedEffect86C4State { std::span<std::uint8_t> characters; std::span<const std::uint16_t> table67CA; };
struct TimedEffect86C4Ops { std::function<void(int,int)> call24A9C; };
class TimedEffect86C4 { public: static void run(TimedEffect86C4State&,const TimedEffect86C4Ops&,int character,int effect); };
}
