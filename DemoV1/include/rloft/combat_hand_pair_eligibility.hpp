#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "rloft/combat_single_hand_eligibility.hpp"

namespace rloft {

// Original CODE.2 24478: complete nonnegative two-item candidate
// arbitration (including temporary pseudo-hand objects 7Eh..86h),
// plus the both-empty short circuit when the actual original
// OBJECTS[-1] pre-base classification byte is supplied. Any mixed
// -1 + positive/pseudo pair remains unresolved until other
// original allocator-adjacent OBJECTS bytes are observed.
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
