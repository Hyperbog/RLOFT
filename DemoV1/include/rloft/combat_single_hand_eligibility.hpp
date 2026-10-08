#pragma once

#include <cstddef>
#include <cstdint>

#include "rloft/combat_hand_predicates.hpp"
#include "rloft/exploration_state.hpp"
#include "rloft/object_catalog.hpp"

namespace rloft {

// Original CODE.2 240A8 computes one hand's initial eligibility and
// writes its "attack/special class" predicate through an out pointer.
// 24478 later combines the results for BOTH hands. This output alone
// is never authorization to start an attack.
struct CombatSingleHandEligibility final {
    bool eligible{};
    bool classified{};
};

// Explicit original global at DGROUP 258A8; exact 0xFF matters.
struct CombatSingleHandContext final {
    std::uint8_t party_mode_258a8{};
};

class CombatSingleHandEligibilityModel final {
public:
    // Exact nonnegative-item 240A8 branch model. Original 23E58/23F50
    // access raw object memory even for signed item -1; until the
    // actual pointer-before-OBJECTS context is modeled, reject -1
    // instead of inventing a normal unarmed result.
    [[nodiscard]] static CombatSingleHandEligibility evaluate(
        const PartyImage& party,
        const ObjectCatalog& objects,
        std::size_t actor,
        std::int32_t item_id,
        CombatSingleHandContext context);
};

} // namespace rloft
