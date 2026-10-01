#pragma once

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CharacterHitRollOps37C0 {
    std::function<std::uint8_t(std::uint32_t)> readByte;
    std::function<std::uint16_t(std::uint32_t)> readWord;
    std::function<std::uint32_t(std::uint32_t)> readDword;
    std::function<void(std::uint32_t, std::uint16_t)> writeWord;
    std::function<std::uint32_t(std::uint32_t, std::uint32_t,
                                std::uint32_t, std::uint32_t)> call0010;
};

class CharacterHitRoll37C0 {
public:
    // Direct reconstruction of CODE.1 0x37C0..0x3881.
    static void initialize(const CharacterHitRollOps37C0& ops);
};

} // namespace ravenloft::game
