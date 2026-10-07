#include "rloft/inventory_screen.hpp"

#include <array>
#include <cstddef>
#include <stdexcept>

namespace rloft {
namespace {

bool uses_generic_hand_graphic(std::int16_t item) noexcept {
    switch (static_cast<std::uint16_t>(item)) {
        case 0x006:
        case 0x109:
        case 0x10E:
        case 0x120:
        case 0x127:
        case 0x131:
        case 0x134:
        case 0x135:
        case 0x13B:
        case 0x13D:
        case 0x141:
        case 0x146:
        case 0x147:
            return true;
        default:
            return false;
    }
}

InventoryDrawRole role_for_slot(std::size_t slot) {
    switch (slot) {
        case 12: return InventoryDrawRole::RingA;
        case 13: return InventoryDrawRole::RingB;
        case 14: return InventoryDrawRole::HandA;
        case 15: return InventoryDrawRole::HandB;
        case 16: return InventoryDrawRole::Body;
        case 17: return InventoryDrawRole::Head;
        case 18: return InventoryDrawRole::Accessory;
        case 19: return InventoryDrawRole::Quiver;
        default:
            throw std::out_of_range("equipment slot role");
    }
}

std::uint16_t absolute_id(
    std::uint16_t base,
    std::uint16_t local) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<std::uint32_t>(base) +
        static_cast<std::uint32_t>(local));
}

InventoryItemDraw make_item(
    InventoryDrawRole role,
    std::size_t slot,
    std::int16_t stored_item,
    std::int16_t rendered_item,
    int x,
    int y,
    std::optional<int> bottom_align_bias,
    std::uint16_t local,
    std::uint16_t base,
    bool clear_before = false,
    bool flip = false) {
    return InventoryItemDraw{
        role,
        slot,
        stored_item,
        rendered_item,
        x,
        y,
        bottom_align_bias,
        local,
        absolute_id(base, local),
        clear_before,
        flip,
    };
}

}  // namespace

InventoryScreenModel InventoryScreenModel::build(
    const PartyActorPresentation& actor,
    const ObjectCatalog& objects,
    std::uint16_t res1_object_ui_base) {
    InventoryScreenModel out{};
    out.actor_index = actor.actor_index;
    out.panel_base_y =
        static_cast<int>(actor.actor_index) *
        kPanelHeight;

    // CODE.2 0F368 returns 0 for race codes 2/3/4 and 1 otherwise;
    // 10BBC XORs the result with 1.
    out.equipment_side =
        actor.race_code == 2 ||
        actor.race_code == 3 ||
        actor.race_code == 4
            ? 1
            : 0;

    // 1118C: ordinary inventory slots form three columns by four rows.
    for (std::size_t slot = 0; slot < 12U; ++slot) {
        const auto item = actor.items[slot];
        if (item == -1) continue;
        if (item < 0) {
            // Preserve non--1 negative sentinels in party state, but they do
            // not name OBJECTS.DAT records and therefore have no UI sprite.
            continue;
        }
        const auto& object =
            objects.at(static_cast<std::size_t>(item));
        const auto column = static_cast<int>(slot / 4U);
        const auto row = static_cast<int>(slot % 4U);
        out.draws.push_back(make_item(
            InventoryDrawRole::Ordinary,
            slot,
            item,
            item,
            kOrdinaryGridX + column * 0x22,
            out.panel_base_y +
                kOrdinaryGridTop + row * 0x12,
            std::nullopt,
            object.ui_graphic_index,
            res1_object_ui_base));
    }

    const int side = out.equipment_side;
    const int base_y = out.panel_base_y;
    const int hand_x = 0x72 + side * 0x12;

    // 10BBC: ring slots.
    for (std::size_t slot = 12; slot <= 13; ++slot) {
        const auto item = actor.items[slot];
        if (item < 0) continue;
        const auto& object =
            objects.at(static_cast<std::size_t>(item));
        const int y =
            base_y + (slot == 12 ? 3 : 0x3D);
        out.draws.push_back(make_item(
            role_for_slot(slot), slot, item, item,
            0xF8, y, std::nullopt,
            object.ui_graphic_index,
            res1_object_ui_base,
            true));
    }

    // 10BBC hand slots. Certain hard-coded object IDs use the generic object
    // 6 hand graphic plus one instead of their own UI graphic.
    for (std::size_t slot = 14; slot <= 15; ++slot) {
        const auto item = actor.items[slot];
        if (item < 0) continue;

        std::int16_t rendered_item = item;
        std::uint16_t local = 0;
        if (uses_generic_hand_graphic(item)) {
            rendered_item = 6;
            local = static_cast<std::uint16_t>(
                objects.at(6).ui_graphic_index + 1U);
        } else {
            local = objects.at(
                static_cast<std::size_t>(item))
                .ui_graphic_index;
        }

        out.draws.push_back(make_item(
            role_for_slot(slot), slot,
            item, rendered_item,
            hand_x,
            base_y,
            slot == 15
                ? std::optional<int>{0x4F}
                : std::nullopt,
            local,
            res1_object_ui_base));
    }

    // Body, head, accessory.
    const struct {
        std::size_t slot;
        int side_multiplier;
        bool add_side_to_graphic;
    } simple[] = {
        {16, 0x00, true},
        {18, 0x13, false},
        {17, 0x14, false},
    };
    for (const auto& entry : simple) {
        const auto item = actor.items[entry.slot];
        if (item < 0) continue;
        auto local = objects.at(
            static_cast<std::size_t>(item))
            .ui_graphic_index;
        if (entry.add_side_to_graphic) {
            local = static_cast<std::uint16_t>(
                local + side);
        }
        out.draws.push_back(make_item(
            role_for_slot(entry.slot),
            entry.slot,
            item,
            item,
            0x72 + side * entry.side_multiplier,
            base_y,
            std::nullopt,
            local,
            res1_object_ui_base));
    }

    // Quiver slot 19 is rendered with the generic object-6 graphic.
    if (actor.items[19] >= 0) {
        const auto local =
            objects.at(6).ui_graphic_index;
        out.draws.push_back(make_item(
            InventoryDrawRole::Quiver,
            19,
            actor.items[19],
            6,
            0x72 + side * 0x15,
            base_y,
            std::nullopt,
            local,
            res1_object_ui_base));
    }

    // Bracer/gauntlet slot 20 is drawn twice. Between draws CODE.2 flips the
    // cached sprite, bottom-aligns the second copy using sprite width, then
    // flips it back.
    if (actor.items[20] >= 0) {
        const auto item = actor.items[20];
        const auto local =
            objects.at(static_cast<std::size_t>(item))
                .ui_graphic_index;
        out.draws.push_back(make_item(
            InventoryDrawRole::BracerTop,
            20, item, item,
            hand_x, base_y,
            std::nullopt,
            local,
            res1_object_ui_base));
        out.draws.push_back(make_item(
            InventoryDrawRole::BracerBottomMirrored,
            20, item, item,
            hand_x, base_y,
            std::optional<int>{0x50},
            local,
            res1_object_ui_base,
            false,
            true));
    }

    return out;
}

}  // namespace rloft
