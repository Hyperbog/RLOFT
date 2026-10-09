#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include "rloft/exploration_session.hpp"
#include "rloft/inventory_runtime.hpp"
#include "rloft/level_world_state.hpp"
#include "rloft/object_catalog.hpp"
#include "rloft/renderer_tables.hpp"

namespace rloft {

enum class InventoryWorldDropStatus : std::uint8_t {
    NothingHeld,
    Dropped,
    SourceSpecialActionPending,
    InvalidWorldPosition,
    InvalidInventoryObject
};
struct InventoryWorldDropResult final {
    InventoryWorldDropStatus status{InventoryWorldDropStatus::NothingHeld};
    std::size_t loose_item_index{};
    std::uint16_t cached_cell_index{};
};

// CODE.2 13118, logical key 0x44 (D scancode 0x20) dispatches to
// 2195C with screen seed 0x80, held OBJECTS ID and known bit,
// original player coordinate words, forward-placement flag 1,
// and clear-held-on-success flag 1.
class InventoryWorldDropModel final {
public:
    using PlacementProbe=std::function<bool(
        std::uint32_t y,std::uint32_t x,std::uint32_t radius)>;
    // 2D5D0(y,x,0x190): fixed +/-0x5800 footprint, map/group-2\n    // control tests, live-NPC checks and player proximity.\n    [[nodiscard]] static bool placement_passable_2d5d0(\n        const ExplorationSessionState& session,\n        const LevelWorldState& world,\n        const NpcCatalog& npcs,\n        std::uint32_t y,std::uint32_t x,\n        std::uint32_t actor_mode);\n\n    [[nodiscard]] static InventoryWorldDropResult drop(
        LevelWorldState& world,
        const ExplorationSessionState& session,
        InventoryRuntimeState& inventory,
        const ObjectCatalog& objects,
        const RendererTables& tables,
        const PlacementProbe& can_place,
        bool original_c02f_center_fraction=false);
};
}
