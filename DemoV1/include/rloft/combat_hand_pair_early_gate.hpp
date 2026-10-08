#pragma once

#include <cstddef>
#include <cstdint>

#include "rloft/exploration_state.hpp"

namespace rloft {

// Exact CODE.2 244CE..246E9 *pre-arbitration* candidate modifications.
// Inputs are two 240A8 eligibility results; classifying or loading the
// original item[-1] heap-adjacent bytes is NOT this stage's job.
struct CombatHandPairEarlyGateResult final {
    bool first_eligible{};
    bool second_eligible{};
    // Both-empty and paired-pseudo branches jump directly to 24A58;
    // the caller MUST skip object qualifier/weight arbitration.
    bool skip_object_arbitration{};
};

class CombatHandPairEarlyGateModel final {
public:
    [[nodiscard]] static CombatHandPairEarlyGateResult apply(
        const PartyImage& party,
        std::size_t actor,
        std::int32_t first_item,
        std::int32_t second_item,
        std::uint8_t party_mode_258a8,
        bool first_initial_eligible,
        bool second_initial_eligible);
};

} // namespace rloft
