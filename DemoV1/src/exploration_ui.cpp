#include "rloft/exploration_ui.hpp"

#include "rloft/exploration_state.hpp"

#include <stdexcept>

namespace rloft {

void ExplorationUiController::update_mouse(
    int mouse_x,
    int mouse_y) noexcept {
    if (state_.screen != ExplorationUiScreen::Viewport) {
        state_.top_edge_menu_visible = false;
        state_.top_edge_hover = -1;
        return;
    }

    state_.top_edge_menu_visible =
        TopEdgeMenuModel::active_for_mouse_y(mouse_y);
    state_.top_edge_hover =
        state_.top_edge_menu_visible
            ? TopEdgeMenuModel::selection_from_mouse_x(mouse_x)
            : -1;
}

ExplorationUiScreen ExplorationUiController::screen_for_action(
    TopEdgeMenuAction action) noexcept {
    switch (action) {
        case TopEdgeMenuAction::Rest:
            return ExplorationUiScreen::Rest;
        case TopEdgeMenuAction::Pray:
            return ExplorationUiScreen::Pray;
        case TopEdgeMenuAction::Memorize:
            return ExplorationUiScreen::Memorize;
        case TopEdgeMenuAction::Automap:
            return ExplorationUiScreen::Automap;
        case TopEdgeMenuAction::Load:
            return ExplorationUiScreen::Load;
        case TopEdgeMenuAction::Save:
            return ExplorationUiScreen::Save;
        case TopEdgeMenuAction::Quit:
            return ExplorationUiScreen::QuitConfirm;
        case TopEdgeMenuAction::Options:
            return ExplorationUiScreen::Options;
    }
    return ExplorationUiScreen::Viewport;
}

bool ExplorationUiController::activate_top_edge_selection() {
    if (state_.screen != ExplorationUiScreen::Viewport ||
        !state_.top_edge_menu_visible ||
        state_.top_edge_hover < 0) {
        return false;
    }

    const auto action =
        TopEdgeMenuModel::action_for_selection(state_.top_edge_hover);
    state_.screen = screen_for_action(action);
    state_.top_edge_menu_visible = false;
    state_.top_edge_hover = -1;
    return true;
}

void ExplorationUiController::open_character(
    std::size_t actor_index) {
    if (actor_index >= PartyImage::kActorCount) {
        throw std::out_of_range("party actor index");
    }
    state_.selected_party_actor = actor_index;
    state_.screen = ExplorationUiScreen::Character;
    state_.top_edge_menu_visible = false;
    state_.top_edge_hover = -1;
}

void ExplorationUiController::open_inventory(
    std::size_t actor_index) {
    if (actor_index >= PartyImage::kActorCount) {
        throw std::out_of_range("party actor index");
    }
    state_.selected_party_actor = actor_index;
    state_.screen = ExplorationUiScreen::Inventory;
    state_.top_edge_menu_visible = false;
    state_.top_edge_hover = -1;
}

void ExplorationUiController::close_screen() noexcept {
    state_.screen = ExplorationUiScreen::Viewport;
    state_.top_edge_menu_visible = false;
    state_.top_edge_hover = -1;
}

}  // namespace rloft
