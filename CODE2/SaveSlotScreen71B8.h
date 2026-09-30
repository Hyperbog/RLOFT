#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct SaveSlotScreen71B8State { std::uint32_t c050=0,resource258DC=0; };
struct SaveSlotScreen71B8Ops {
 std::function<void(std::uint8_t)> call291B0;
 std::function<std::uint32_t(int,int,int,int)> call30CA2;
 std::function<void(int,int,int,int,int)> call2975C;
 std::function<void(int)> build7070;
 std::function<void(std::uint32_t)> call1A510;
 std::function<void()> call1A9F8;
};
class SaveSlotScreen71B8 { public: static void draw(SaveSlotScreen71B8State&,const SaveSlotScreen71B8Ops&,int listArg); };
}
