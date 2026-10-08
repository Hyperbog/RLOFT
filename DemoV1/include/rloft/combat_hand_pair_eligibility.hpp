#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "rloft/combat_single_hand_eligibility.hpp"

namespace rloft {

// Original CODE.2 24478: ordinary two-item arbitration plus the two
// proven 244CE/24522 short-circuits (both empty and both pseudo).
// BOTH-empty requires the actual observed -1 OBJECTS pre-base byte.
// Mixed empty/pseudo/normal pairs remain unresolved, never guessed.
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
        CombatSingleHandContext context,
        std::optional<std::uint8_t> empty_prebase_category =
            std::nullopt);
};

} // namespace rloft
