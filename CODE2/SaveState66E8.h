#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct SaveState66E8 {
 std::span<std::uint8_t> ds; std::array<std::uint8_t,0x32> name12990{};
};
struct SaveState66E8Ops {
 std::function<void(const char*)> rewind31380;
 std::function<int(const char*)> tell312C8;
 std::function<void(int)> close312DC;
 std::function<int(const char*)> create312B4;
 std::function<int(const void*,int,int)> write31304;
 std::function<std::uint32_t()> time30700;
};
class SaveState66E8 { public: static int save(SaveState66E8&,const SaveState66E8Ops&,int slot); };
}
