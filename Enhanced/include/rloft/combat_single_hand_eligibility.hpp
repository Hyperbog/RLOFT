#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

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
    // For -1 the original 23E58/23F50 read OBJECTS base -0x12
    // (category byte at record[-1]+0x2F). The caller must supply
    // this RAW loaded-memory byte when known from an original trace.
    // Absent evidence -> reject; do not invent unarmed eligibility.
    [[nodiscard]] static CombatSingleHandEligibility evaluate(
        const PartyImage& party,
        const ObjectCatalog& objects,
        std::size_t actor,
        std::int32_t item_id,
        CombatSingleHandContext context,
        std::optional<std::uint8_t> empty_prebase_category =
            std::nullopt);
};

} // namespace rloft
