#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

#include "rloft/combat_hit_check.hpp"
#include "rloft/combat_strength_attack.hpp"
#include "rloft/exploration_state.hpp"
#include "rloft/inventory_encumbrance.hpp"
#include "rloft/inventory_runtime.hpp"
#include "rloft/npc_catalog.hpp"
#include "rloft/level_world_state.hpp"
#include "rloft/object_catalog.hpp"
#include "rloft/original_random.hpp"

namespace rloft {

// Bounded E5F0 weapon-hit slice for an EXPLICIT, source-validated
// type-9 Level22 Assassin. The normal -1 DB94/DACC target-search
// buffers and 24B48 animation clock are not reconstructed here.
// Caller stages the entire runtime before calling: source effects,
// C80C RNG and injury must commit/roll back together.
struct CombatLevel22StrikeResult final {
    CombatHitCheckResult hit{};
    std::uint8_t raw_d20{};
    std::uint16_t damage_after_c910{};
    std::array<std::uint8_t,PartyImage::kActorCount> redraw_cf5a{};
};

class CombatLevel22StrikeModel final {
public:
    [[nodiscard]] static CombatLevel22StrikeResult resolve(
        PartyImage& party,
        const InventoryRuntimeState& inventory,
        const ObjectCatalog& objects,
        const InventoryEncumbranceModel& burden,
        const CombatStrengthAttackModel& strength,
        const NpcDefinition& npc_definition,
        const LiveNpcRecord& npc,
        OriginalRandom& random,
        std::size_t actor,
        std::size_t hand,
        std::int32_t item_id);
};
} // namespace rloft
