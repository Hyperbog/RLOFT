#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "rloft/combat_single_hand_eligibility.hpp"

namespace rloft {

// Original CODE.2 24478: ordinary positive/pseudo item candidate
// arbitration plus empty-hand combinations ONLY when original
// OBJECTS[-1] heap-adjacent category, weight and qualifier fields
// are supplied by an observed runtime snapshot. Never manufacture
// raw -1 item bytes from OBJECTS.DAT.
// An ORIGINAL observed 24478 heap-adjacent item[-1] byte triple.
// OBJECTS_PTR-0x12 = category (+2Fh), -0x11 = weight (+30h),
// -0x0C = low 2 qualifier bits of word (+35h). These are not
// serialized OBJECTS.DAT bytes and MUST NOT be synthesized.
struct CombatEmptyHandRawFields final {
    std::uint8_t category{};
    // Both-empty branches bypass these reads entirely. For a
    // mixed -1/nonempty pair they MUST be observed, not defaulted.
    std::optional<std::uint8_t> weight;
    std::optional<std::uint8_t> qualifier_low2;
};

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
        std::optional<CombatEmptyHandRawFields> empty_raw =
            std::nullopt);
};

} // namespace rloft
