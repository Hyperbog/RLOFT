#include "rloft/character_stats.hpp"

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace rloft {

CharacterStatTables CharacterStatTables::from_assets(
    const RavenloftBootAssets& assets) {
    CharacterStatTables tables;

    for (std::size_t i = 0; i < tables.progression.size(); ++i) {
        tables.progression[i] =
            assets.code1_u16(0x057CU + i * 2U);
    }
    for (std::size_t i = 0; i < tables.race_roll.size(); ++i) {
        tables.race_roll[i] =
            assets.code1_u8(0x082EU + i);
    }
    for (std::size_t i = 0; i < tables.race_bounds.size(); ++i) {
        tables.race_bounds[i] =
            assets.code1_u8(0x066CU + i);
    }
    for (std::size_t i = 0; i < tables.race_bonus.size(); ++i) {
        tables.race_bonus[i] = static_cast<std::int8_t>(
            assets.code1_u8(0x06B4U + i));
    }
    for (std::size_t i = 0; i < tables.class_minima.size(); ++i) {
        tables.class_minima[i] =
            assets.code1_u8(0x07D4U + i);
    }
    for (std::size_t i = 0; i < tables.hit_die.size(); ++i) {
        tables.hit_die[i] = static_cast<std::int8_t>(
            assets.code1_u8(0x0853U + i));
    }
    for (std::size_t i = 0; i < tables.hp_variant.size(); ++i) {
        tables.hp_variant[i] =
            assets.code1_s32(0x04D4U + i * 4U);
    }
    for (std::size_t i = 0; i < tables.secondary_bonus.size(); ++i) {
        tables.secondary_bonus[i] = static_cast<std::int8_t>(
            assets.code1_u8(0x083FU + i));
    }
    tables.step_mode = (assets.code1_u8(0x04D4U) & 1U) != 0U;
    return tables;
}

CharacterStatsModel::CharacterStatsModel(
    const RavenloftBootAssets& assets)
    : tables_(CharacterStatTables::from_assets(assets)) {}

CharacterStatsModel::CharacterStatsModel(
    CharacterStatTables tables)
    : tables_(std::move(tables)) {}

void CharacterStatsModel::seed(std::uint32_t value) noexcept {
    rng_state_ = value;
}

std::uint32_t CharacterStatsModel::next_rand() noexcept {
    // CODE.1 618A: Microsoft-compatible LCG.
    rng_state_ = rng_state_ * 0x41C64E6DU + 0x3039U;
    return (rng_state_ >> 16U) & 0x7FFFU;
}

int CharacterStatsModel::roll(
    int count, int scale, int add_each, int add_final) noexcept {
    std::uint32_t total = 0;
    for (int i = 0; i < count; ++i) {
        std::uint32_t value =
            next_rand() * static_cast<std::uint32_t>(scale);
        value >>= 15U;
        ++value;
        total += value;
        total += static_cast<std::uint32_t>(add_each);
    }
    total += static_cast<std::uint32_t>(add_final);
    const auto result = static_cast<std::int32_t>(total);
    return result < 1 ? 1 : result;
}

int CharacterStatsModel::class_count(
    const CharacterDraft& draft) const noexcept {
    int count = 1;
    if (draft.classes[1] != -1) count = 2;
    if (draft.classes[2] != -1) ++count;
    return count;
}

int CharacterStatsModel::hp_variant_extra(
    int cls, int variant) const {
    if (cls < 0 || cls >= 6) {
        throw std::out_of_range("CODE.1 HP class table index");
    }

    // Exact CODE.1 37C0/3404 address expression:
    //   04D4h + (class/3)*4Ch + constitution*4
    //
    // 37C0 runs before 3404 clamps the freshly rolled Constitution. 334C's
    // encoded roll can transiently yield 19..21, so those values legally
    // fall through into the following DGROUP dwords in the original image.
    // No bounds check exists in CODE.1. The 0..21 range below is the complete
    // reachable range from the original generator.
    if (variant < 0 || variant > 21) {
        throw std::out_of_range(
            "CODE.1 unreachable HP variant table index");
    }

    const std::size_t index =
        static_cast<std::size_t>(cls / 3) * 19U +
        static_cast<std::size_t>(variant);
    return tables_.hp_variant[index];
}

void CharacterStatsModel::generate(const CharacterDraft& draft) {
    if (draft.race < 0 || draft.race >= 6 ||
        draft.classes[0] < 0 || draft.classes[0] >= 6) {
        throw std::logic_error(
            "CODE.1 stat generation requires race and first class");
    }

    stats_ = {};
    stats_.secondary_6813 = 10;

    const int classes = class_count(draft);

    // function_3208.
    const int quota = 0x88BC / classes;
    for (int i = 0; i < classes; ++i) {
        const int cls = draft.classes[static_cast<std::size_t>(i)];
        if (cls < 0 || cls >= 6) {
            throw std::logic_error("invalid class in CODE.1 stat generation");
        }

        int level = 0;
        while (level < 20 &&
               quota / 1000 >=
                   tables_.progression[
                       static_cast<std::size_t>(cls) * 20U +
                       static_cast<std::size_t>(level)]) {
            ++level;
        }
        stats_.levels[static_cast<std::size_t>(i)] = level;
    }

    // function_32F4.
    const std::size_t race_roll =
        static_cast<std::size_t>(draft.race) * 3U;
    stats_.base_6810 =
        static_cast<int>(tables_.race_roll[race_roll]) +
        roll(
            tables_.race_roll[race_roll + 1U],
            tables_.race_roll[race_roll + 2U]);

    // function_334C.
    for (auto& ability : stats_.abilities) {
        ability = roll(3, 6) + 3;
    }

    if (stats_.abilities[0] >= 0x12 && draft.race != 2) {
        const int c0 = draft.classes[0];
        const int c1 = draft.classes[1];
        const int c2 = draft.classes[2];
        if (c0 == 0 || c0 == 1 || c0 == 2 ||
            c1 == 0 || c2 == 0) {
            stats_.exceptional_strength = roll(1, 100);
        }
    }

    // function_37C0.
    int hp_total = 0;
    int last_class = 0;
    for (int i = 0; i < classes; ++i) {
        last_class = draft.classes[static_cast<std::size_t>(i)];
        const int sides =
            tables_.hit_die[static_cast<std::size_t>(last_class)];
        hp_total += roll(
            stats_.levels[static_cast<std::size_t>(i)],
            sides);
    }

    int hp = hp_total / classes;
    hp += hp_variant_extra(last_class, stats_.abilities[2]);
    stats_.hit_points =
        static_cast<std::int16_t>(static_cast<std::uint16_t>(hp));

    // function_3404.
    normalize(draft);
}

void CharacterStatsModel::normalize(const CharacterDraft& draft) {
    const int race = draft.race;

    // Race ranges and signed race modifiers.
    for (int stat = 0; stat < 6; ++stat) {
        const int bonus =
            tables_.race_bonus[
                static_cast<std::size_t>(race) * 6U +
                static_cast<std::size_t>(stat)];

        const std::size_t bound =
            static_cast<std::size_t>(race) * 12U +
            static_cast<std::size_t>(stat) * 2U;
        const int lo = tables_.race_bounds[bound] + bonus;
        const int hi = tables_.race_bounds[bound + 1U] + bonus;

        stats_.abilities[static_cast<std::size_t>(stat)] =
            std::clamp(
                stats_.abilities[static_cast<std::size_t>(stat)],
                lo,
                hi);
    }

    if (stats_.abilities[0] == 0x12 &&
        stats_.exceptional_strength == 0) {
        ++stats_.exceptional_strength;
    }

    const int c0 = draft.classes[0];
    const int c1 = draft.classes[1];
    const int c2 = draft.classes[2];
    if (stats_.abilities[0] < 0x12 ||
        (c0 != 1 && c0 != 2 &&
         c1 != 2 && c2 != 2 &&
         c0 != 0 && c1 != 0 && c2 != 0)) {
        stats_.exceptional_strength = 0;
    }

    // Per-class minima at 07D4, stat stride 15.
    for (int stat = 0; stat < 6; ++stat) {
        for (int j = 0; j < 3; ++j) {
            const int cls = draft.classes[static_cast<std::size_t>(j)];
            if (cls < 0) continue;
            const int minimum =
                tables_.class_minima[
                    static_cast<std::size_t>(stat) * 15U +
                    static_cast<std::size_t>(cls)];
            stats_.abilities[static_cast<std::size_t>(stat)] =
                std::max(
                    stats_.abilities[static_cast<std::size_t>(stat)],
                    minimum);
        }
    }

    const int classes = class_count(draft);
    int aggregate = 0;
    int minimum_level = 0x63;
    int maximum_level = 0;

    for (int i = 0; i < classes; ++i) {
        const int cls = draft.classes[static_cast<std::size_t>(i)];
        int die =
            tables_.hit_die[static_cast<std::size_t>(cls)];
        if (die < 1) die = 1;

        const int level = stats_.levels[static_cast<std::size_t>(i)];
        int sum = 0;
        for (int n = 0; n < level; ++n) sum += die;
        aggregate += sum;

        minimum_level = std::min(minimum_level, level);
        maximum_level = std::max(maximum_level, level);
    }

    int hp_class = c0;
    if (c1 == 2) hp_class = 2;

    int upper = aggregate / classes;
    int extra =
        hp_variant_extra(hp_class, stats_.abilities[2]);
    if (extra < 0) extra = 0;

    const int scaled = extra * maximum_level;
    upper += scaled;
    int lower = minimum_level + scaled;

    stats_.secondary_6813 = 10;
    if (upper < lower) upper = lower;

    stats_.hit_points =
        std::clamp(stats_.hit_points, lower, upper);
    if (stats_.hit_points == 0) stats_.hit_points = 1;

    stats_.exceptional_strength =
        std::min(stats_.exceptional_strength, 100);
}

int CharacterStatsModel::secondary_display_value() const {
    const int index = stats_.abilities[1];
    if (index < 0 || index > 255) {
        throw std::out_of_range("CODE.1 secondary-stat lookup index");
    }
    if (index >= static_cast<int>(tables_.secondary_bonus.size())) {
        throw std::out_of_range("CODE.1 secondary-stat table index");
    }
    const int bonus =
        tables_.secondary_bonus[static_cast<std::size_t>(index)];
    return stats_.secondary_6813 + bonus;
}

bool CharacterStatsModel::adjust(
    const CharacterDraft& draft,
    int selection,
    bool increase) {
    if (!((selection >= 0 && selection < 6) || selection == 10)) {
        return false;
    }

    const bool step_mode = tables_.step_mode;

    if (selection < 6) {
        auto& stat = stats_.abilities[static_cast<std::size_t>(selection)];

        if (selection == 0) {
            if (increase) {
                if (step_mode) {
                    if (stat == 0x12) {
                        const int cls = draft.classes[0];
                        if (cls == 0 || cls == 1 || cls == 2) {
                            ++stats_.exceptional_strength;
                        }
                    } else {
                        ++stat;
                    }
                } else {
                    stat = 0x14;
                    stats_.exceptional_strength = 0x64;
                }
            } else {
                if (step_mode) {
                    if (stat == 0x12 &&
                        stats_.exceptional_strength != 1) {
                        --stats_.exceptional_strength;
                    } else {
                        --stat;
                    }
                } else {
                    stat = 0;
                    stats_.exceptional_strength = 0;
                }
            }
        } else {
            if (step_mode) {
                stat += increase ? 1 : -1;
            } else {
                stat = increase ? 0x14 : 0;
            }
        }
    } else {
        if (step_mode) {
            stats_.hit_points += increase ? 1 : -1;
        } else {
            stats_.hit_points = increase ? 1000 : 0;
        }
    }

    normalize(draft);
    return true;
}

}  // namespace rloft
