#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct StateLoad6CADState {
 std::span<std::uint8_t> ds;
 std::array<std::uint8_t,4> cf5a5d{};
 std::int32_t c6c8=-1,bf44=0,c038=0;
};
struct StateLoad6CADOps {
 std::function<void(void*,int,int)> read312EC;
 std::function<void(int,int,int)> seek3131C;
 std::function<void(int)> call1C0C0,call30738,call1E74C,close312DC,call31EDC,call31F38,call181B4;
 std::function<void(int,int)> call1A344;
 std::function<void* (int)> alloc3104C;
 std::function<void(void*)> free310B0;
 std::function<void(void*,const void*,int)> copy30CD4;
};
class StateLoad6CAD { public: static int run(StateLoad6CADState&,const StateLoad6CADOps&,int handle,int ebxHandle,int arg0,int arg1,int arg2); };
}
