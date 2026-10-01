#pragma once

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CharacterInitOps3884 {
    std::function<std::uint8_t(std::uint32_t)> readByte;
    std::function<std::uint16_t(std::uint32_t)> readWord;
    std::function<std::uint32_t(std::uint32_t)> readDword;
    std::function<void(std::uint32_t, std::uint8_t)> writeByte;
    std::function<void(std::uint32_t, std::uint16_t)> writeWord;
    std::function<void(std::uint32_t, std::uint32_t)> writeDword;
    std::function<std::uint32_t(std::uint32_t, std::uint32_t,
                                std::uint32_t, std::uint32_t)> call0010;
    std::function<void(std::uint32_t, std::uint32_t, std::uint32_t)> call84A0;
};

class CharacterInit3884 {
public:
    // Direct reconstruction of CODE.1 0x3884..0x38A3.
    static void initialize(const CharacterInitOps3884& ops);
};

} // namespace ravenloft::game
