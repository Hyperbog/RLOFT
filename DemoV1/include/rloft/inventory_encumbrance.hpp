#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "rloft/exploration_state.hpp"
#include "rloft/inventory_runtime.hpp"
#include "rloft/object_catalog.hpp"

namespace rloft {

struct InventoryEncumbranceSnapshot final {
    std::uint32_t carried_pounds{};
    std::uint32_t carried_half_pounds{};
    std::uint16_t maximum_pounds{};
    std::array<std::uint16_t,5> burden_thresholds{};
    std::uint8_t burden_rank{}; // 0..4 from original CODE.2 0FE44
    std::uint8_t effective_strength{};
};

class InventoryEncumbranceModel final {
public:
    // Original CODE.2 DGROUP 7E8C has the five threshold
    // values per strength row, 10 bytes each, for indices 2..22.
    // 0FDE4 uses fifth threshold as the actor's "MAX" capacity.
    [[nodiscard]] static InventoryEncumbranceModel from_code2(
        const std::vector<std::uint8_t>& original_code2);

    [[nodiscard]] InventoryEncumbranceSnapshot actor(
        const PartyImage& party,
        const InventoryRuntimeState& inventory,
        const ObjectCatalog& objects,
        std::size_t actor) const;

    [[nodiscard]] static std::uint8_t effective_strength(
        std::uint8_t ability,std::uint8_t exceptional) noexcept;

    [[nodiscard]] std::array<std::uint16_t,5> thresholds(
        std::uint8_t effective_strength) const;

private:
    std::array<std::array<std::uint16_t,5>,21> thresholds_{};
};

} // namespace rloft
