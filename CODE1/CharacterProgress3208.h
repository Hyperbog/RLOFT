#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CharacterProgressState3208 {
    std::array<std::uint8_t, 3> fields680D{{0xFFu, 0xFFu, 0xFFu}};
    std::array<std::uint16_t, 3> words6818{};
    std::array<std::uint32_t, 3> dwords681E{};
};

struct CharacterProgressOps3208 {
    std::function<std::uint8_t(std::uint32_t)> readByte;
    std::function<std::uint16_t(std::uint32_t)> readWord;
    std::function<std::uint32_t(std::uint32_t)> readDword;
    std::function<void(std::uint32_t, std::uint16_t)> writeWord;
    std::function<void(std::uint32_t, std::uint32_t)> writeDword;
};

class CharacterProgress3208 {
public:
    // Faithful architectural-memory reconstruction of CODE.1 0x3208..0x32F3.
    static void run(const CharacterProgressOps3208& ops);

    // Compatibility adapter retained for existing higher-level callers.
    static void initialize(CharacterProgressState3208& state) noexcept;
};

} // namespace ravenloft::game
