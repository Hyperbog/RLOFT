#pragma once

#include <array>
#include <cstdint>
#include <span>

namespace ravenloft::game {

struct Code1CharacterTableRecord5D90 {
    std::uint8_t race680C = 0;
    std::array<std::uint8_t, 3> classes680D{{0xFFu, 0xFFu, 0xFFu}};

    // Record offsets 0x40..0x53 and 0x5C..0x63.
    std::array<std::uint16_t, 10> words6838{};
    std::array<std::uint16_t, 4> words6854{};
};

class Code1CharacterClassCode5D90 {
public:
    // Direct reconstruction of CODE.1 0x5D90..0x5F49.
    //
    // The original routine never initializes BL. For combinations not covered
    // by its valid-state branches, it returns the incoming BL value.
    static std::uint8_t compute(
        const std::array<std::uint8_t, 3>& classes680D,
        std::uint8_t incomingBl) noexcept;
};

struct Code1CharacterTableState5F4C {
    std::array<Code1CharacterTableRecord5D90, 2> characters{};

    // Original static table at DS:0988:
    // 15 combination codes x 6 race rows x 14 WORDs per 0x1C-byte row.
    std::array<std::uint16_t, 15 * 6 * 14> table0988{};

    // Original record block referenced by DS:7128. Records are 65 bytes.
    std::span<const std::uint8_t> recordBlock7128{};
};

class Code1CharacterTable5F4C {
public:
    // Direct reconstruction of CODE.1 0x5F4C..0x6053.
    // CODE.1 0x6054..0x605F is zero/alignment data.
    static void initialize(Code1CharacterTableState5F4C& state) noexcept;
};

} // namespace ravenloft::game
