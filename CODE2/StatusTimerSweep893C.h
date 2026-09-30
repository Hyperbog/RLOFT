#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct StatusTimerSweep893CState { std::span<std::uint8_t> characters; std::array<std::uint8_t,4> refreshFlags{}; };
struct StatusTimerSweep893COps { std::function<std::int32_t()> time19CC4; std::function<void(int,int)> probe86C4,expire871C,call1202C; std::function<int(int)> call12098; std::function<int(int,int,int,int)> dice1C328; };
class StatusTimerSweep893C { public: static void run(StatusTimerSweep893CState&,const StatusTimerSweep893COps&); };
}
