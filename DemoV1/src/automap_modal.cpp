#include "rloft/automap_modal.hpp"

namespace rloft {

AutomapModalResult AutomapModalController::apply_action(
    int action,
    int map_width,
    int map_height) noexcept {
    const auto mask =
        AutomapControlModel::navigation_mask(action);
    if (mask != 0U) {
        state_.navigation_mask = mask;
        AutomapScreenStateModel::apply_navigation(
            state_, map_width, map_height);
        state_.navigation_mask = 0;
        return AutomapModalResult::Redraw;
    }

    if (action == 8) {
        annotation_overlay_enabled_ =
            !annotation_overlay_enabled_;
        return AutomapModalResult::Redraw;
    }

    if (action == 11) {
        state_.navigation_mask = 0;
        return AutomapModalResult::Close;
    }

    if (action >= 0 &&
        action < static_cast<int>(
            AutomapControlModel::kActionCount)) {
        return AutomapModalResult::DeferredAction;
    }

    return AutomapModalResult::None;
}

}  // namespace rloft
