#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ConfigIo5DD0State { std::int32_t handle6138=0; std::int32_t resource60DC=0; std::uint8_t c02b=0,c02c=0,c02e=0; std::int32_t c034=0; };
struct ConfigIo5DD0Ops {
 std::function<void(int)> rewind31380,close312DC,flush312B4,open312A0;
 std::function<int(int)> tell312C8;
 std::function<int()> create312B4;
 std::function<void(void*,int,int)> write31304,read312EC;
 std::function<void(int,int)> error2F5F4;
 std::function<void(int,int,int,int,int)> blit325B0;
 std::function<void(int)> restore29234,call28098,call290A8;
};
class ConfigIo5DD0 { public: static void save(ConfigIo5DD0State&,const ConfigIo5DD0Ops&); static void load(ConfigIo5DD0State&,const ConfigIo5DD0Ops&); };
}
