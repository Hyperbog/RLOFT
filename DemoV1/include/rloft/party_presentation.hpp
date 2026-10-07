#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

#include "rloft/exploration_state.hpp"

namespace rloft {

enum class InventorySlotRole {
    Ordinary,
    RingA,
    RingB,
    HandA,
    HandB,
    Body,
    Head,
    Accessory,
    Quiver,
    BracerGauntlet,
};

struct PartyActorPresentation final {
    static constexpr std::size_t kInventorySlotCount = 21;
    static constexpr std::size_t kOrdinaryInventoryCount = 12;

    std::size_t actor_index{};
    std::string name;
    std::uint16_t status_word{};
    bool active{};

    std::uint8_t portrait_index{};
    std::uint8_t sex_code{};
    std::int8_t base_armor_class{};
    std::int16_t current_hp{};
    std::int16_t max_hp{};

    std::array<std::int16_t, kInventorySlotCount> items{};
    std::uint32_t item_known_bits{};

    [[nodiscard]] bool item_known(std::size_t slot) const;
    [[nodiscard]] static InventorySlotRole slot_role(std::size_t slot);
    [[nodiscard]] int portrait_local_index() const noexcept;
};

class PartyPresentationModel final {
public:
    [[nodiscard]] static PartyActorPresentation actor(
        const PartyImage& party,
        std::size_t actor_index);
};

}  // namespace rloft
