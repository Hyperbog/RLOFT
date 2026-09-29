#pragma once

#include "TextDatabase.h"

#include <array>
#include <cstdint>
#include <functional>
#include <string>

namespace ravenloft::game {

struct CharacterSheetState38A4 {
    std::array<std::uint8_t, 3> fields680D{{0xFFu,0xFFu,0xFFu}};
    std::uint16_t word6810 = 0;
    std::uint8_t field6813 = 0;
    std::uint16_t word6816 = 0;
    std::array<std::uint16_t, 3> words6818{};
    std::array<std::uint8_t, 14> bytes682A{};

    // DS:7124: row selected/highlighted by the final-sheet interaction.
    std::int32_t selectedRow7124 = -1;

    // Original 0x59F8 control object. Only byte +8 (DS:5A00) is mutated here.
    std::array<std::uint8_t, 9> control59F8{};
};

struct CharacterSheetOps38A4 {
    std::function<void(std::int32_t,std::int32_t,std::int32_t,
                       std::int32_t,std::int32_t)> call6C60;
    std::function<void(const std::uint8_t*)> call7DF4;

    // CODE.1 0x7E44: draw from the supplied anchor.
    std::function<void(std::int32_t,std::int32_t,
                       const std::string&,std::int32_t)> call7E44;

    // CODE.1 0x7E04: right-aligned draw ending at the supplied X.
    std::function<void(std::int32_t,std::int32_t,
                       const std::string&,std::int32_t)> call7E04;
};

class CharacterSheet38A4 {
public:
    // Direct reconstruction of CODE.1 0x38A4..0x3C54.
    static void draw(std::int32_t x,
                     std::int32_t y,
                     CharacterSheetState38A4& state,
                     const TextDatabase& text,
                     const CharacterSheetOps38A4& ops);
};

} // namespace ravenloft::game
