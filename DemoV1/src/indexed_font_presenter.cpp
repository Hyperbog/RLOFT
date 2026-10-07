#include "rloft/indexed_font_presenter.hpp"

#include <cstddef>
#include <stdexcept>

namespace rloft {

int IndexedFontPresenter::measure(
    const Type2Font& font,
    std::string_view text) {
    int width = 0;
    for (const unsigned char ch : text) {
        if (ch >= font.widths.size() ||
            ch >= font.raster.count) {
            throw std::runtime_error(
                "text glyph exceeds RES0 font");
        }
        width += font.widths[ch];
    }
    return width;
}

void IndexedFontPresenter::draw(
    IndexedFramebuffer& framebuffer,
    const Type2Font& font,
    int row,
    int column,
    std::string_view text) {
    for (const unsigned char ch : text) {
        if (ch >= font.widths.size() ||
            ch >= font.raster.count) {
            throw std::runtime_error(
                "text glyph exceeds RES0 font");
        }
        framebuffer.draw_frame(
            font.raster,
            ch,
            row,
            column);
        column += font.widths[ch];
    }
}

void IndexedFontPresenter::draw_centered(
    IndexedFramebuffer& framebuffer,
    const Type2Font& font,
    int row,
    int column,
    int span,
    std::string_view text) {
    if (span < 0) {
        throw std::invalid_argument(
            "negative centered text span");
    }
    const auto width =
        measure(font, text);
    draw(
        framebuffer,
        font,
        row,
        column + (span - width) / 2,
        text);
}

}  // namespace rloft
