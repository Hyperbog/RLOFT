#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct SurvivalLoop461CState {
 std::span<std::uint8_t> objects218E0;
 std::array<std::uint8_t,4> dirtyCF5A{};
 std::int32_t tickA7E4=0,tickCF4C=0;
 std::uint16_t inputCF4E=0;
 std::uint8_t active258A8=0xFF;
};
struct SurvivalLoop461COps {
 std::function<int()> call4100;
 std::function<int(int,int)> call50DC;
 std::function<void(int,int)> callCD44;
 std::function<void(int)> call1F494;
 std::function<int()> call30B40;
 std::function<void()> call31F94,call893C;
};
class SurvivalLoop461C { public: static void run(SurvivalLoop461CState&,const SurvivalLoop461COps&); };
}
