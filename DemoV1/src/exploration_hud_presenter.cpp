#include "rloft/exploration_hud_presenter.hpp"

#include <stdexcept>

namespace rloft {

ExplorationHudBaseLayout ExplorationHudPresenter::base_layout(
    std::uint16_t res0_raster_base,
    int mode) {
    if (mode < 0) {
        throw std::out_of_range(
            "negative exploration presentation mode");
    }

    const int group =
        static_cast<int>(res0_raster_base) +
        mode * 3;

    return ExplorationHudBaseLayout{
        group + 1,
        group + 2,
        0,
        0xC8,
        0,
    };
}

void ExplorationHudPresenter::draw_base(
    IndexedFramebuffer& framebuffer,
    int mode) const {
    const auto layout =
        base_layout(
            assets_.init_u16(0x00),
            mode);

    framebuffer.draw(
        assets_.type1(
            0, layout.upper_resource_id),
        layout.upper_y,
        layout.x);

    framebuffer.draw(
        assets_.type1(
            0, layout.lower_resource_id),
        layout.lower_y,
        layout.x);
}

}  // namespace rloft
