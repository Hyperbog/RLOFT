#include "rloft/automap_controls.hpp"

#include <cassert>

int main() {
    const auto& r =
        rloft::AutomapControlModel::rectangles();
    assert(r.size() == 12);

    // Inclusive CODE.2 hit bounds.
    assert(rloft::AutomapControlModel::action_at(
               180, 3).value() == 0);
    assert(rloft::AutomapControlModel::action_at(
               218, 12).value() == 0);
    assert(!rloft::AutomapControlModel::action_at(
                219, 12).has_value());

    assert(rloft::AutomapControlModel::action_at(
               6, 110).value() == 2);
    assert(rloft::AutomapControlModel::action_at(
               390, 303).value() == 11);

    assert(rloft::AutomapControlModel::held_action(0) == 12);
    assert(rloft::AutomapControlModel::held_action(3) == 15);
    assert(rloft::AutomapControlModel::held_action(8) == 11);

    assert(rloft::AutomapControlModel::navigation_mask(0) == 0x01);
    assert(rloft::AutomapControlModel::navigation_mask(1) == 0x02);
    assert(rloft::AutomapControlModel::navigation_mask(2) == 0x04);
    assert(rloft::AutomapControlModel::navigation_mask(3) == 0x08);
    assert(rloft::AutomapControlModel::navigation_mask(12) == 0x10);
    assert(rloft::AutomapControlModel::navigation_mask(13) == 0x20);
    assert(rloft::AutomapControlModel::navigation_mask(14) == 0x40);
    assert(rloft::AutomapControlModel::navigation_mask(15) == 0x80);
    assert(rloft::AutomapControlModel::navigation_mask(11) == 0);
}
