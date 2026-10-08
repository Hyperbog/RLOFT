#pragma once

#include <cstddef>
#include <cstdint>

#include "rloft/combat_single_hand_eligibility.hpp"

namespace rloft {

// Original CODE.2 24478 ordinary two-hand pair arbitration. Both item
// IDs must be nonnegative and outside the temporary 7Eh..86h pseudo
// spell-hand range. Original -1/raw pre-OBJECTS and pseudo branches
// are separate and are not guessed by this restricted implementation.
struct CombatHandPairEligibility final {
    bool first_eligible{};
    bool second_eligible{};
    bool first_classified{};
    bool second_classified{};
    bool selected_eligible{};
    // The original 24478 result is a prior condition for 26018;
    // this must never be equated with permission to damage an NPC.
    bool requires_action_dispatch{true};
};

class CombatHandPairEligibilityModel final {
public:
    [[nodiscard]] static CombatHandPairEligibility evaluate(
        const PartyImage& party,
        const ObjectCatalog& objects,
        std::size_t actor,
        std::size_t selected_hand,
        CombatSingleHandContext context);
};

} // namespace rloft
