#pragma once

#include "Code1RuntimeRng618A.h"

#include <cstdint>

namespace ravenloft::game {

struct Code1CharacterVitalState32F4 {
    // Character-record offset 0x14 (DS:680C).
    std::uint8_t field680C = 0;

    // Character-record offset 0x18 (DS:6810).
    std::uint16_t word6810 = 0;
};

class Code1CharacterVital32F4 {
public:
    // Direct reconstruction of CODE.1 0x32F4..0x3349.
    static void initialize(Code1CharacterVitalState32F4& state,
                           Code1RuntimeRng618A& rng) noexcept;
};

} // namespace ravenloft::game
