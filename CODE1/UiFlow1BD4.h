#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct UiFlowState1BD4 {
    std::int32_t previous904 = 0;
    std::int32_t current908 = 0;
    std::int32_t previous90C = 0;
    std::int32_t current910 = 0;
    std::int32_t current940 = -1;
    std::int32_t selectedCharacter4CC = 0;
    std::array<std::uint16_t, 2> word6808{};
    std::array<std::uint8_t, 2> flags4C4{};
    std::uint8_t buffer7138 = 0;
};

struct UiFlowOps1BD4 {
    std::function<void(std::int32_t, std::int32_t)> call07E4;
    std::function<void(std::uint8_t)> call6588;
    std::function<void(std::int32_t)> call04B0;
    std::function<void(std::uint8_t, std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call550C;
    std::function<std::int32_t()> call043C;
    std::function<void()> call1A04;
};

class UiFlow1BD4 {
public:
    // Direct reconstruction of CODE.1 0x1BD4..0x1CCF.
    static void run(UiFlowState1BD4& state,
                    const UiFlowOps1BD4& ops);
};

} // namespace ravenloft::game
