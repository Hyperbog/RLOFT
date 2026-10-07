#pragma once

#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"
#include "rloft/top_edge_menu.hpp"

namespace rloft {

struct TopEdgeMenuButtonLayout final {
    int x{};
    int y{};
    int width{};
    int height{};
    int frame{};
};

class TopEdgeMenuPresenter final {
public:
    explicit TopEdgeMenuPresenter(
        const RavenloftBootAssets& assets)
        : assets_(assets) {}

    [[nodiscard]] static int resource_id(
        std::uint16_t res0_raster_base) noexcept {
        // CODE.2 app12_draw_menu_row: 258DC + 49h.
        return static_cast<int>(res0_raster_base) + 0x49;
    }

    [[nodiscard]] static TopEdgeMenuButtonLayout button_layout(
        int selection,
        int hovered_selection,
        int pressed_selection = -1);

    // CODE.2 080AC/app12_draw_menu_row:
    //   RES0[base+49h] frame 0/1/2 at (y=0,x=selection*40)
    //   centered label at y=1Bh in the same 40-pixel span, font item 1.
    void draw(
        IndexedFramebuffer& framebuffer,
        int hovered_selection,
        int pressed_selection = -1) const;

private:
    void draw_centered_text(
        IndexedFramebuffer& framebuffer,
        int y,
        int left,
        int span,
        std::string_view text) const;

    const RavenloftBootAssets& assets_;
};

}  // namespace rloft
