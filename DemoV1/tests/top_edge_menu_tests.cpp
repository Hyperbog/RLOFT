#include "rloft/top_edge_menu.hpp"

#include <cassert>
#include <stdexcept>

int main() {
    using rloft::TopEdgeMenuAction;
    using rloft::TopEdgeMenuModel;

    assert(TopEdgeMenuModel::active_for_mouse_y(0));
    assert(TopEdgeMenuModel::active_for_mouse_y(63));
    assert(!TopEdgeMenuModel::active_for_mouse_y(64));

    assert(TopEdgeMenuModel::selection_from_mouse_x(0) == 0);
    assert(TopEdgeMenuModel::selection_from_mouse_x(39) == 0);
    assert(TopEdgeMenuModel::selection_from_mouse_x(40) == 1);
    assert(TopEdgeMenuModel::selection_from_mouse_x(279) == 6);
    assert(TopEdgeMenuModel::selection_from_mouse_x(280) == 7);
    assert(TopEdgeMenuModel::selection_from_mouse_x(319) == 7);
    assert(TopEdgeMenuModel::selection_from_mouse_x(320) == -1);

    // Signed IDIV truncates toward zero: -1 / 40 selects column zero.
    assert(TopEdgeMenuModel::selection_from_mouse_x(-1) == 0);
    assert(TopEdgeMenuModel::selection_from_mouse_x(-40) == -1);

    assert(TopEdgeMenuModel::selection_from_mouse(120, 63) == 3);
    assert(TopEdgeMenuModel::selection_from_mouse(120, 64) == -1);

    assert(TopEdgeMenuModel::label(TopEdgeMenuAction::Rest) == "rest");
    assert(TopEdgeMenuModel::label(TopEdgeMenuAction::Pray) == "pray");
    assert(TopEdgeMenuModel::label(TopEdgeMenuAction::Memorize) == "memorize");
    assert(TopEdgeMenuModel::label(TopEdgeMenuAction::Automap) == "automap");
    assert(TopEdgeMenuModel::label(TopEdgeMenuAction::Load) == "load");
    assert(TopEdgeMenuModel::label(TopEdgeMenuAction::Save) == "save");
    assert(TopEdgeMenuModel::label(TopEdgeMenuAction::Quit) == "quit");
    assert(TopEdgeMenuModel::label(TopEdgeMenuAction::Options) == "options");

    assert(TopEdgeMenuModel::action_for_selection(3) ==
           TopEdgeMenuAction::Automap);

    bool rejected = false;
    try {
        (void)TopEdgeMenuModel::action_for_selection(8);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected);
}
