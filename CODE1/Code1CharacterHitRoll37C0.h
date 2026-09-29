#pragma once

#include "Code1RuntimeRng618A.h"

#include <array>
#include <cstdint>

namespace ravenloft::game {

struct Code1CharacterHitRollState37C0 {
    std::array<std::uint8_t, 3> fields680D{{0xFFu, 0xFFu, 0xFFu}};
    std::uint16_t word6816 = 0;
    std::array<std::uint16_t, 3> words6818{};
    std::array<std::uint8_t, 14> bytes682A{};
};

class Code1CharacterHitRoll37C0 {
public:
    // Direct reconstruction of CODE.1 0x37C0..0x3881.
    static void initialize(Code1CharacterHitRollState37C0& state,
                           Code1RuntimeRng618A& rng) noexcept;
};

} // namespace ravenloft::game
