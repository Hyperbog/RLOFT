#pragma once
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct PackedModeDrivers5C84State { std::span<std::uint8_t> objects; std::span<std::uint8_t> limits6208; std::uint8_t flag620A=0; std::int32_t resource258DC=0; };
struct PackedModeDrivers5C84Ops {
 std::function<int(int,int)> select50DC;
 std::function<void(int,int,int,std::span<const std::uint8_t>)> build5BDC;
 std::function<int(int,int,int,int,int,std::uint8_t*,int)> controller5AA4;
 std::function<int(int,int)> callF2D4;
};
class PackedModeDrivers5C84 { public: static int mode0(PackedModeDrivers5C84State&,const PackedModeDrivers5C84Ops&); static int mode1(PackedModeDrivers5C84State&,const PackedModeDrivers5C84Ops&); };
}
