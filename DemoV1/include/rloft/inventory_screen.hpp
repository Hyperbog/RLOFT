#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include "rloft/object_catalog.hpp"
#include "rloft/party_presentation.hpp"

namespace rloft {

enum class InventoryDrawRole {
    Ordinary,
    RingA,
    RingB,
    HandA,
    HandB,
    Body,
    Head,
    Accessory,
    Quiver,
    BracerTop,
    BracerBottomMirrored,
};

struct InventoryItemDraw final {
    InventoryDrawRole role{InventoryDrawRole::Ordinary};
    std::size_t slot{};
    std::int16_t stored_item_id{-1};
    std::int16_t rendered_item_id{-1};

    int x{};
    int y{};
    // CODE.2 bottom alignment uses panelBase + bias - sprite_width.
    std::optional<int> bottom_align_bias;

    std::uint16_t local_ui_graphic{};
    std::uint16_t absolute_res1_id{};
    bool clear_32x16_before_draw{};
    bool temporarily_flip_sprite{};
};

struct InventoryScreenModel final {
    static constexpr int kPanelHeight = 0x50;
    static constexpr int kOrdinaryGridX = 0x129;
    static constexpr int kOrdinaryGridTop = 5;

    std::size_t actor_index{};
    int panel_base_y{};
    int equipment_side{};
    std::vector<InventoryItemDraw> draws;

    // CODE.2 10A70 physical 4B9EF: selected open-container subtype
    // uses RES2 bank, source graphic (9+subtype), rendered at VGA
    // row 0xCE, the character's own 80-pixel column origin.
    [[nodiscard]] static int original_container_res2_graphic(
        std::uint8_t subtype);

    [[nodiscard]] static InventoryScreenModel build(
        const PartyActorPresentation& actor,
        const ObjectCatalog& objects,
        std::uint16_t res1_object_ui_base);
};

}  // namespace rloft
