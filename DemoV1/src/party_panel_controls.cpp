#include "rloft/party_panel_controls.hpp"

namespace rloft {
namespace {

// Recovered directly from certified CODE.2 object-2 data at
// file 75F24h + DGROUP D684h. Table coordinates are (row,column).
constexpr std::array<PartyPanelHitRect, 4> kGameplayInventoryRegions{{
    {0x138, 0x004, 0x187, 0x033},
    {0x138, 0x039, 0x187, 0x068},
    {0x138, 0x0D7, 0x187, 0x106},
    {0x138, 0x10C, 0x187, 0x13B},
}};

}  // namespace

const std::array<PartyPanelHitRect, 4>&
PartyPanelControlModel::gameplay_inventory_regions() noexcept {
    return kGameplayInventoryRegions;
}

std::optional<std::size_t>
PartyPanelControlModel::gameplay_actor_at(
    int row,
    int column) noexcept {
    for (std::size_t actor = 0;
         actor < kGameplayInventoryRegions.size();
         ++actor) {
        if (kGameplayInventoryRegions[actor].contains(
                row, column)) {
            return actor;
        }
    }
    return std::nullopt;
}

std::optional<std::size_t>
PartyPanelControlModel::modal_actor_at_column(
    int column) noexcept {
    if (column < 0 ||
        column >= kActorCount * kModalColumnWidth) {
        return std::nullopt;
    }
    return static_cast<std::size_t>(
        column / kModalColumnWidth);
}

std::optional<PartyPanelControlModel::OrdinaryInventoryHit>
PartyPanelControlModel::modal_ordinary_slot_at(
    int row,
    int column) noexcept {
    const auto actor=modal_actor_at_column(column);
    if (!actor || row < 0x129 || row > 0x18C) {
        return std::nullopt;
    }
    const auto local_column =
        column-static_cast<int>(*actor)*kModalColumnWidth;
    if (local_column < 5 || local_column > 0x4A) {
        return std::nullopt;
    }
    const int local_row=row-0x129;
    const int local_x=local_column-5;
    const int vertical_index=local_row/0x22;
    const int horizontal_index=local_x/0x12;
    if (vertical_index >= 3 || horizontal_index >= 4 ||
        local_row%0x22 >= 0x20 ||
        local_x%0x12 >= 0x10) {
        return std::nullopt;
    }
    return OrdinaryInventoryHit{
        *actor,
        static_cast<std::size_t>(
            vertical_index*4+horizontal_index),
    };
}

std::optional<std::size_t>
PartyPanelControlModel::modal_toggle_actor_at(
    int row,
    int column) noexcept {
    const auto actor =
        modal_actor_at_column(column);
    if (!actor.has_value()) {
        return std::nullopt;
    }

    const int local_column =
        column -
        static_cast<int>(*actor) *
            kModalColumnWidth;

    if (row < 0x45 ||
        row > 0x60 ||
        local_column < 0x39 ||
        local_column > 0x4D) {
        return std::nullopt;
    }
    return actor;
}

}  // namespace rloft
