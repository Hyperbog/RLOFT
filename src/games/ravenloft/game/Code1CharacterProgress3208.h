#pragma once

#include <array>
#include <cstdint>

namespace ravenloft::game {

struct Code1CharacterProgressState3208 {
    // Character-record offsets 0x15..0x17 (DS:680D..680F).
    std::array<std::uint8_t, 3> fields680D{{0xFFu, 0xFFu, 0xFFu}};

    // Character-record offsets 0x20..0x25 (DS:6818..681D).
    std::array<std::uint16_t, 3> words6818{};

    // Character-record offsets 0x26..0x31 (DS:681E..6829).
    std::array<std::uint32_t, 3> dwords681E{};
};

class Code1CharacterProgress3208 {
public:
    // Direct reconstruction of CODE.1 0x3208..0x32F3.
    static void initialize(Code1CharacterProgressState3208& state) noexcept;
};

} // namespace ravenloft::game
