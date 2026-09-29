#pragma once

#include "RuntimeRng618A.h"

#include <array>
#include <cstdint>

namespace ravenloft::game {

struct CharacterInitState3884 {
    std::uint8_t field680C = 0;
    std::array<std::uint8_t, 3> fields680D{{0xFFu, 0xFFu, 0xFFu}};
    std::uint16_t word6810 = 0;
    std::uint8_t field6813 = 0;
    std::uint16_t word6816 = 0;
    std::array<std::uint16_t, 3> words6818{};
    std::array<std::uint32_t, 3> dwords681E{};
    std::array<std::uint8_t, 14> bytes682A{};
    std::int32_t global7118 = 0;
    std::int32_t global711C = 0;
};

class CharacterInit3884 {
public:
    // Direct reconstruction of CODE.1 0x3884..0x38A3.
    static void initialize(CharacterInitState3884& state,
                           RuntimeRng618A& rng) noexcept;
};

} // namespace ravenloft::game
