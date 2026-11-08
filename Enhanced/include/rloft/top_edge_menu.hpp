#pragma once

#include <array>
#include <string_view>

namespace rloft {

enum class TopEdgeMenuAction {
    Rest = 0,
    Pray = 1,
    Memorize = 2,
    Automap = 3,
    Load = 4,
    Save = 5,
    Quit = 6,
    Options = 7,
};

class TopEdgeMenuModel final {
public:
    static constexpr int kHeight = 64;
    static constexpr int kWidth = 320;
    static constexpr int kEntryWidth = 40;
    static constexpr int kEntryCount = 8;

    [[nodiscard]] static bool active_for_mouse_y(int mouse_y) noexcept;
    [[nodiscard]] static int selection_from_mouse_x(int mouse_x) noexcept;
    [[nodiscard]] static int selection_from_mouse(
        int mouse_x,
        int mouse_y) noexcept;
    [[nodiscard]] static std::string_view label(
        TopEdgeMenuAction action) noexcept;
    [[nodiscard]] static TopEdgeMenuAction action_for_selection(
        int selection);

private:
    static constexpr std::array<std::string_view, kEntryCount> kLabels{{
        "rest",
        "pray",
        "memorize",
        "automap",
        "load",
        "save",
        "quit",
        "options",
    }};
};

}  // namespace rloft
