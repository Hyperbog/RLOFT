#include "rloft/top_edge_menu_presenter.hpp"

#include <cassert>
#include <stdexcept>

int main() {
    assert(rloft::TopEdgeMenuPresenter::resource_id(15) == 88);

    auto b =
        rloft::TopEdgeMenuPresenter::button_layout(
            0, -1, -1);
    assert(b.x == 0);
    assert(b.y == 0);
    assert(b.width == 40);
    assert(b.height == 64);
    assert(b.frame == 0);

    b = rloft::TopEdgeMenuPresenter::button_layout(
        3, 3, -1);
    assert(b.x == 120);
    assert(b.frame == 1);

    b = rloft::TopEdgeMenuPresenter::button_layout(
        7, 7, 7);
    assert(b.x == 280);
    assert(b.frame == 2);

    // Pressed state wins when a button is both hovered and pressed.
    b = rloft::TopEdgeMenuPresenter::button_layout(
        4, 4, 4);
    assert(b.frame == 2);

    bool rejected = false;
    try {
        (void)rloft::TopEdgeMenuPresenter::button_layout(
            8, -1, -1);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected);
}
