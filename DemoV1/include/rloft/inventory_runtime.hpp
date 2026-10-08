#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/exploration_state.hpp"
#include "rloft/game_source.hpp"
#include "rloft/object_catalog.hpp"

namespace rloft {

struct HeldItemState final {
    std::int16_t object_id{-1};
    std::uint8_t known{};
};

struct ContainerRuntimeState final {
    std::uint8_t slot_count{};
    std::uint8_t ui_columns{};
    std::uint8_t capacity_pounds{};
    std::uint8_t ui_x_offset{};
    std::uint8_t subtype{};
    std::uint8_t per_slot_state_bits{};
    std::vector<std::int16_t> items;
};

enum class InventoryQuickStashStatus : std::uint8_t {
    NothingHeld,
    Stashed,
    Full,
    Unsupported
};

enum class InventoryHandSwapStatus : std::uint8_t {
    NoChange,
    Swapped,
    Unsupported
};

enum class InventoryOrdinarySwapStatus : std::uint8_t {
    NoChange,
    Swapped,
    Unsupported
};

struct InventoryRuntimeState final {
    static constexpr std::size_t kSerializedContainerCount = 62;
    static constexpr std::size_t kRuntimeDescriptorCount = 63;

    HeldItemState held{};
    std::array<ContainerRuntimeState, kRuntimeDescriptorCount> containers{};

    [[nodiscard]] static InventoryRuntimeState fresh(
        const GameSource& source);

    // CODE.2 117A0 verified ordinary-slot identity + 0x12B known-bit
    // transfer. The caller must have already resolved a 12E70 3x4
    // ordinary-grid hit; unsupported container insertion or special
    // object interactions are rejected without mutating either state.
    [[nodiscard]] InventoryOrdinarySwapStatus swap_ordinary_slot(
        PartyImage& party,
        const ObjectCatalog& objects,
        std::size_t actor,
        std::size_t slot);

    // CODE.2 211D8 flag 08h generic actor-panel fallback:
    // place a held ordinary object in the first empty slot 0..11,
    // clear the cursor, leave party item-known bits unchanged.
    // Category 0F/0D special paths are intentionally separate.
    [[nodiscard]] InventoryQuickStashStatus quick_stash_held(
        PartyImage& party,
        const ObjectCatalog& objects,
        std::size_t actor);

    // CODE.2 214B8, pointer flag 08h: swap held item with actor
    // item[14+hand]. Only the ordinary, non-container / non-special
    // exchange is enabled. Clear this hand's +18E/192 readiness field.
    // Separate pointer flag 01h invokes *use/attack*, NOT this method.
    [[nodiscard]] InventoryHandSwapStatus swap_hand_slot(
        PartyImage& party,
        const ObjectCatalog& objects,
        std::size_t actor,
        std::size_t hand);

    [[nodiscard]] bool has_object(
        const PartyImage& party,
        const ObjectCatalog& objects,
        std::int16_t target) const;

    // CODE.2 0F540 ordering: held item first, then each active actor and each
    // of 21 slots, with recursive container removal. Direct party-slot removal
    // intentionally leaves the stale item-known bit because 0F238 is called
    // only after the slot was set to -1 and therefore returns immediately.
    void remove_object(
        PartyImage& party,
        const ObjectCatalog& objects,
        std::int16_t target);
};

}  // namespace rloft
