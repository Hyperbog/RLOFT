#pragma once

#include <array>
#include <cstdint>

namespace ravenloft::game {

class CharacterTriple1F24 {
public:
    // Direct reconstruction of CODE.1 0x1F24..0x2196.
    // The three bytes correspond to character-record offsets 0x15..0x17
    // (DS:680D..680F for character 0). 0xFF is the original empty sentinel.
    //
    // DL is NOT initialized by this routine. Some paths reach the dispatch at
    // 0x20BC without assigning DL, so the incoming low byte of EDX is part of
    // the routine's architectural input and must be supplied by the caller.
    static void normalize(std::array<std::uint8_t, 3>& fields680D,
                          std::uint8_t inheritedDL) noexcept;
};

} // namespace ravenloft::game
