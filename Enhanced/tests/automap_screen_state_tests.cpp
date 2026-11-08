#include "rloft/automap_screen_state.hpp"

#include <cassert>

int main() {
    rloft::ExplorationSessionState session{};
    session.coordinate_a = 0x00048000U;
    session.coordinate_b = 0x00048000U;

    auto state =
        rloft::AutomapScreenStateModel::initial(
            session);
    assert(state.column == -20);
    assert(state.row == -20);
    assert(state.navigation_mask == 0);

    // First four bits are the encoded 4-cell steps.
    state.navigation_mask = 0x01;
    rloft::AutomapScreenStateModel::
        apply_navigation(state, 30, 15);
    assert(state.row == -24);

    state.navigation_mask = 0x02;
    rloft::AutomapScreenStateModel::
        apply_navigation(state, 30, 15);
    assert(state.row == -20);

    state.navigation_mask = 0x04;
    rloft::AutomapScreenStateModel::
        apply_navigation(state, 30, 15);
    assert(state.column == -24);

    state.navigation_mask = 0x08;
    rloft::AutomapScreenStateModel::
        apply_navigation(state, 30, 15);
    assert(state.column == -20);

    // Page-like 40-cell moves clamp exactly to the binary's limits.
    state.navigation_mask = 0x10;
    rloft::AutomapScreenStateModel::
        apply_navigation(state, 30, 15);
    assert(state.row == -40);

    state.navigation_mask = 0x20;
    rloft::AutomapScreenStateModel::
        apply_navigation(state, 30, 15);
    assert(state.row == 15 - 0x2F);

    state.column = -20;
    state.navigation_mask = 0x40;
    rloft::AutomapScreenStateModel::
        apply_navigation(state, 30, 15);
    assert(state.column == -40);

    state.navigation_mask = 0x80;
    rloft::AutomapScreenStateModel::
        apply_navigation(state, 30, 15);
    assert(state.column == 30 - 0x30);

    // Priority is first set bit, matching the else-if chain.
    state.row = -20;
    state.column = -20;
    state.navigation_mask = 0x05;
    rloft::AutomapScreenStateModel::
        apply_navigation(state, 30, 15);
    assert(state.row == -24);
    assert(state.column == -20);
}
