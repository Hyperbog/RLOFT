#pragma once

#include <array>
#include <cstdint>

namespace ravenloft::game {

struct CharacterNormalizeState3404 {
    std::uint8_t field680C = 0;
    std::array<std::uint8_t, 3> fields680D{{0xFFu, 0xFFu, 0xFFu}};

    std::uint8_t field6813 = 0;
    std::uint16_t word6816 = 0;
    std::array<std::uint16_t, 3> words6818{};

    // Record offsets 0x32..0x3F (DS:682A..6837).
    std::array<std::uint8_t, 14> bytes682A{};

    // Original globals DS:7118 and DS:711C.
    std::int32_t global7118 = 0;
    std::int32_t global711C = 0;
};

class CharacterNormalize3404 {
public:
    // Direct reconstruction of CODE.1 0x3404..0x37BD.
    static void apply(CharacterNormalizeState3404& state) noexcept;
};

} // namespace ravenloft::game
