#pragma once
#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct AttributeBoostA8A0State { std::uint8_t* characters=nullptr; int resourceBase=0; std::uint8_t d681=0,d682=0; std::array<std::uint8_t,4> refresh{}; };
struct AttributeBoostA8A0Ops {
 std::function<void(int,int,int,int)> play28110; std::function<void(int)> ui1F494;
 std::function<void(int,int,int)> panel9B4C; std::function<int(int,int)> value22E40;
 std::function<std::int32_t()> clock19CC4; std::function<int(int,int)> adjust1202C;
};
class AttributeBoostA8A0 { public: static int first(AttributeBoostA8A0State&,const AttributeBoostA8A0Ops&,int character); static int second(AttributeBoostA8A0State&,const AttributeBoostA8A0Ops&,int character); };
}
