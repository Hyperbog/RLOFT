#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <string>
namespace ravenloft::game {
struct SaveSlotList7070State { std::array<std::uint32_t,10> surfaces{}; const char* emptyLabel=nullptr; };
struct SaveSlotList7070Ops {
 std::function<int(const char*)> open312A0;
 std::function<void(void*,int,int)> read312EC;
 std::function<void(int)> close312DC;
 std::function<std::uint32_t(int,int,int,int,int)> alloc30070;
 std::function<void(int,int,int,int,int)> draw2975C;
 std::function<void(int,int,int,int)> draw3060C;
};
class SaveSlotList7070 { public: static void build(SaveSlotList7070State&,const SaveSlotList7070Ops&,int baseX,int baseY,int arg2); };
struct SaveSlotList719CState { std::array<std::uint32_t,10> surfaces{}; };
struct SaveSlotList719COps { std::function<void(std::uint32_t)> free30040; };
class SaveSlotList719C { public: static void destroy(SaveSlotList719CState&,const SaveSlotList719COps&); };
}
