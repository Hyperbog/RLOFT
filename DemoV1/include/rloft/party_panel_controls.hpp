#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <cstdint>
#include <vector>

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

// CODE.2 12180 reads ten 24-byte (slot,flags,rowMin,colMin,
// rowMax,colMax) descriptors through live DGROUP:12C00. CODE.2
// 12E70 sets that pointer to the original data tables at 6DB0 or
// 6EA0 based on source 0F368(race): 2/3/4 use 6EA0; other races
// use 6DB0. These are item pickup rectangles, not sprite bounds.
struct OriginalEquipmentHitDescriptor final {
    std::int32_t slot{};
    std::uint32_t accepted_pointer_flags{};
    PartyPanelHitRect rect{};
};

class OriginalEquipmentHitTables final {
public:
    static constexpr std::size_t kTableCount=2U;
    static constexpr std::size_t kDescriptorsPerTable=10U;
    [[nodiscard]] static OriginalEquipmentHitTables from_code2(
        const std::vector<std::uint8_t>& original_code2);
    [[nodiscard]] const OriginalEquipmentHitDescriptor& descriptor(
        bool short_race,std::size_t index) const;
    [[nodiscard]] std::optional<std::size_t> occupied_hit(
        bool short_race,int row,int local_column,
        std::uint32_t mouse_flags,
        const std::array<std::int16_t,21>& actor_items) const noexcept;
private:
    // Index 0: original 6DB0 (non-short races);
    // index 1: original 6EA0 (race 2,3,4).
    std::array<std::array<OriginalEquipmentHitDescriptor,
        kDescriptorsPerTable>,kTableCount> tables_{};
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

    struct GameplayHandHit final {
        std::size_t actor{};
        std::size_t hand{};  // 0 or 1
    };

    // CODE.2 DGROUP D684 regions (1,3),(5,7),(9,11),(13,15),
    // routed through 229CC to 214B8. No inferred overlap or gutters.
    [[nodiscard]] static std::optional<GameplayHandHit>
    gameplay_hand_at(int row, int column) noexcept;

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

    struct ContainerSlotHit final {
        std::size_t actor{};
        std::size_t slot{};
    };

    // CODE.2 12E70/11C3C open container at row 0xDB plus 0x22
    // per grid row, with descriptor-specific columns and x-offset.
    // The caller supplies the ORIGINAL 10-byte descriptor's layout.
    [[nodiscard]] static std::optional<ContainerSlotHit>
    modal_container_slot_at(
        int row,int column,std::size_t actor,
        std::uint8_t columns,std::uint8_t x_offset,
        std::size_t slot_count) noexcept;

    // CODE.2 12E70 -> 11AD8. Within the selected 80-pixel actor column the
    // inventory/character toggle is row 45h..60h, local column 39h..4Dh.
    // CODE.2 12E70 row 0x10..0x27 and actor-local col
    // 0x44..0x4D directly calls original 0x12988, which
    // renders MAX/CURRENT/rank from 0FD84,0FE44,0FDE4.
    // The 0x2B..0x40 icons are separate status/effect screens.
    [[nodiscard]] static std::optional<std::size_t>
    modal_encumbrance_actor_at(int row,int column) noexcept;

    [[nodiscard]] static std::optional<std::size_t>
    modal_toggle_actor_at(
        int row,
        int column) noexcept;
};

}  // namespace rloft
