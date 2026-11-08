#include "rloft/character_modal_stats.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

constexpr std::size_t kCode2DataFileBase = 0x75F24U;
constexpr std::size_t kExperienceTableDgroup = 0x7D9CU;

std::uint16_t u16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8U);
}

}  // namespace

CharacterExperienceTable
CharacterExperienceTable::from_code2(
    const std::vector<std::uint8_t>& code2) {
    constexpr std::size_t kWords =
        kClassCount * kLevelsPerClass;
    constexpr std::size_t kBytes =
        kWords * sizeof(std::uint16_t);
    const auto start =
        kCode2DataFileBase +
        kExperienceTableDgroup;

    if (start > code2.size() ||
        kBytes > code2.size() - start) {
        throw std::runtime_error(
            "CODE.2 is truncated before class XP thresholds");
    }

    CharacterExperienceTable out{};
    for (std::size_t i = 0; i < kWords; ++i) {
        out.thousands[i] =
            u16(code2.data() + start + i * 2U);
    }
    return out;
}

std::uint16_t CharacterExperienceTable::threshold_thousands(
    int class_code,
    int level) const {
    if (class_code < 0 ||
        class_code >= static_cast<int>(kClassCount) ||
        level < 0 ||
        level >= static_cast<int>(kLevelsPerClass)) {
        throw std::out_of_range(
            "character XP threshold index");
    }

    return thousands[
        static_cast<std::size_t>(class_code) *
            kLevelsPerClass +
        static_cast<std::size_t>(level)];
}

std::string CharacterModalStatsModel::hit_points_line(
    const CharacterSheet& sheet,
    const std::string& label) {
    return label + " " +
           std::to_string(sheet.current_hp) +
           "/" +
           std::to_string(sheet.max_hp);
}

std::vector<CharacterExperienceRow>
CharacterModalStatsModel::experience_rows(
    const CharacterSheet& sheet,
    const CharacterExperienceTable& table) {
    const auto count =
        sheet.class_track_count();

    int row =
        0x145 -
        static_cast<int>(
            (count * 0x0BU) / 2U);
    int needed_row = row + 0x35;

    std::vector<CharacterExperienceRow> out;
    out.reserve(count);

    for (const auto& track : sheet.classes) {
        if (!track.present()) continue;

        const auto threshold =
            static_cast<std::uint32_t>(
                table.threshold_thousands(
                    static_cast<int>(track.class_code),
                    static_cast<int>(track.level))) *
            1000U;
        const auto experience =
            static_cast<std::uint32_t>(
                track.progress);
        const auto needed =
            threshold - experience;

        out.push_back(
            CharacterExperienceRow{
                track.class_code,
                track.level,
                experience,
                needed,
                row,
                needed_row,
            });

        row += 0x0B;
        needed_row += 0x0B;
    }

    return out;
}

}  // namespace rloft
