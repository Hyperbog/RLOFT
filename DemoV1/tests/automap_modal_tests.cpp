#include "rloft/automap_modal.hpp"

#include <cassert>

int main() {
    rloft::ExplorationSessionState session{};
    session.coordinate_a = 0x00048000U;
    session.coordinate_b = 0x00048000U;

    rloft::AutomapModalController modal(session);
    assert(modal.state().column == -20);
    assert(modal.state().row == -20);
    assert(!modal.annotation_overlay_enabled());

    auto result = modal.apply_action(0, 30, 15);
    assert(result == rloft::AutomapModalResult::Redraw);
    assert(modal.state().row == -24);
    assert(modal.state().navigation_mask == 0);

    result = modal.apply_action(3, 30, 15);
    assert(result == rloft::AutomapModalResult::Redraw);
    assert(modal.state().column == -16);

    result = modal.apply_action(12, 30, 15);
    assert(result == rloft::AutomapModalResult::Redraw);
    assert(modal.state().row == -40);

    result = modal.apply_action(8, 30, 15);
    assert(result == rloft::AutomapModalResult::Redraw);
    assert(modal.annotation_overlay_enabled());

    result = modal.apply_action(4, 30, 15);
    assert(result == rloft::AutomapModalResult::DeferredAction);

    result = modal.apply_action(11, 30, 15);
    assert(result == rloft::AutomapModalResult::Close);
}
