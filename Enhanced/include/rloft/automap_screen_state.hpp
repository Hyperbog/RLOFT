#pragma once

#include <cstdint>

#include "rloft/automap_viewport_presenter.hpp"
#include "rloft/exploration_session.hpp"

namespace rloft {

struct AutomapScreenState final {
    int column{};
    int row{};
    std::uint8_t navigation_mask{};
};

class AutomapScreenStateModel final {
public:
    [[nodiscard]] static AutomapScreenState initial(
        const ExplorationSessionState& session) noexcept;

    // CODE.2 034B0. Only the first active bit in the encoded priority order
    // is applied per update.
    static void apply_navigation(
        AutomapScreenState& state,
        int map_width,
        int map_height) noexcept;
};

}  // namespace rloft
