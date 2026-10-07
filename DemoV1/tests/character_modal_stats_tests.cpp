#include "rloft/character_modal_stats.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

void put16(
    std::vector<std::uint8_t>& bytes,
    std::size_t offset,
    std::uint16_t value) {
    bytes[offset] =
        static_cast<std::uint8_t>(value);
    bytes[offset + 1U] =
        static_cast<std::uint8_t>(value >> 8U);
}

}  // namespace

int main() {
    constexpr std::size_t file_base = 0x75F24U;
    constexpr std::size_t table_at = 0x7D9CU;
    constexpr std::size_t words = 6U * 20U;

    std::vector<std::uint8_t> code2(
        file_base + table_at + words * 2U,
        0);

    for (std::size_t cls = 0; cls < 6U; ++cls) {
        for (std::size_t level = 0;
             level < 20U;
             ++level) {
            put16(
                code2,
                file_base + table_at +
                    (cls * 20U + level) * 2U,
                static_cast<std::uint16_t>(
                    cls * 100U + level));
        }
    }

    const auto table =
        rloft::CharacterExperienceTable::from_code2(
            code2);
    assert(table.threshold_thousands(0, 0) == 0);
    assert(table.threshold_thousands(2, 7) == 207);
    assert(table.threshold_thousands(5, 19) == 519);

    rloft::CharacterSheet sheet{};
    sheet.current_hp = 17;
    sheet.max_hp = 23;
    sheet.classes[0] =
        rloft::CharacterClassTrack{2, 7, 123456};
    sheet.classes[1] =
        rloft::CharacterClassTrack{4, 3, 222222};
    sheet.classes[2] =
        rloft::CharacterClassTrack{-1, 0, 0};

    assert(
        rloft::CharacterModalStatsModel::hit_points_line(
            sheet, "HP") ==
        "HP 17/23");

    const auto rows =
        rloft::CharacterModalStatsModel::experience_rows(
            sheet, table);
    assert(rows.size() == 2);

    // Two tracks: initial y = 145h - (2*0Bh)/2 = 13Ah.
    assert(rows[0].current_row == 0x13A);
    assert(rows[0].needed_row == 0x16F);
    assert(rows[0].class_code == 2);
    assert(rows[0].level == 7);
    assert(rows[0].experience == 123456U);
    assert(rows[0].needed_for_next ==
           207000U - 123456U);

    assert(rows[1].current_row == 0x145);
    assert(rows[1].needed_row == 0x17A);
    assert(rows[1].class_text_resource_index() ==
           0x15 + 4);
    assert(rows[1].needed_for_next ==
           403000U - 222222U);

    bool rejected = false;
    try {
        (void)table.threshold_thousands(6, 0);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected);
}
