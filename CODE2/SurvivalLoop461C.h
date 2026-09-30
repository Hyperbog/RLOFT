#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct SurvivalLoop461CState { std::span<std::uint8_t> objects218E0; std::array<std::uint8_t,4> dirtyCF5A{}; std::int32_t tickA7E4=0,tickCF4C=0; std::uint16_t inputCF4E=0; std::uint8_t active258A8=0xFF,d681=0,d682=0; std::int32_t d8A8=0,d8AC=0; };
struct SurvivalLoop461COps {
 std::function<int()> call4100,call30B40,call30CA2,call4228;
 std::function<int(int,int)> call50DC; std::function<void(int,int)> callCD44;
 std::function<void(int)> call1F494,call1E74C; std::function<void()> call31F94,call893C,call30AEC;
 std::function<int(std::int32_t)> call7D44; std::function<int(int,int)> call23EAC;
 std::function<int(int,int,int,int*,int*,int,int)> call44D0; std::function<void(int,int*,int*)> call43F4;
 std::function<void(int,int)> call231C0,callMessage;
};
class SurvivalLoop461C { public: static void run(SurvivalLoop461CState&,const SurvivalLoop461COps&); };
}
