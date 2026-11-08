#include "rloft/combat_damage_stat_bonus.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>

#include "rloft/combat_hand_predicates.hpp"

namespace rloft {

namespace {

// Original CODE.2 object-2 DGROUP +0x684B; physical file
// offset 0x75F24+0x684B=0x7C76F. Used by E5F0:E736 MOVSX.
// Preserve authored bytes, including index0=7.
constexpr std::array<std::int8_t,26U> kOriginalStrengthDamage{{
     7,-4,-2,-1,-1,-1,0,0,0,0,0,0,0,
     0,0,0,1,1,2,7,8,9,10,11,12,14
}};

} // namespace

std::int32_t CombatDamageStatBonusModel::inspect(
    const PartyImage& party,
    std::size_t actor,
    std::optional<std::int8_t> observed_table_684b_entry) {
    if (actor>=PartyImage::kActorCount) {
        throw std::out_of_range("CODE.2 E6C9 actor index");
    }
    const auto offset=actor*PartyImage::kActorSize;
    const auto strength=party.bytes[offset+0x33U];
    const auto exceptional=party.bytes[offset+0x3FU];

    // E6DA..E72F: only raw strength == 18 triggers a special
    // class-list exception. Original class probes are ordered
    // 1,5,0,2 and compare SIGNED bytes +15..+17.
    if (strength==0x12U &&
        (CombatHandPredicates::has_class(party,actor,1) ||
         CombatHandPredicates::has_class(party,actor,5) ||
         CombatHandPredicates::has_class(party,actor,0) ||
         CombatHandPredicates::has_class(party,actor,2))) {
        // Original signed JG tests at E73F/E74B/E757.
        if (exceptional<=0x4BU) return 3;
        if (exceptional<=0x5AU) return 4;
        if (exceptional<=0x63U) return 5;
        return 6;
    }

    // E733..E73D: MOVSX BYTE [strength + DGROUP:0x684B].
    // Original scores 0..25 now have recovered immutable values.
    if (strength<kOriginalStrengthDamage.size()) {
        const auto original=kOriginalStrengthDamage[strength];
        if (observed_table_684b_entry &&
            *observed_table_684b_entry!=original) {
            throw std::invalid_argument(
                "CODE.2 DGROUP 684B observation contradicts original bytes");
        }
        return static_cast<std::int32_t>(original);
    }
    // Above 25 the executable reads adjacent DGROUP memory with
    // no bounds check: require an actual observed byte rather
    // than extending any guessed tabletop modifier series.
    if (!observed_table_684b_entry) {
        throw std::invalid_argument(
            "CODE.2 strength >25 requires adjacent DGROUP byte");
    }
    return static_cast<std::int32_t>(*observed_table_684b_entry);
}

} // namespace rloft
