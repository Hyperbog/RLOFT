#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "rloft/exploration_state.hpp"
#include "rloft/inventory_runtime.hpp"
#include "rloft/level_world_state.hpp"

namespace rloft {

// CODE.2 12470 exchanges two 0x248 party records and repairs
// live actor references in NPC and loose-item family-C records.
// These reference bits are the three-bit field at 11..13 of
// packed_runtime_state. Placed family-B objects are untouched.
struct InventoryPartyOrderResult final {
    bool changed{};
    std::size_t npc_references_updated{};
    std::size_t loose_references_updated{};
};

class InventoryPartyOrderModel final {
public:
    [[nodiscard]] static InventoryPartyOrderResult exchange(
        PartyImage& party,LevelWorldState& world,
        InventoryRuntimeState& inventory,
        std::array<bool,4>& panel_modes,
        std::size_t first,std::size_t second);
};

} // namespace rloft
