#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CharacterActivationState3138 {
    std::int32_t mode4D0 = 0;
    std::int32_t selectedCharacter4CC = 0;
    std::array<std::uint8_t, 2> active4C4{};
    std::array<std::uint16_t, 2> status6808{};
};

struct CharacterActivationOps3138 {
    std::function<void(std::int32_t,
                       const std::array<std::int32_t, 9>&,
                       std::int32_t, std::int32_t, std::int32_t)> call1464;
    std::function<std::int32_t()> call043C;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call15B0;
    std::function<void()> call3020;
};

class CharacterActivation3138 {
public:
    // Direct reconstruction of CODE.1 0x3138..0x3204.
    static void run(CharacterActivationState3138& state,
                    const CharacterActivationOps3138& ops);
};

} // namespace ravenloft::game
