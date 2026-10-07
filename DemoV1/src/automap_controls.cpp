#include "rloft/automap_controls.hpp"

namespace rloft {
namespace {

// Recovered from certified CODE.2 DGROUP 5F62. Runtime drawing coordinates
// use row first, column second throughout this UI family.
constexpr std::array<
    AutomapControlRect,
    AutomapControlModel::kActionCount>
kRects{{
    {180,   3, 218,  12},
    {180, 231, 218, 240},
    {  6, 110,  24, 132},
    {375, 110, 394, 132},
    {168, 248, 229, 281},
    {243, 245, 270, 286},
    {174, 292, 187, 302},
    {243, 292, 256, 302},
    {282, 256, 306, 303},
    {310, 256, 334, 303},
    {338, 256, 362, 303},
    {366, 256, 390, 303},
}};

}  // namespace

const std::array<
    AutomapControlRect,
    AutomapControlModel::kActionCount>&
AutomapControlModel::rectangles() noexcept {
    return kRects;
}

std::optional<int> AutomapControlModel::action_at(
    int row,
    int column) noexcept {
    for (std::size_t i = 0; i < kRects.size(); ++i) {
        if (kRects[i].contains(row, column)) {
            return static_cast<int>(i);
        }
    }
    return std::nullopt;
}

int AutomapControlModel::held_action(
    int base_action) noexcept {
    if (base_action >= 0 && base_action <= 3) {
        return base_action + 12;
    }
    return 11;
}

std::uint8_t AutomapControlModel::navigation_mask(
    int action) noexcept {
    if (action >= 0 && action <= 3) {
        return static_cast<std::uint8_t>(
            1U << static_cast<unsigned>(action));
    }
    if (action >= 12 && action <= 15) {
        return static_cast<std::uint8_t>(
            1U << static_cast<unsigned>(action - 8));
    }
    return 0;
}

}  // namespace rloft
