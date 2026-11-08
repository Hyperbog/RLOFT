#pragma once
#include <cstdint>
#include <optional>

#include "rloft/combat_progression_thresholds.hpp"
#include "rloft/combat_xp_distribution.hpp"
#include "rloft/exploration_state.hpp"

namespace rloft {

// Original DDA4 class-track XP DWORD writes, projected through the
// exact authenticated CODE.2 progression tables. The 50DC/3EA4/3DE4
// level-change callbacks are not available yet. Those are reported
// explicitly, not silently replaced with fabricated levels/spells/HP.
struct CombatXpAwardResult final {
    CombatXpDistributionPlan distribution{};
    CombatXpLevelProjection level_projection{};
    bool level_change_services_pending{};
    std::uint8_t updated_tracks{};
};

class CombatXpAwardService final {
public:
    // Every validation completes before the first byte write. Caller
    // stages its ENTIRE game state before the attack, so injury/loot
    // and XP cannot be partially committed across exceptions.
    [[nodiscard]] static std::optional<CombatXpAwardResult> apply(
        PartyImage& party,
        std::uint16_t original_npc_award,
        const CombatProgressionTables& source_tables) noexcept;
};

} // namespace rloft
