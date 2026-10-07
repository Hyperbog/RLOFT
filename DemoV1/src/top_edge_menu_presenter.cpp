#include "rloft/top_edge_menu_presenter.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>

namespace rloft {

TopEdgeMenuButtonLayout TopEdgeMenuPresenter::button_layout(
    int selection,
    int hovered_selection,
    int pressed_selection) {
    if (selection < 0 ||
        selection >= TopEdgeMenuModel::kEntryCount) {
        throw std::out_of_range(
            "top-edge menu button selection");
    }

    int frame = 0;
    if (selection == pressed_selection) {
        frame = 2;
    } else if (selection == hovered_selection) {
        frame = 1;
    }

    return TopEdgeMenuButtonLayout{
        selection * TopEdgeMenuModel::kEntryWidth,
        0,
        TopEdgeMenuModel::kEntryWidth,
        TopEdgeMenuModel::kHeight,
        frame,
    };
}

void TopEdgeMenuPresenter::draw_centered_text(
    IndexedFramebuffer& framebuffer,
    int y,
    int left,
    int span,
    std::string_view text) const {
    if (text.empty()) return;

    const auto font = assets_.res0_font(1);
    int width = 0;
    for (const unsigned char ch :
         std::string(text)) {
        if (ch >= font.widths.size() ||
            ch >= font.raster.count) {
            throw std::runtime_error(
                "top-edge menu label exceeds RES0 font");
        }
        width += font.widths[ch];
    }

    int x = left + (span - width) / 2;
    for (const unsigned char ch :
         std::string(text)) {
        framebuffer.draw_frame(
            font.raster,
            ch,
            y,
            x);
        x += font.widths[ch];
    }
}

void TopEdgeMenuPresenter::draw(
    IndexedFramebuffer& framebuffer,
    int hovered_selection,
    int pressed_selection) const {
    const auto buttons =
        assets_.type1(
            0,
            resource_id(
                assets_.init_u16(0x00)));

    if (buttons.count < 3) {
        throw std::runtime_error(
            "top-edge menu resource lacks three button states");
    }

    for (int selection = 0;
         selection < TopEdgeMenuModel::kEntryCount;
         ++selection) {
        const auto layout =
            button_layout(
                selection,
                hovered_selection,
                pressed_selection);

        framebuffer.draw_frame(
            buttons,
            layout.frame,
            layout.y,
            layout.x);

        const auto action =
            TopEdgeMenuModel::action_for_selection(
                selection);
        draw_centered_text(
            framebuffer,
            0x1B,
            layout.x,
            layout.width,
            TopEdgeMenuModel::label(action));
    }
}

}  // namespace rloft
