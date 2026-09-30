#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ConfigScreen6008State { std::uint32_t c050=0; std::int32_t resource258DC=0; std::uint32_t art60E4=0,art60E8=0; };
struct ConfigScreen6008Ops {
 std::function<void(std::uint8_t)> call291B0;
 std::function<std::uint32_t(int,int,int,int)> call30CA2;
 std::function<void(int,int,int,int,int)> call2975C,call2E374,call30070;
 std::function<void(int,int,int,int)> call3060C,call305A8;
 std::function<std::uint32_t(int,int,int,int,int)> call30070Alloc;
 std::function<void()> call5EEC,call1A9F8;
 std::function<void(std::uint32_t)> call1A510;
};
class ConfigScreen6008 { public: static void draw(ConfigScreen6008State&,const ConfigScreen6008Ops&); };
}
