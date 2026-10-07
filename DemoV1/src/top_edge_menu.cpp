#include "rloft/top_edge_menu.hpp"

#include <stdexcept>

namespace rloft {

bool TopEdgeMenuModel::active_for_mouse_y(int mouse_y) noexcept {
    // CODE.2 080B5..080BF uses a signed comparison against 0x40.
    return mouse_y < kHeight;
}

int TopEdgeMenuModel::selection_from_mouse_x(int mouse_x) noexcept {
    // CODE.2 uses signed IDIV by 0x28. C++ integer division has the same
    // truncation-toward-zero rule for representable values.
    const int selection = mouse_x / kEntryWidth;
    return selection >= 0 && selection < kEntryCount
        ? selection
        : -1;
}

int TopEdgeMenuModel::selection_from_mouse(
    int mouse_x,
    int mouse_y) noexcept {
    if (!active_for_mouse_y(mouse_y)) {
        return -1;
    }
    return selection_from_mouse_x(mouse_x);
}

std::string_view TopEdgeMenuModel::label(
    TopEdgeMenuAction action) noexcept {
    const auto index = static_cast<int>(action);
    if (index < 0 || index >= kEntryCount) {
        return {};
    }
    return kLabels[static_cast<std::size_t>(index)];
}

TopEdgeMenuAction TopEdgeMenuModel::action_for_selection(
    int selection) {
    if (selection < 0 || selection >= kEntryCount) {
        throw std::out_of_range("top-edge menu selection");
    }
    return static_cast<TopEdgeMenuAction>(selection);
}

}  // namespace rloft
