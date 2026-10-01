#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CharacterActivationState3138 {};

struct CharacterActivationOps3138 {
    std::function<std::uint8_t(std::uint32_t)> readByte;
    std::function<std::uint32_t(std::uint32_t)> readDword;
    std::function<void(std::uint32_t,std::uint8_t)> writeByte;
    std::function<void(std::uint32_t,std::uint32_t)> writeDword;
    std::function<void(std::int32_t,const std::array<std::int32_t,9>&,
                       std::int32_t,std::int32_t,std::int32_t)> call1464;
    std::function<std::int32_t()> call043C;
    std::function<void(std::int32_t,std::int32_t,
                       std::int32_t,std::int32_t)> call15B0;
    std::function<void()> call3020;
};

class CharacterActivation3138 {
public:
    // Fidelity reconstruction of CODE.1 0x3138..0x3204.
    static void run(CharacterActivationState3138& state,
                    const CharacterActivationOps3138& ops);
};

} // namespace ravenloft::game
