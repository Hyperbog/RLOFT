#pragma once

#include <cstdint>

#include "rloft/level_world_state.hpp"
#include "rloft/npc_catalog.hpp"
#include "rloft/placed_scene_catalog.hpp"

namespace rloft {

enum class OriginalEntityContact {
    Clear,
    Blocked,
    RequiresSideEffect,
};

// CODE.2 2D5D0's POST-cell checks call 2C800 (live NPC overlap)
// and 2CC88 (placed-object footprint / authored transitions).
// Party mode -1; no NPC/player knockback, interactions, or teleports
// are synthesized from read-only overlap tests.
class OriginalEntityCollisionModel final {
public:
    [[nodiscard]] static OriginalEntityContact party_contact(
        const LevelWorldState& world,
        const NpcCatalog& npcs,
        const PlacedSceneCatalog& placed,
        std::uint32_t current_level_id,
        std::uint32_t coordinate_a,
        std::uint32_t coordinate_b);
};

} // namespace rloft
