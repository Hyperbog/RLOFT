#pragma once

#include "RuntimeRng618A.h"

#include <array>
#include <cstdint>

namespace ravenloft::game {

struct CharacterRollState334C {
    // Character-record offsets 0x14..0x17 (DS:680C..680F).
    std::uint8_t field680C = 0;
    std::array<std::uint8_t, 3> fields680D{{0xFFu, 0xFFu, 0xFFu}};

    // Character-record offsets 0x32..0x3F (DS:682A..6837).
    std::array<std::uint8_t, 14> bytes682A{};
};

class CharacterRolls334C {
public:
    // Direct reconstruction of CODE.1 0x334C..0x3403.
    static void initialize(CharacterRollState334C& state,
                           RuntimeRng618A& rng) noexcept;
};

} // namespace ravenloft::game
