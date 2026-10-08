#include "rloft/combat_hand_pair_eligibility.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

std::uint16_t word(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8U);
}

std::uint32_t dword(const std::uint8_t* p) noexcept {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8U) |
           (static_cast<std::uint32_t>(p[2]) << 16U) |
           (static_cast<std::uint32_t>(p[3]) << 24U);
}

bool class_group_0251(const PartyImage& party, std::size_t actor) {
    // 24478 24765/24880/24959: signed class OR in original
    // order 0 -> 2 -> 5 -> 1; 23EAC tests three class lanes.
    return CombatHandPredicates::has_class(party,actor,0) ||
           CombatHandPredicates::has_class(party,actor,2) ||
           CombatHandPredicates::has_class(party,actor,5) ||
           CombatHandPredicates::has_class(party,actor,1);
}

} // namespace

CombatHandPairEligibility CombatHandPairEligibilityModel::evaluate(
    const PartyImage& party,
    const ObjectCatalog& objects,
    std::size_t actor,
    std::size_t selected_hand,
    CombatSingleHandContext context) {
    if (actor >= PartyImage::kActorCount || selected_hand >= 2U) {
        throw std::out_of_range("CODE.2 24478 actor/hand");
    }
    const auto base=actor*PartyImage::kActorSize;
    const auto* rec=party.bytes.data()+base;
    const auto first_id=
        static_cast<std::int16_t>(word(rec+0x40U+14U*2U));
    const auto second_id=
        static_cast<std::int16_t>(word(rec+0x40U+15U*2U));

    // This bounded 24478 slice excludes original pseudo-hand special
    // branches 24522,24641/24693 and their 23F34 classifier, as well
    // as OBJECTS[-1] raw pre-base memory for an empty hand.
    if (first_id < 0 || second_id < 0 ||
        (first_id>=0x7E && first_id<=0x86) ||
        (second_id>=0x7E && second_id<=0x86)) {
        throw std::invalid_argument(
            "CODE.2 24478 empty/pseudo hand requires separate branch");
    }

    const auto& a=objects.at(static_cast<std::size_t>(first_id));
    const auto& b=objects.at(static_cast<std::size_t>(second_id));
    const auto initial_a=CombatSingleHandEligibilityModel::evaluate(
        party,objects,actor,first_id,context);
    const auto initial_b=CombatSingleHandEligibilityModel::evaluate(
        party,objects,actor,second_id,context);

    CombatHandPairEligibility result{};
    result.first_classified=initial_a.classified;
    result.second_classified=initial_b.classified;
    result.first_eligible=initial_a.eligible;
    result.second_eligible=initial_b.eligible;

    // 3459C..345B9: the original status mask clears both candidates.
    if ((party.status_word(actor) & 0x7240U) != 0U) {
        result.first_eligible=false;
        result.second_eligible=false;
    }

    // 346E9..34A56: arbitrate classification flags in the
    // exact CODE.2 branch order. These operations only suppress
    // candidates; they never grant a new eligibility result.
    if (result.first_classified && result.second_classified) {
        const auto qual_a=a.raw_byte_35_low2;
        const auto qual_b=b.raw_byte_35_low2;
        const auto weight_a=a.weight_half_units;
        const auto weight_b=b.weight_half_units;

        if (qual_a==qual_b) {
            if (qual_a==2U) {
                // 3474B: matching special-qualifier=2 vetoes BOTH.
                result.first_eligible=false;
                result.second_eligible=false;
            } else if (!(class_group_0251(party,actor) &&
                         CombatHandPredicates::is_pair_exception(first_id) &&
                         CombatHandPredicates::is_pair_exception(second_id))) {
                // 347C2..34857: lighter object wins, equal favors
                // the first hand regardless of class-group branch.
                if (weight_b < weight_a)
                    result.first_eligible=false;
                else
                    result.second_eligible=false;
            }
        } else {
            if (qual_a==2U || qual_b==2U) {
                // 34862/3493B: any qualifier 2 in unequal pair
                // vetoes both.
                result.first_eligible=false;
                result.second_eligible=false;
            } else {
                const bool grouped=class_group_0251(party,actor);
                const bool both_exceptions =
                    CombatHandPredicates::is_pair_exception(first_id) &&
                    CombatHandPredicates::is_pair_exception(second_id);
                if (!(grouped && both_exceptions)) {
                    if (qual_a>qual_b) {
                        if (grouped) {
                            // 348DD..34906: only suppress second
                            // when its weight is >= first; otherwise
                            // the original allows both candidates.
                            if (weight_b>=weight_a)
                                result.second_eligible=false;
                        } else {
                            if (weight_b<weight_a)
                                result.first_eligible=false;
                            else
                                result.second_eligible=false;
                        }
                    } else {
                        if (grouped) {
                            // 349B6..349DF: only equal weights
                            // suppress first. Distinct weights
                            // leave both candidates unchanged.
                            if (weight_a==weight_b)
                                result.first_eligible=false;
                        } else {
                            if (weight_a<weight_b)
                                result.first_eligible=false;
                            else
                                result.second_eligible=false;
                        }
                    }
                }
            }
        }
    } else if (result.first_classified) {
        // 34A0A..34A2E: only first classified; +35 qualifier
        // exactly 2 suppresses the OTHER hand.
        if (a.raw_byte_35_low2==2U)
            result.second_eligible=false;
    } else if (result.second_classified) {
        // 34A30..34A56: only second classified, same rule
        // in the opposite direction.
        if (b.raw_byte_35_low2==2U)
            result.first_eligible=false;
    }

    // 24A5E/24A6F: individual 32-bit action-state busy vetoes
    // at original actor offsets +10B and +10F, NOT hand ready
    // deadlines +19E/+1A2. These are the final candidate gates.
    if (dword(rec+0x10BU)!=0U)
        result.first_eligible=false;
    if (dword(rec+0x10FU)!=0U)
        result.second_eligible=false;

    result.selected_eligible=selected_hand==0U
        ? result.first_eligible : result.second_eligible;
    return result;
}

} // namespace rloft
