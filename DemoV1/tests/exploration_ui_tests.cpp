#include "rloft/exploration_ui.hpp"

#include <cassert>
#include <stdexcept>

int main() {
    using rloft::ExplorationUiController;
    using rloft::ExplorationUiScreen;

    ExplorationUiController ui;
    assert(ui.state().screen == ExplorationUiScreen::Viewport);

    ui.update_mouse(121, 20);
    assert(ui.state().top_edge_menu_visible);
    assert(ui.state().top_edge_hover == 3);
    assert(ui.activate_top_edge_selection());
    assert(ui.state().screen == ExplorationUiScreen::Automap);
    assert(!ui.state().top_edge_menu_visible);

    ui.update_mouse(0, 0);
    assert(!ui.state().top_edge_menu_visible);
    ui.close_screen();
    assert(ui.state().screen == ExplorationUiScreen::Viewport);

    ui.update_mouse(319, 63);
    assert(ui.state().top_edge_hover == 7);
    assert(ui.activate_top_edge_selection());
    assert(ui.state().screen == ExplorationUiScreen::Options);

    ui.close_screen();
    ui.update_mouse(245, 10);
    assert(ui.state().top_edge_hover == 6);
    assert(ui.activate_top_edge_selection());
    assert(ui.state().screen == ExplorationUiScreen::QuitConfirm);

    ui.open_character(2);
    assert(ui.state().screen == ExplorationUiScreen::Character);
    assert(ui.state().selected_party_actor == 2);

    ui.open_inventory(1);
    assert(ui.state().screen == ExplorationUiScreen::Inventory);
    assert(ui.state().selected_party_actor == 1);

    bool rejected = false;
    try {
        ui.open_inventory(4);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected);
}
