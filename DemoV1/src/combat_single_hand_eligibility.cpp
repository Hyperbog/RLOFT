#include "rloft/combat_single_hand_eligibility.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>

namespace rloft {
namespace {

bool effect_special(std::int32_t id,
                    std::uint8_t category) noexcept {
    // CODE.2 23F50: different list from 23EE4 (pair exceptions).
    switch (id) {
        case 0x2E: case 0x2F: case 0x41:
        case 0x57: case 0x59: case 0x5A:
            return true;
        default:
            return category == 0x0FU || category == 0x0DU;
    }
}

bool test_34060(std::int32_t id) noexcept {
    if (id >= 0x7E && id <= 0x86) return true;
    if (CombatHandPredicates::is_pair_exception(id)) return true;
    switch (id) {
        case 0x31: case 0x5B: case 0x26:
        case 0xF8: case 0xF9:
            return true;
        default:
            return false;
    }
}

bool test_33fc4(std::int32_t id,
                 std::uint8_t category) noexcept {
    if (id >= 0x7E && id <= 0x86) return true;
    if (effect_special(id, category) ||
        CombatHandPredicates::is_pair_exception(id)) return true;
    switch (id) {
        case 0x30: case 0x29: case 0xFE: case 0xFF:
        case 0x28: case 0xFC: case 0xFD: case 0x27:
        case 0xFA: case 0xFB: case 0x26: case 0xF8:
        case 0xF9:
            return true;
        default:
            return false;
    }
}

std::int16_t class_level(const PartyImage& party,
                         std::size_t actor,
                         std::size_t index) noexcept {
    const auto pos=actor*PartyImage::kActorSize+0x20U+index*2U;
    const auto word=static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(party.bytes[pos]) |
        (static_cast<std::uint16_t>(party.bytes[pos+1U]) << 8U));
    return static_cast<std::int16_t>(word);
}

bool has_class_level_at_least_ten(const PartyImage& party,
                                 std::size_t actor) noexcept {
    const auto base=actor*PartyImage::kActorSize;
    for (std::size_t index=0U; index<3U; ++index) {
        const auto code=static_cast<std::int8_t>(
            party.bytes[base+0x15U+index]);
        if (code==5 && class_level(party,actor,index)>=10) {
            return true;
        }
    }
    return false;
}

} // namespace

CombatSingleHandEligibility
CombatSingleHandEligibilityModel::evaluate(
    const PartyImage& party,
    const ObjectCatalog& objects,
    std::size_t actor,
    std::int32_t item_id,
    CombatSingleHandContext context,
    std::optional<std::uint8_t> empty_prebase_category) {
    if (actor >= PartyImage::kActorCount) {
        throw std::out_of_range("CODE.2 240A8 actor index");
    }
    if (item_id < -1) {
        throw std::out_of_range("CODE.2 240A8 invalid negative item");
    }
    if (item_id == -1 && !empty_prebase_category) {
        throw std::invalid_argument(
            "CODE.2 240A8 requires actual OBJECTS base-0x12 byte");
    }

    // 23E58(-1) and 23F50(-1) do NOT read an abstract unarmed
    // category: both calculate (signed item * 0x41) + OBJECTS+0x2F.
    // Their resulting byte for -1 is physically OBJECTS base-0x12.
    const auto category = item_id == -1
        ? *empty_prebase_category
        : objects.at(static_cast<std::size_t>(item_id)).item_category;
    const bool special=effect_special(item_id,category);
    const bool classified = item_id == -1
        ? (category == 9U || category == 10U ||
           category == 11U || special)
        : (CombatHandPredicates::is_attack_hand_item(item_id,objects)
           || special);
    bool eligible=true;

    if (item_id == -1) {
        // 240E3 jumps over all class/OBJECTS body logic directly to
        // 2420D when classified. The 24248 -1 guard then clears
        // eligibility only under its exact party-mode and status gate.
        // When unclassified, 2428A..243D7 leaves initial true intact.
        if (classified &&
            context.party_mode_258a8 == 0xFFU &&
            actor >= 2U &&
            !(party.status_word(0U)==0x8000U &&
              party.status_word(1U)==0x8000U)) {
            eligible=false;
        }
        return {eligible,classified};
    }

    const auto& object=objects.at(
        static_cast<std::size_t>(item_id));

    if (classified) {
        // CODE.2 240D9..2420B. All original class-code comparisons
        // use 23EAC, testing signed actor bytes +15/+16/+17.
        const bool class0=
            CombatHandPredicates::has_class(party,actor,0);
        const bool class1=
            CombatHandPredicates::has_class(party,actor,1);
        const bool class3=
            CombatHandPredicates::has_class(party,actor,3);
        const bool class4=
            CombatHandPredicates::has_class(party,actor,4);
        const bool class5=
            CombatHandPredicates::has_class(party,actor,5);
        if (class4) {
            if (!class0 && !class5 && !class1) {
                eligible=test_34060(item_id);
            }
            if (class5 && !class0 && !class1) {
                // 34179..3418F: OR, not AND, the two lists.
                eligible=test_34060(item_id) ||
                         test_33fc4(item_id,category);
            }
        } else if (class5 && !class0 && !class1 && !class3) {
            eligible=test_33fc4(item_id,category);
        }
        // 341E5..3420B: class3 with category 10 vetoes hand.
        if (class3 && category==0x0AU) eligible=false;
    } else {
        // CODE.2 2428A..243D7 has special spell/item ranges.
        if (item_id>=0xA7 && item_id<=0xC9) {
            eligible=
                CombatHandPredicates::has_class(party,actor,4) ||
                has_class_level_at_least_ten(party,actor);
        } else if ((item_id>=0xCA && item_id<=0xE4) ||
                   item_id==0x12D) {
            eligible=
                CombatHandPredicates::has_class(party,actor,3) ||
                has_class_level_at_least_ten(party,actor);
        }
    }

    // 2420D..24281 is reached ONLY by the classified branch;
    // normal item-range tests return from 243D7 earlier.
    if (classified &&
        context.party_mode_258a8==0xFFU &&
        actor>=2U &&
        !(party.status_word(0U)==0x8000U &&
          party.status_word(1U)==0x8000U)) {
        // The source compares (item record+35h & 3) to 2.
        // 23F50 is the alternative allowed special predicate.
        eligible=eligible &&
            (object.raw_byte_35_low2==2U || special);
    }

    return {eligible,classified};
}

} // namespace rloft
