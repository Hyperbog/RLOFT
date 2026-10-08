#include "rloft/combat_hand_pair_eligibility.hpp"
#include "rloft/combat_hand_pair_early_gate.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
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
    CombatSingleHandContext context,
    std::optional<CombatEmptyHandRawFields> empty_raw) {
    if (actor >= PartyImage::kActorCount || selected_hand >= 2U) {
        throw std::out_of_range("CODE.2 24478 actor/hand");
    }
    const auto base=actor*PartyImage::kActorSize;
    const auto* rec=party.bytes.data()+base;
    const auto first_id=
        static_cast<std::int16_t>(word(rec+0x40U+14U*2U));
    const auto second_id=
        static_cast<std::int16_t>(word(rec+0x40U+15U*2U));

    const auto pseudo=[](std::int32_t id) noexcept {
        return id>=0x7E && id<=0x86;
    };
    const bool both_empty=first_id==-1 && second_id==-1;
    const bool both_pseudo=pseudo(first_id) && pseudo(second_id);
    // The original has no -1 record inside OBJECTS.DAT.
    // Mixed -1/nonempty branches at 246E9 may read its category,
    // weight and qualifier from allocator-adjacent memory.
    // Require a caller-observed raw snapshot for ANY -1 hand;
    // do not fill missing bytes with a hypothetical unarmed item.
    if ((first_id==-1 || second_id==-1) && !empty_raw) {
        throw std::invalid_argument(
            "CODE.2 24478 requires observed raw OBJECTS[-1] fields");
    }
    if (empty_raw && empty_raw->qualifier_low2>3U) {
        throw std::out_of_range(
            "CODE.2 OBJECTS[-1] qualifier must be masked to two bits");
    }
    const auto prebase_category=empty_raw
        ? std::optional<std::uint8_t>(empty_raw->category)
        : std::nullopt;
    const auto initial_a=CombatSingleHandEligibilityModel::evaluate(
        party,objects,actor,first_id,context,
        prebase_category);
    const auto initial_b=CombatSingleHandEligibilityModel::evaluate(
        party,objects,actor,second_id,context,
        prebase_category);

    CombatHandPairEligibility result{};
    result.first_classified=initial_a.classified;
    result.second_classified=initial_b.classified;
    result.first_eligible=initial_a.eligible;
    result.second_eligible=initial_b.eligible;

    // 244CE..246E9 is a common early-gate stage. The supported
    // ordinary two-positive-item subset reaches only its 2459C
    // actor-status branch, but call the shared audited stage so
    // the mask is not implemented differently in separate paths.
    const auto early=CombatHandPairEarlyGateModel::apply(
        party,actor,first_id,second_id,context.party_mode_258a8,
        result.first_eligible,result.second_eligible);
    result.first_eligible=early.first_eligible;
    result.second_eligible=early.second_eligible;

    if (early.skip_object_arbitration) {
        // 244CE or 24522 jumps directly to 24A58. Never read the
        // fictional OBJECTS[-1] qualifier/weight or discard the
        // paired-pseudo candidate override. Hand busy vetoes still
        // execute on both early-return routes.
        if (dword(rec+0x10BU)!=0U)
            result.first_eligible=false;
        if (dword(rec+0x10FU)!=0U)
            result.second_eligible=false;
        result.selected_eligible=selected_hand==0U
            ? result.first_eligible : result.second_eligible;
        return result;
    }

    // Preserve the same field-level semantics for valid item IDs
    // and original raw heap reads for sentinel -1. These objects
    // only carry the two item properties read by 246E9..24A56;
    // no invented pre-OBJECTS record is written into game state.
    ObjectDefinition raw_first{};
    ObjectDefinition raw_second{};
    if (first_id==-1) {
        raw_first.weight_half_units=empty_raw->weight;
        raw_first.raw_byte_35_low2=empty_raw->qualifier_low2;
    }
    if (second_id==-1) {
        raw_second.weight_half_units=empty_raw->weight;
        raw_second.raw_byte_35_low2=empty_raw->qualifier_low2;
    }
    const auto& a=first_id==-1
        ? raw_first
        : objects.at(static_cast<std::size_t>(first_id));
    const auto& b=second_id==-1
        ? raw_second
        : objects.at(static_cast<std::size_t>(second_id));

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
