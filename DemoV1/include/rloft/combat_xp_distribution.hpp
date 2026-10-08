#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

#include "rloft/exploration_state.hpp"

namespace rloft {

// Original CODE.2 logical DDA4..DFC0: pre-level-change XP arithmetic.
// Each entry is a read-only projection. This model deliberately DOES NOT
// invoke the 7D9C class threshold table, 50DC/DC7C level-change services,
// or DBD4's derived attack-target recomputation.
struct CombatXpActorShare final {
    bool eligible{};
    std::uint8_t class_track_count{};
    std::uint32_t per_track_award{};
    // Only [0, class_track_count) are touched by original DDA4's loop.
    // Preserve actual 32-bit wrapping ADD to each class-progress dword.
    std::array<std::uint32_t,3> projected_track_progress{};
};

struct CombatXpDistributionPlan final {
    std::uint16_t definition_award{};
    std::uint8_t eligible_party_members{};
    std::uint32_t rounded_party_share{};
    std::array<CombatXpActorShare,PartyImage::kActorCount> actors{};
};

class CombatXpDistributionModel final {
public:
    // Empty eligible party would execute x86 IDIV with divisor zero:
    // report unsupported, never silently create zero XP/award events.
    [[nodiscard]] static std::optional<CombatXpDistributionPlan> inspect(
        const PartyImage& party,
        std::uint16_t npc_definition_award) noexcept;
};

} // namespace rloft
