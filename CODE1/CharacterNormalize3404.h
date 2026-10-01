#pragma once

#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CharacterNormalizeOps3404 {
    std::function<std::uint8_t(std::uint32_t)> readByte;
    std::function<std::uint16_t(std::uint32_t)> readWord;
    std::function<std::uint32_t(std::uint32_t)> readDword;
    std::function<void(std::uint32_t, std::uint8_t)> writeByte;
    std::function<void(std::uint32_t, std::uint16_t)> writeWord;
    std::function<void(std::uint32_t, std::uint32_t)> writeDword;
};

class CharacterNormalize3404 {
public:
    // Direct reconstruction of CODE.1 0x3404..0x37BD. All DS table and
    // character accesses are raw; no host-side bounds assumptions are made.
    static void apply(const CharacterNormalizeOps3404& ops);
};

} // namespace ravenloft::game
