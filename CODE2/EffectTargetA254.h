#pragma once
#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct EffectTargetA254State {
 std::uint8_t* characters=nullptr; std::int32_t resourceBase=0;
 std::uint8_t d681=0,d682=0; std::array<std::uint8_t,4> refresh{};
};
struct EffectTargetA254Ops {
 std::function<void(int)> ui1F494; std::function<void()> prompt,begin,end,finish;
 std::function<int()> input; std::function<std::int16_t()> mouseX,mouseY;
 std::function<bool(int,int)> insidePartyRect;
 std::function<void(int,int,int,int)> playResource;
 std::function<void(int,int,int)> render9B4C;
 std::function<int(int,int,int)> effect9D90;
 std::function<std::int32_t()> clock;
 std::function<int(int,int)> value22E40;
 std::function<int(int,int,int,int)> dice;
};
class EffectTargetA254 {
public:
 static int run(EffectTargetA254State&,const EffectTargetA254Ops&,int mask,int effect,
                int descriptor,int duration,int resourceOffset,int effectArg,
                int forceDispatcher,int durationMultiplier);
};
}
