#pragma once

#include <array>
#include <cstddef>
#include <optional>

namespace rloft {

struct PartyPanelHitRect final {
    int row_min{};
    int column_min{};
    int row_max{};
    int column_max{};

    [[nodiscard]] bool contains(
        int row,
        int column) const noexcept {
        return row >= row_min &&
               row <= row_max &&
               column >= column_min &&
               column <= column_max;
    }
};

class PartyPanelControlModel final {
public:
    static constexpr int kActorCount = 4;
    static constexpr int kModalColumnWidth = 0x50;

    // CODE.2 DGROUP D684 region table, regions 4/8/12/16. These are the
    // lane-3 gameplay hit regions routed by 229CC to 211D8(actor,...), whose
    // primary-click path enters the 13118 inventory/character modal.
    [[nodiscard]] static const std::array<PartyPanelHitRect, 4>&
    gameplay_inventory_regions() noexcept;

    [[nodiscard]] static std::optional<std::size_t>
    gameplay_actor_at(
        int row,
        int column) noexcept;

    // CODE.2 12E70 computes actor = column / 0x50 in modal mode D680=1.
    [[nodiscard]] static std::optional<std::size_t>
    modal_actor_at_column(
        int column) noexcept;

    struct OrdinaryInventoryHit final {
        std::size_t actor{};
        std::size_t slot{};
    };

    // 12E70 ordinary 3x4 grid: rows 129h + (slot/4)*22h,
    // actor-local columns 5 + (slot%4)*12h, 32x16 draw/hit tiles.
    // Preserve the two-pixel gutters: they are not item clicks.
    [[nodiscard]] static std::optional<OrdinaryInventoryHit>
    modal_ordinary_slot_at(int row, int column) noexcept;

    // CODE.2 12E70 -> 11AD8. Within the selected 80-pixel actor column the
    // inventory/character toggle is row 45h..60h, local column 39h..4Dh.
    [[nodiscard]] static std::optional<std::size_t>
    modal_toggle_actor_at(
        int row,
        int column) noexcept;
};

}  // namespace rloft
