#pragma once

#include <array>
#include <cstdint>
#include <optional>

namespace rloft {

struct AutomapControlRect final {
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

class AutomapControlModel final {
public:
    static constexpr std::size_t kActionCount = 12;

    [[nodiscard]] static const std::array<
        AutomapControlRect,
        kActionCount>& rectangles() noexcept;

    // CODE.2 02DB0 scans rectangles in order and uses inclusive bounds.
    [[nodiscard]] static std::optional<int> action_at(
        int row,
        int column) noexcept;

    // Press/drag form of actions 0..3 maps to page-like actions 12..15.
    [[nodiscard]] static int held_action(
        int base_action) noexcept;

    // Exact 02DB0 action -> 5FCA navigation mask for navigation actions.
    [[nodiscard]] static std::uint8_t navigation_mask(
        int action) noexcept;
};

}  // namespace rloft
