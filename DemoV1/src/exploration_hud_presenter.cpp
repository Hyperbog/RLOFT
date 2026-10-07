#include "rloft/exploration_hud_presenter.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>

namespace rloft {
namespace {

constexpr std::size_t kCode2DataFileBase = 0x75F24U;
constexpr std::size_t kPortraitMapsDgroup = 0xCF70U;

}  // namespace

ExplorationHudPresenter::ExplorationHudPresenter(
    const RavenloftBootAssets& assets)
    : assets_(assets),
      portrait_maps_(
          portrait_maps_from_code2(
              assets.source().read("CODE.2"))) {}

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

ExplorationHudPresenter::PortraitMaps
ExplorationHudPresenter::portrait_maps_from_code2(
    const std::vector<std::uint8_t>& code2) {
    constexpr std::size_t kBytes =
        6U * 256U;
    const auto start =
        kCode2DataFileBase +
        kPortraitMapsDgroup;

    if (start > code2.size() ||
        kBytes > code2.size() - start) {
        throw std::runtime_error(
            "CODE.2 is truncated before portrait remap tables");
    }

    PortraitMaps out{};
    for (std::size_t state = 0;
         state < out.size();
         ++state) {
        std::copy_n(
            code2.begin() +
                static_cast<std::ptrdiff_t>(
                    start + state * 256U),
            256,
            out[state].begin());
    }
    return out;
}

void ExplorationHudPresenter::draw_outline(
    IndexedFramebuffer& framebuffer,
    int row,
    int column,
    int rows,
    int columns,
    std::uint8_t color) {
    if (rows <= 0 || columns <= 0) {
        return;
    }

    // Portable chunky equivalent of 2E374's four 29CEC line calls.
    framebuffer.fill_rect(
        row, column, 1, columns, color);

    if (rows > 2) {
        framebuffer.fill_rect(
            row + 1, column,
            rows - 2, 1, color);
    }

    if (rows > 1) {
        framebuffer.fill_rect(
            row + rows - 1,
            column,
            1,
            columns,
            color);
    }

    if (rows > 2 && columns > 1) {
        framebuffer.fill_rect(
            row + 1,
            column + columns - 1,
            rows - 2,
            1,
            color);
    }
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

void ExplorationHudPresenter::draw_party_panels(
    IndexedFramebuffer& framebuffer,
    const std::array<PartyHudPanel, 4>& panels) const {
    for (const auto& panel : panels) {
        if (panel.inactive) {
            framebuffer.draw(
                assets_.type1(
                    0,
                    panel.inactive_resource_id),
                panel.inactive_y,
                panel.inactive_x);
            continue;
        }

        framebuffer.draw(
            assets_.type1(
                0,
                panel.panel_resource_id),
            panel.panel_y,
            panel.panel_x);

        if (panel.portrait_state < 0 ||
            panel.portrait_state >=
                static_cast<int>(
                    portrait_maps_.size())) {
            throw std::out_of_range(
                "party portrait state");
        }

        framebuffer.draw_frame_mapped(
            assets_.type1(
                1,
                panel.portrait_resource_id),
            0,
            panel.portrait_y,
            panel.portrait_x,
            portrait_maps_[
                static_cast<std::size_t>(
                    panel.portrait_state)]);

        if (panel.health_bar.drawable()) {
            framebuffer.fill_rect(
                panel.health_bar.y,
                panel.health_bar.x,
                panel.health_bar.rows,
                panel.health_bar.width,
                panel.health_bar.color);
        }

        if (panel.condition_outline) {
            draw_outline(
                framebuffer,
                0x138,
                panel.anchor_x + 4,
                0x50,
                0x30,
                0x43);
        }

        if (panel.status_outline) {
            draw_outline(
                framebuffer,
                0x139,
                panel.anchor_x + 5,
                0x4E,
                0x2E,
                0x53);
        }
    }
}

}  // namespace rloft
