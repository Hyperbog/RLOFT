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

struct InventoryRuntimeState final {
    static constexpr std::size_t kSerializedContainerCount = 62;
    static constexpr std::size_t kRuntimeDescriptorCount = 63;

    HeldItemState held{};
    std::array<ContainerRuntimeState, kRuntimeDescriptorCount> containers{};

    [[nodiscard]] static InventoryRuntimeState fresh(
        const GameSource& source);

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
