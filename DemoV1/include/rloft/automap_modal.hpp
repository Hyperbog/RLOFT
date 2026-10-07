#pragma once

#include "rloft/automap_controls.hpp"
#include "rloft/automap_screen_state.hpp"
#include "rloft/exploration_session.hpp"

namespace rloft {

enum class AutomapModalResult {
    None,
    Redraw,
    Close,
    DeferredAction,
};

class AutomapModalController final {
public:
    explicit AutomapModalController(
        const ExplorationSessionState& session) noexcept
        : state_(AutomapScreenStateModel::initial(session)) {}

    [[nodiscard]] const AutomapScreenState& state() const noexcept {
        return state_;
    }

    [[nodiscard]] bool annotation_overlay_enabled() const noexcept {
        return annotation_overlay_enabled_;
    }

    [[nodiscard]] AutomapModalResult apply_action(
        int action,
        int map_width,
        int map_height) noexcept;

private:
    AutomapScreenState state_{};
    bool annotation_overlay_enabled_{};
};

}  // namespace rloft
