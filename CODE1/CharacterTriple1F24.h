#pragma once

#include <array>
#include <cstdint>

namespace ravenloft::game {

class CharacterTriple1F24 {
public:
    // Direct reconstruction of CODE.1 0x1F24..0x2196.
    // The three bytes correspond to character-record offsets 0x15..0x17
    // (DS:680D..680F for character 0). 0xFF is the original empty sentinel.
    static void normalize(std::array<std::uint8_t, 3>& fields680D) noexcept;
};

} // namespace ravenloft::game
