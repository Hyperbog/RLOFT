#pragma once

#include <array>
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

// CODE.2 21880..2195A, invoked by the world loose-item
// constructor 2195C after adding OBJECTS ID 0x89 on Level16.
// Source constants are the EIGHT verbatim CODE.2 object-1
// bytes at logical 207E0: 0D 13 0B 15 0D 17 0F 15.
// Each (x,y) pair compares to the HIGH WORDS of the 0x16
// byte live loose-item coordinates. Duplicates do not fill
// multiple distinct sites. No item is consumed by this check.
enum class Level16SeedRitualStatus : std::uint8_t {
    NotLevel16, AlreadyFlagged, MissingSeed, ReadyForSourceActions
};
struct Level16SeedRitualCheck final {
    Level16SeedRitualStatus status{
        Level16SeedRitualStatus::NotLevel16};
    std::array<bool,4> present{};
    static constexpr std::size_t kEventBit=0x44U;
    static constexpr std::uint32_t kFirstFollowOnArg=8U; // call 13EF8
    static constexpr std::uint32_t kSecondFollowOnArg=0x73U; // 15050
};
class Level16SeedRitualModel final {
public:
    static constexpr std::uint16_t kSeedObjectId=0x89U;
    static constexpr std::array<std::array<std::uint8_t,2>,4>
        kSourceCells{{{{13U,19U}},{{11U,21U}},
                      {{13U,23U}},{{15U,21U}}}};

    // On 2195C append, the source evicts the oldest loose-item
    // record when the vector is full. The candidate placement is
    // evaluated as if already appended, without prematurely
    // mutating world/hand or the persistent event bit.
    [[nodiscard]] static Level16SeedRitualCheck evaluate_21880(
        const ExplorationSessionState& session,
        const LevelWorldState& world,
        const LiveObjectRecord* candidate=nullptr,
        bool evict_oldest=false) noexcept;
};

// CODE.2 13118, logical key 0x44 (D scancode 0x20) dispatches to
// 2195C with screen seed 0x80, held OBJECTS ID and known bit,
// original player coordinate words, forward-placement flag 1,
// and clear-held-on-success flag 1.
class InventoryWorldDropModel final {
public:
    using PlacementProbe=std::function<bool(
        std::uint32_t y,std::uint32_t x,std::uint32_t actor_mode)>;
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
