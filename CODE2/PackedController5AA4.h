#pragma once
#include <cstdint>
#include <functional>
#include <vector>
namespace ravenloft::game {
struct PackedController5AA4Ops {
 std::function<void*(int)> alloc3104C; std::function<void(void*)> free310B0;
 std::function<void(int,int,int,int,int,int,int)> draw518C;
 std::function<int()> input1A1F4; std::function<void(int,int)> show5984; std::function<void(int)> hide5A14;
 std::function<int(int,int,int,int,int,int,int)> hit5698; std::function<int(int,int)> select50DC; std::function<void()> cleanup5578;
 std::function<std::int16_t()> mouseX9E31,mouseY9E33;
};
class PackedController5AA4 { public: static int run(const PackedController5AA4Ops&,int object,int selected,int a,int b,int c,int d,int e); };
}
