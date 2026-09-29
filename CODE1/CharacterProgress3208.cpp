#include "CharacterProgress3208.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

// Exact 6 x 20 WORD table from CODE.1 data object DS:057C.
constexpr std::array<std::array<std::uint16_t, 20>, 6> kProgress057C{{
    {{0,2,4,8,16,32,64,125,250,500,750,1000,1250,1500,1750,2000,2250,2500,2750,3000}},
    {{0,2,4,9,18,36,75,150,300,600,900,1200,1500,1800,2100,2400,2700,3000,3300,3600}},
    {{0,2,4,9,18,36,75,150,300,600,900,1200,1500,1800,2100,2400,2700,3000,3300,3600}},
    {{0,1,3,6,13,27,55,110,225,450,675,900,1125,1350,1575,1800,2025,2055,2475,2700}},
    {{0,2,5,10,20,40,60,90,135,250,375,750,1125,1500,1875,2250,2625,3000,3375,3750}},
    {{0,1,2,5,10,20,40,70,110,160,220,440,660,880,1100,1320,1540,1760,1980,2200}},
}};

} // namespace

void CharacterProgress3208::initialize(
    CharacterProgressState3208& state) noexcept {
    // 0x3211..0x3248: one active slot is mandatory; the two trailing
    // 0xFF-sentinel fields increase the class count when present.
    std::int32_t count = 1;
    if (static_cast<std::int8_t>(state.fields680D[1]) != -1) {
        count = 2;
    }
    if (static_cast<std::int8_t>(state.fields680D[2]) != -1) {
        ++count;
    }

    // 0x3248..0x32E6.
    for (std::int32_t slot = 0; slot < count; ++slot) {
        const auto index = static_cast<std::size_t>(slot);

        // Signed IDIV of positive constants in the original.
        state.dwords681E[index] =
            static_cast<std::uint32_t>(0x88BC / count);

        const auto classId = static_cast<std::int8_t>(state.fields680D[index]);
        const auto tableIndex = static_cast<std::size_t>(
            static_cast<std::uint8_t>(classId));

        std::uint16_t levelIndex = 0;
        for (;;) {
            const std::uint16_t threshold =
                kProgress057C[tableIndex][levelIndex];

            // 0x32B7..0x32C6 uses unsigned DIV by 1000.
            const std::uint32_t units = state.dwords681E[index] / 1000u;
            if (units < threshold) {
                break;
            }

            ++levelIndex;
            if (levelIndex >= 20u) {
                break;
            }
        }

        state.words6818[index] = levelIndex;
    }
}

} // namespace ravenloft::game
