#include "rloft/character_transfer.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>

namespace {

std::uint16_t u16(
    const rloft::CharacterTransferImage& image,
    std::size_t at) {
    return static_cast<std::uint16_t>(image[at]) |
           (static_cast<std::uint16_t>(image[at + 1U]) << 8U);
}

rloft::CharacterTransferActor actor(
    int index,
    const char* name) {
    rloft::CharacterTransferActor out;
    out.draft.actor = index;
    out.draft.sex = index;
    out.draft.race = 2;
    out.draft.classes = {{0, -1, -1}};
    out.draft.option = index;
    out.draft.face = 4 + index;
    out.draft.name = name;

    out.stats.abilities = {{18, 17, 16, 15, 14, 13}};
    out.stats.levels = {{7, 0, 0}};
    out.stats.exceptional_strength = 55 + index;
    out.stats.base_6810 = 42 + index;
    out.stats.secondary_6813 = 10;
    out.stats.hit_points = 38 + index;
    return out;
}

rloft::CharacterTransferActor actor_with_classes(
    int index,
    int race,
    std::array<int, 3> classes) {
    auto out = actor(index, index == 0 ? "CLASS0" : "CLASS1");
    out.draft.race = race;
    out.draft.classes = classes;
    return out;
}

std::uint16_t family_word(
    std::size_t family,
    std::size_t race,
    std::size_t word) {
    return static_cast<std::uint16_t>(
        family * 1000U + race * 100U + word);
}

}  // namespace

int main() {
    rloft::CharacterTransferTables tables;

    // Both actors are single class 0/race 2, so function_5D90 resolves family
    // zero for both and the conflict pass applies to actor 1.
    for (std::size_t word = 0; word < 14U; ++word) {
        tables.family_race_words[0][2][word] =
            static_cast<std::uint16_t>(100U + word);
    }

    // The ten +40h words are source words 4..13. Make source word 7 point
    // at OBJECTS.DAT record 3 and flag that record as conflicting.
    tables.family_race_words[0][2][7] = 3;
    tables.object_conflict_flag.resize(16, 0);
    tables.object_conflict_flag[3] = 1;

    const std::array<rloft::CharacterTransferActor, 2> actors{{
        actor(0, "ONE"),
        actor(1, "TWO")
    }};

    const auto image =
        rloft::build_character_transfer(actors, tables);
    static_assert(
        std::tuple_size<rloft::CharacterTransferImage>::value == 0x920);

    constexpr std::size_t stride = 0x248U;
    for (std::size_t a = 0; a < 2U; ++a) {
        const auto base = a * stride;
        assert(image[base + 0] == (a == 0 ? 'O' : 'T'));
        assert(image[base + 3] == 0);
        assert(u16(image, base + 0x10U) == 0);
        assert(image[base + 0x12U] == 4U + a);
        assert(image[base + 0x13U] == a);
        assert(image[base + 0x14U] == 2);
        assert(image[base + 0x15U] == 0);
        assert(image[base + 0x16U] == 0xFF);
        assert(image[base + 0x17U] == 0xFF);
        assert(u16(image, base + 0x18U) == 42U + a);
        assert(image[base + 0x1AU] == a);
        assert(image[base + 0x1BU] == 10);
        assert(u16(image, base + 0x1CU) == 38U + a);
        assert(u16(image, base + 0x1EU) == 38U + a);
        assert(u16(image, base + 0x20U) == 7);
        assert(image[base + 0x32U] == 18);
        assert(image[base + 0x33U] == 18);
        assert(image[base + 0x3EU] == 55U + a);
        assert(image[base + 0x3FU] == 55U + a);

        // 4EF0 zeroes the tail and then writes class flags at +131.
        for (std::size_t q = 0x10BU; q < stride; ++q) {
            if (q == 0x131U) continue;
            assert(image[base + q] == 0);
        }
        assert(image[base + 0x131U] == 0);

        // 5F4C q4 reorder: source words 2,3,1,0.
        assert(u16(image, base + 0x5CU) == 102);
        assert(u16(image, base + 0x5EU) == 103);
        assert(u16(image, base + 0x60U) == 101);
        assert(u16(image, base + 0x62U) == 100);
    }

    // Source word 7 is fourth of the ten runtime words.
    assert(u16(image, 0x40U + 3U * 2U) == 3);
    assert(u16(image, stride + 0x40U + 3U * 2U) == 0xFFFF);

    // Records 2/3 remain FFh except the four dwords cleared by 4DC4.
    for (std::size_t a = 2; a < 4U; ++a) {
        const auto base = a * stride;
        assert(image[base] == 0xFF);
        assert(image[base + 0x10U] == 0xFF);
        for (std::size_t q = 0x10BU; q < 0x11BU; ++q) {
            assert(image[base + q] == 0);
        }
        assert(image[base + 0x11BU] == 0xFF);
    }


    // CODE.1 5D90/5F4C class-family regression. Each family/race record gets
    // unique words so a wrong family selection or slot mapping is visible.
    rloft::CharacterTransferTables family_tables;
    family_tables.object_conflict_flag.resize(512, 0);
    for (std::size_t family = 0; family < 15U; ++family) {
        for (std::size_t race = 0; race < 6U; ++race) {
            for (std::size_t word = 0; word < 14U; ++word) {
                family_tables.family_race_words[family][race][word] =
                    family_word(family, race, word);
            }
        }
    }

    struct ClassFamilyCase {
        std::array<int, 3> classes;
        std::size_t family;
    };
    const std::array<ClassFamilyCase, 15> family_cases{{
        {{{0, -1, -1}}, 0},  // Fighter
        {{{1, -1, -1}}, 1},  // Paladin
        {{{2, -1, -1}}, 2},  // Ranger
        {{{3, -1, -1}}, 3},  // Cleric
        {{{4, -1, -1}}, 4},  // Mage
        {{{5, -1, -1}}, 5},  // Thief
        {{{0, 5, -1}}, 6},   // Fighter/Thief
        {{{0, 3, -1}}, 7},   // Fighter/Cleric
        {{{0, 4, -1}}, 8},   // Fighter/Mage
        {{{3, 5, -1}}, 9},   // Cleric/Thief
        {{{2, 3, -1}}, 10},  // Ranger/Cleric
        {{{3, 4, -1}}, 11},  // Cleric/Mage
        {{{4, 5, -1}}, 12},  // Mage/Thief
        {{{0, 4, 5}}, 13},   // Fighter/Mage/Thief
        {{{0, 4, 3}}, 14},   // Fighter/Mage/Cleric
    }};

    constexpr std::size_t inventory_at = 0x40U;
    for (const auto& test_case : family_cases) {
        std::array<rloft::CharacterTransferActor, 2> class_actors{{
            actor_with_classes(0, 2, test_case.classes),
            actor_with_classes(1, 5, {{1, -1, -1}})
        }};
        const auto class_image =
            rloft::build_character_transfer(
                class_actors,
                family_tables);

        // item[0..9] <- source words 4..13.
        for (std::size_t slot = 0; slot < 10U; ++slot) {
            assert(
                u16(class_image, inventory_at + slot * 2U) ==
                family_word(
                    test_case.family,
                    2,
                    4U + slot));
        }

        // item[10..13] are not written by 5F4C and retain FFh.
        for (std::size_t slot = 10U; slot <= 13U; ++slot) {
            assert(
                u16(class_image, inventory_at + slot * 2U) ==
                0xFFFFU);
        }

        // Equipped item[14..17] use q4 order source words 2,3,1,0.
        constexpr std::array<std::size_t, 4> equipped_words{{
            2, 3, 1, 0
        }};
        for (std::size_t i = 0; i < equipped_words.size(); ++i) {
            assert(
                u16(
                    class_image,
                    inventory_at + (14U + i) * 2U) ==
                family_word(
                    test_case.family,
                    2,
                    equipped_words[i]));
        }

        // item[18..20] likewise remain empty at creation.
        for (std::size_t slot = 18U; slot <= 20U; ++slot) {
            assert(
                u16(class_image, inventory_at + slot * 2U) ==
                0xFFFFU);
        }
    }

    std::cout << "character_transfer_tests: PASS\n";
}
