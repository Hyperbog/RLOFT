#pragma once
#include <cstdint>
#include <functional>
#include <string>
namespace ravenloft::game {
struct SlotNameInput75CCOps { std::function<void(std::uint8_t)> call291B0; std::function<void(std::uint32_t,int,int,int)> restore2A380; std::function<void(int,int,const char*,int)> draw3060C; std::function<void(int,int)> cursor30354; std::function<void(std::uint32_t,int,int,int,int)> present1A8C8; std::function<int()> key30AEC; std::function<int(const char*,int)> width30288; std::function<void(const char*,int,int)> call30D30; std::function<std::uint8_t(int)> charClass165C; };
class SlotNameInput75CC { public: static bool run(const SlotNameInput75CCOps&,std::uint32_t c050,std::uint32_t surface,int slot,const char*initial,char*out); };
}
