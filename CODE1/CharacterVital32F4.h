#pragma once

#include "RuntimeRng618A.h"

#include <cstdint>

namespace ravenloft::game {

struct CharacterVitalState32F4 {
    // Character-record offset 0x14 (DS:680C).
    std::uint8_t field680C = 0;

    // Character-record offset 0x18 (DS:6810).
    std::uint16_t word6810 = 0;
};

class CharacterVital32F4 {
public:
    // Direct reconstruction of CODE.1 0x32F4..0x3349.
    static void initialize(CharacterVitalState32F4& state,
                           RuntimeRng618A& rng) noexcept;
};

} // namespace ravenloft::game
