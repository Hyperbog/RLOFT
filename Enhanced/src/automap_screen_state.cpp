#include "rloft/automap_screen_state.hpp"

namespace rloft {

AutomapScreenState AutomapScreenStateModel::initial(
    const ExplorationSessionState& session) noexcept {
    const auto origin =
        AutomapViewportPresenter::initial_origin(
            session);
    return AutomapScreenState{
        origin.column,
        origin.row,
        0,
    };
}

void AutomapScreenStateModel::apply_navigation(
    AutomapScreenState& state,
    int map_width,
    int map_height) noexcept {
    const auto mask =
        state.navigation_mask;

    if ((mask & 0x01U) != 0U) {
        if (state.row > -40) {
            state.row -= 4;
        }
    } else if ((mask & 0x02U) != 0U) {
        const int limit =
            map_height - 0x2B;
        if (state.row < limit ||
            state.row < -4) {
            state.row += 4;
        }
    } else if ((mask & 0x04U) != 0U) {
        if (state.column > -40) {
            state.column -= 4;
        }
    } else if ((mask & 0x08U) != 0U) {
        const int limit =
            map_width - 0x2C;
        if (state.column < limit ||
            state.column < -4) {
            state.column += 4;
        }
    } else if ((mask & 0x10U) != 0U) {
        state.row -= 0x28;
        if (state.row < -40) {
            state.row = -40;
        }
    } else if ((mask & 0x20U) != 0U) {
        const int limit =
            map_height - 0x2F;
        state.row += 0x28;
        if (state.row > limit) {
            state.row = limit;
        }
    } else if ((mask & 0x40U) != 0U) {
        state.column -= 0x28;
        if (state.column < -40) {
            state.column = -40;
        }
    } else if ((mask & 0x80U) != 0U) {
        const int limit =
            map_width - 0x30;
        state.column += 0x28;
        if (state.column > limit) {
            state.column = limit;
        }
    }
}

}  // namespace rloft
