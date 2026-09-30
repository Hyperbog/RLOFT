#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct DirectionPicker9418State { std::uint8_t c02f=0; std::int32_t facingC658=0; };
struct DirectionPicker9418Ops { std::function<void()> message,call1C258,flip1A9F8,call1C2CC,call22D70; std::function<int()> input1A1F4; std::function<std::int16_t()> mouseX9E31,mouseY9E33; };
class DirectionPicker9418 { public: static int run(DirectionPicker9418State&,const DirectionPicker9418Ops&,bool relative); };
}
