#pragma once

#include "CharacterClassId5D90.h"

#include <array>
#include <cstdint>
#include <span>

namespace ravenloft::game {

struct CharacterTableRecord5F4C {
    std::uint8_t field680C = 0;
    std::array<std::uint8_t, 3> fields680D{{0xFFu, 0xFFu, 0xFFu}};

    // Character-record WORDs 6838..684A.
    std::array<std::int16_t, 10> words6838{};

    std::int16_t word6854 = 0;
    std::int16_t word6856 = 0;
    std::int16_t word6858 = 0;
    std::int16_t word685A = 0;
};

class CharacterTable5F4C {
public:
    // Direct reconstruction of CODE.1 0x5F4C..0x6053.
    //
    // table0988 is the original 15 * 6 * 0x1C-byte data table beginning at
    // CODE.1 data offset 0x0988. It remains external game data rather than
    // being copied into the source repository.
    //
    // recordBlock7128 is the original 65-byte-record block loaded by 0x4D54.
    // CODE.1 0x6054..0x605F is twelve bytes of zero padding.
    static void apply(
        std::array<CharacterTableRecord5F4C, 2>& characters,
        std::span<const std::uint8_t> table0988,
        std::span<const std::uint8_t> recordBlock7128) noexcept;
};

} // namespace ravenloft::game
