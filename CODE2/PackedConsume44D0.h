#pragma once
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct PackedConsume44D0State { std::span<std::uint8_t> objects; std::span<std::uint8_t> flagsCF5A; };
struct PackedConsume44D0Ops {
 std::function<void(int,int,int)> call9D90;
 std::function<void(int)> call1F494;
};
class PackedConsume44D0 { public:
 static int run(PackedConsume44D0State&,const PackedConsume44D0Ops&,int object,int row,int col,int* timer,int* count,int kind,int timeBase);
};
}
