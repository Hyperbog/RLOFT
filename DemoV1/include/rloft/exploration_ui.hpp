#pragma once

#include <cstddef>

#include "rloft/top_edge_menu.hpp"

namespace rloft {

enum class ExplorationUiScreen {
    Viewport,
    Rest,
    Pray,
    Memorize,
    Automap,
    Load,
    Save,
    QuitConfirm,
    Options,
    Character,
    Inventory,
};

struct ExplorationUiState final {
    ExplorationUiScreen screen{ExplorationUiScreen::Viewport};
    bool top_edge_menu_visible{};
    int top_edge_hover{-1};
    std::size_t selected_party_actor{};
};

class ExplorationUiController final {
public:
    [[nodiscard]] const ExplorationUiState& state() const noexcept {
        return state_;
    }

    void update_mouse(int mouse_x, int mouse_y) noexcept;
    bool activate_top_edge_selection();
    void open_character(std::size_t actor_index);
    void open_inventory(std::size_t actor_index);
    void close_screen() noexcept;

private:
    static ExplorationUiScreen screen_for_action(
        TopEdgeMenuAction action) noexcept;

    ExplorationUiState state_{};
};

}  // namespace rloft
