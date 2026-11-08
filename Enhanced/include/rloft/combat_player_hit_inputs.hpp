#pragma once
#include <cstddef>
#include <cstdint>

#include "rloft/combat_hit_check.hpp"
#include "rloft/combat_strength_attack.hpp"
#include "rloft/exploration_state.hpp"
#include "rloft/inventory_encumbrance.hpp"
#include "rloft/inventory_runtime.hpp"
#include "rloft/level_world_state.hpp"
#include "rloft/npc_catalog.hpp"
#include "rloft/object_catalog.hpp"

namespace rloft {

// Typed adapter from the ACTUAL live character/item/NPC sources to the
// already audited CODE.2 C80C hit arithmetic. No random draws or combat
// effects: caller must perform 24478/26018 eligibility and target selection
// before calling this, then supply a genuine 1d20 if the NPC is not held.
class CombatPlayerHitInputsModel final {
public:
    [[nodiscard]] static CombatHitCheckInputs build(
        const PartyImage& party,
        const InventoryRuntimeState& inventory,
        const ObjectCatalog& objects,
        const InventoryEncumbranceModel& burden,
        const CombatStrengthAttackModel& strength,
        const NpcDefinition& definition,
        const LiveNpcRecord& npc,
        std::size_t actor,
        std::int32_t dispatched_item_id,
        std::uint8_t raw_d20);
};

} // namespace rloft
