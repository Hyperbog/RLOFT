#pragma once

#include <string_view>

#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"

namespace rloft {

class IndexedFontPresenter final {
public:
    [[nodiscard]] static int measure(
        const Type2Font& font,
        std::string_view text);

    static void draw(
        IndexedFramebuffer& framebuffer,
        const Type2Font& font,
        int row,
        int column,
        std::string_view text);

    static void draw_right(
        IndexedFramebuffer& framebuffer,
        const Type2Font& font,
        int row,
        int right,
        std::string_view text);

    static void draw_centered(
        IndexedFramebuffer& framebuffer,
        const Type2Font& font,
        int row,
        int column,
        int span,
        std::string_view text);
};

}  // namespace rloft
