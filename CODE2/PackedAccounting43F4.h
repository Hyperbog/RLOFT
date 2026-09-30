#pragma once
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct PackedAccounting43F4Ops { std::function<int(int,int)> call50DC; };
class PackedAccounting43F4 {
public:
 static void adjust(std::span<const std::uint8_t> objects,const PackedAccounting43F4Ops&,int object,int* primary,int* secondary);
};
}
