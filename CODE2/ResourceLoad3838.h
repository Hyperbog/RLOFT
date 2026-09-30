#pragma once
#include <cstdint>
#include <functional>
#include <span>
#include <vector>
namespace ravenloft::game {
struct ResourceLoad3838State {
 std::vector<std::uint8_t> blockC65C,blockC660,heap608C;
 std::span<std::uint8_t> table19660,table193E0,blockC73C;
 std::int32_t c6D8=0,c6D4=0,c6D0=0,c6DC=0,c6E0=0,value2560C=0,c6CC=0,value255E0=0;
 std::int32_t c738=0,c730=0,c734=0,value6090=0;
 std::vector<std::uint16_t> table258C4;
 std::vector<std::int32_t> ptr18FE0,ptr15618;
 std::int32_t value6094=0;
};
struct ResourceLoad3838Ops {
 std::function<std::int32_t(std::int32_t)> call312A0;
 std::function<void(std::uint8_t*,std::int32_t,std::int32_t)> call312EC;
 std::function<void(std::int32_t)> call312DC;
};
class ResourceLoad3838 {
public:
 static void cleanup380D(ResourceLoad3838State&);
 static std::int32_t bump3820(ResourceLoad3838State&,std::int32_t);
 static void run3838(ResourceLoad3838State&,const ResourceLoad3838Ops&);
};
}
