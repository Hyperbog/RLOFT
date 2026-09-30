#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct ResourcePanel518CState {
 std::span<const std::uint8_t> objects218E0;
 std::array<std::uint8_t,256> remap12A30{};
 std::array<std::span<std::int8_t>,6> rowBuffers12B58;
 std::array<std::int32_t,6> rowX63F0{},rowY63F4{},counts6420{};
 std::uint32_t valueC050=0,value258DC=0,value60E0=0,value609C=0,value60A0=0,value60A4=0,value60A8=0;
 std::int16_t value8600=0; std::uint8_t value8602=0;
};
struct ResourcePanel518COps {
 std::function<int(int,int)> call50DC;
 std::function<void(std::uint8_t)> call291B0;
 std::function<void(const std::array<std::uint8_t,256>&)> call3059C;
 std::function<void(int,int,int,int)> call2E510;
 std::function<void(int,int,int)> call30D30;
 std::function<void(int,int,int,int,int)> call2975C,call30508;
 std::function<void(int,int,int)> call3210F;
 std::function<void(int,int,int,int)> call3060C;
 std::function<int(int)> call2E6F0;
 std::function<void(int,int)> call1C368;
 std::function<void(int,int,int,int,int)> call30630;
 std::function<void(int,int,int,int,int,int)> call32220;
 std::function<void(int,int,int,int,int)> call325B0;
 std::function<void(int,int,int,int,int,int)> call32468;
 std::function<void(int,const std::array<std::uint8_t,256>&,int)> call2AA9C;
 std::function<void(std::uint32_t,std::int16_t,std::uint8_t)> call1A5B0;
 std::function<void(int,int,int,int,int,int)> call322EC;
};
class ResourcePanel518C { public: static void draw(ResourcePanel518CState&,const ResourcePanel518COps&,int,int,int,int,int,const std::uint8_t*,const std::uint8_t*); };
}
