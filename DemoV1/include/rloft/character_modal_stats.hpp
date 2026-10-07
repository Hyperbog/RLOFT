#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "rloft/character_sheet.hpp"

namespace rloft {

struct CharacterExperienceTable final {
    static constexpr std::size_t kClassCount = 6;
    static constexpr std::size_t kLevelsPerClass = 20;

    std::array<std::uint16_t,
               kClassCount * kLevelsPerClass> thousands{};

    [[nodiscard]] static CharacterExperienceTable from_code2(
        const std::vector<std::uint8_t>& code2);

    [[nodiscard]] std::uint16_t threshold_thousands(
        int class_code,
        int level) const;
};

struct CharacterExperienceRow final {
    std::int8_t class_code{-1};
    std::int16_t level{};
    std::uint32_t experience{};
    std::uint32_t needed_for_next{};

    int current_row{};
    int needed_row{};

    [[nodiscard]] int class_text_resource_index() const noexcept {
        return class_code >= 0
            ? 0x15 + static_cast<int>(class_code)
            : -1;
    }
};

class CharacterModalStatsModel final {
public:
    [[nodiscard]] static std::string hit_points_line(
        const CharacterSheet& sheet,
        const std::string& label);

    [[nodiscard]] static std::vector<CharacterExperienceRow>
    experience_rows(
        const CharacterSheet& sheet,
        const CharacterExperienceTable& table);
};

}  // namespace rloft
