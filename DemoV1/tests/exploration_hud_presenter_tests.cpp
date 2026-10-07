#include "rloft/exploration_hud_presenter.hpp"

#include <cassert>
#include <stdexcept>

int main() {
    {
        const auto layout =
            rloft::ExplorationHudPresenter::base_layout(
                15, 0);

        assert(layout.upper_resource_id == 16);
        assert(layout.lower_resource_id == 17);
        assert(layout.upper_y == 0);
        assert(layout.lower_y == 200);
        assert(layout.x == 0);
    }

    {
        const auto layout =
            rloft::ExplorationHudPresenter::base_layout(
                15, 1);

        assert(layout.upper_resource_id == 19);
        assert(layout.lower_resource_id == 20);
    }

    bool rejected = false;
    try {
        (void)rloft::ExplorationHudPresenter::base_layout(
            15, -1);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected);
}
