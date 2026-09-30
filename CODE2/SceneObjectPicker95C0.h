#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct SceneObjectPicker95C0State { std::uint32_t c050=0; std::uint16_t oppositeCount=0; std::uint8_t mouseButtons=0; };
struct SceneObjectPicker95C0Ops { std::function<bool(int)> slotOccupied; std::function<void()> prompt,call1C258,flip1A9F8,call1C2CC,call22D70; std::function<int()> input1A1F4,key30B40; std::function<std::int16_t()> mouseX9E31,mouseY9E33; std::function<int(int,int)> hit94E0; };
class SceneObjectPicker95C0 { public: static int run(SceneObjectPicker95C0State&,const SceneObjectPicker95C0Ops&); };
}
