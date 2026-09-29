#include "Code1CharacterNormalize3404.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

struct Bounds3404 {
    std::uint8_t low;
    std::uint8_t high;
};

// Exact DS:066C table: six records x six pairs.
constexpr std::array<std::array<Bounds3404, 6>, 6> kBounds066C{{
    {{{3,18},{3,18},{3,18},{3,18},{3,18},{3,18}}},
    {{{3,18},{6,18},{6,18},{4,18},{3,18},{3,18}}},
    {{{7,18},{7,18},{10,18},{6,18},{3,17},{3,18}}},
    {{{6,18},{3,18},{8,18},{6,18},{3,18},{3,18}}},
    {{{8,18},{3,17},{11,18},{3,18},{3,18},{3,17}}},
    {{{3,18},{6,18},{7,18},{8,18},{3,18},{8,18}}},
}};

// Exact signed-byte DS:06B4 table.
constexpr std::array<std::array<std::int8_t, 6>, 6> kModifiers06B4{{
    {{ 0, 0, 0, 0, 0, 0}},
    {{ 0, 0, 0, 0, 0, 0}},
    {{-1, 1, 0, 0, 0, 0}},
    {{ 0, 0, 0, 1,-1, 0}},
    {{ 0, 0, 1, 0, 0,-1}},
    {{ 0, 1,-1, 0, 0, 0}},
}};

// Exact DS:07D4 table, indexed [stat][class].
constexpr std::array<std::array<std::uint8_t, 15>, 6> kMinimums07D4{{
    {{9,12,13,0,0,0,9,9,9,0,13,0,0,9,9}},
    {{0,0,13,0,0,9,9,0,0,9,13,0,9,9,0}},
    {{0,9,14,0,0,0,0,0,0,0,14,0,0,0,0}},
    {{0,0,0,0,9,0,0,0,9,0,0,9,9,9,9}},
    {{0,13,14,9,0,0,0,9,0,9,14,9,0,0,9}},
    {{0,17,0,0,0,0,0,0,0,0,0,0,0,0,0}},
}};

// Only class IDs 0..5 are produced by the preceding creation path.
// These are the exact first six signed bytes at DS:0853.
constexpr std::array<std::int8_t, 6> kPerLevel0853{{10,10,10,8,4,6}};

// Exact signed DWORD table from DS:04D4. The assembly treats the two
// 19-DWORD groups as one flat region: group = class/3, index = byte682E.
constexpr std::array<std::int32_t, 38> kBonus04D4{{
     0,-3,-2,-2,-1,-1,-1,0,0,0,0,0,0,0,0,1,2,3,4,
     5,-3,-2,-2,-1,-1,-1,0,0,0,0,0,0,0,0,1,2,2,2
}};

std::int32_t activeClassCount(
    const std::array<std::uint8_t, 3>& classes) noexcept {
    std::int32_t count = 1;
    if (static_cast<std::int8_t>(classes[1]) != -1) count = 2;
    if (static_cast<std::int8_t>(classes[2]) != -1) ++count;
    return count;
}

} // namespace

void Code1CharacterNormalize3404::apply(
    Code1CharacterNormalizeState3404& state) noexcept {
    const auto race = static_cast<std::size_t>(state.field680C);

    // 0x341E..0x34D4: clamp six alternating bytes using race bounds plus the
    // signed modifier table.
    for (std::size_t stat = 0; stat < 6; ++stat) {
        const auto modifier = static_cast<std::int32_t>(
            kModifiers06B4[race][stat]);

        const std::int32_t low =
            static_cast<std::int32_t>(kBounds066C[race][stat].low) + modifier;
        auto& value = state.bytes682A[stat * 2u];
        if (static_cast<std::int32_t>(value) < low) {
            value = static_cast<std::uint8_t>(
                kBounds066C[race][stat].low +
                static_cast<std::uint8_t>(kModifiers06B4[race][stat]));
        }

        const std::int32_t high =
            static_cast<std::int32_t>(kBounds066C[race][stat].high) + modifier;
        if (static_cast<std::int32_t>(value) > high) {
            value = static_cast<std::uint8_t>(
                kBounds066C[race][stat].high +
                static_cast<std::uint8_t>(kModifiers06B4[race][stat]));
        }
    }

    // 0x34DA..0x356C: DS:6836 is offset 12 inside bytes682A.
    if (state.bytes682A[0] == 0x12u && state.bytes682A[12] == 0u) {
        ++state.bytes682A[12];
    }

    bool classAllowsExtra = false;
    if (state.bytes682A[0] >= 0x12u) {
        classAllowsExtra =
            state.fields680D[0] == 1u ||
            state.fields680D[0] == 2u ||
            state.fields680D[1] == 2u ||
            state.fields680D[2] == 2u ||
            state.fields680D[0] == 0u ||
            state.fields680D[1] == 0u ||
            state.fields680D[2] == 0u;
    }
    if (state.bytes682A[0] < 0x12u || !classAllowsExtra) {
        state.bytes682A[12] = 0u;
    }

    // 0x356C..0x35BD: class-specific minimums are applied independently for
    // each of the six alternating record bytes and up to three class slots.
    for (std::size_t stat = 0; stat < 6; ++stat) {
        for (std::size_t slot = 0; slot < 3; ++slot) {
            const auto classIdSigned =
                static_cast<std::int8_t>(state.fields680D[slot]);
            if (classIdSigned == -1) continue;

            const auto classId =
                static_cast<std::size_t>(
                    static_cast<std::uint8_t>(classIdSigned));
            auto& value = state.bytes682A[stat * 2u];
            const std::uint8_t minimum = kMinimums07D4[stat][classId];
            if (value < minimum) {
                value = minimum;
            }
        }
    }

    const std::int32_t count = activeClassCount(state.fields680D);

    // 0x35F6..0x3680.
    state.global711C = 0;
    state.global7118 = 0x63;

    for (std::int32_t slot = 0; slot < count; ++slot) {
        const auto index = static_cast<std::size_t>(slot);
        const auto classId = static_cast<std::size_t>(
            static_cast<std::uint8_t>(
                static_cast<std::int8_t>(state.fields680D[index])));

        std::int32_t accumulated = 0;
        const std::int32_t level =
            static_cast<std::int16_t>(state.words6818[index]);

        for (std::int32_t i = 0; i < level; ++i) {
            std::int32_t increment =
                static_cast<std::int32_t>(kPerLevel0853[classId]);
            if (increment < 1) increment = 1;
            accumulated += increment;
        }

        state.global711C += accumulated;
        if (level < state.global7118) {
            state.global7118 = level;
        }
    }

    // 0x3682..0x36A3: maximum level among active slots.
    std::int32_t maxLevel = 0;
    for (std::int32_t slot = 0; slot < count; ++slot) {
        const std::int32_t level =
            static_cast<std::int16_t>(
                state.words6818[static_cast<std::size_t>(slot)]);
        if (level > maxLevel) maxLevel = level;
    }

    std::int32_t classForBonus =
        static_cast<std::int8_t>(state.fields680D[0]);
    if (state.fields680D[1] == 2u) {
        classForBonus = 2;
    }

    // Signed IDIV by active class count.
    state.global711C /= count;

    const std::uint32_t statValue = state.bytes682A[4];
    const std::int32_t group = classForBonus / 3;
    const std::size_t bonusIndex =
        static_cast<std::size_t>(group * 19) +
        static_cast<std::size_t>(statValue);

    std::int32_t bonus = kBonus04D4[bonusIndex];
    if (bonus < 0) bonus = 0;
    bonus *= maxLevel;

    state.global711C += bonus;
    state.global7118 += bonus;

    // 0x3709..0x37B6.
    state.field6813 = 0x0Au;

    if (state.global711C < state.global7118) {
        state.global711C = state.global7118;
    }

    std::int32_t word6816 =
        static_cast<std::int16_t>(state.word6816);
    if (word6816 < state.global7118) {
        state.word6816 =
            static_cast<std::uint16_t>(state.global7118);
        word6816 = static_cast<std::int16_t>(state.word6816);
    }

    if (word6816 > state.global711C) {
        state.word6816 =
            static_cast<std::uint16_t>(state.global711C);
    }

    if (state.word6816 == 0u) {
        state.word6816 = 1u;
    }

    if (state.bytes682A[12] > 100u) {
        state.bytes682A[12] = 100u;
    }
}

} // namespace ravenloft::game
