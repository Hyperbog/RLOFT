#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "rloft/party_hud.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"

namespace rloft {

struct ExplorationHudBaseLayout final {
    int upper_resource_id{};
    int lower_resource_id{};
    int upper_y{};
    int lower_y{};
    int x{};
};

class ExplorationHudPresenter final {
public:
    using PortraitMap =
        std::array<std::uint8_t, 256>;
    using PortraitMaps =
        std::array<PortraitMap, 6>;

    explicit ExplorationHudPresenter(
        const RavenloftBootAssets& assets);

    [[nodiscard]] static ExplorationHudBaseLayout base_layout(
        std::uint16_t res0_raster_base,
        int mode);

    // Certified CODE.2 data object begins at physical file offset 0x75F24.
    // The six 256-byte portrait-state maps consumed by 1F494 begin at
    // DGROUP CF70.
    [[nodiscard]] static PortraitMaps portrait_maps_from_code2(
        const std::vector<std::uint8_t>& code2);

    static void draw_outline(
        IndexedFramebuffer& framebuffer,
        int row,
        int column,
        int rows,
        int columns,
        std::uint8_t color);

    void draw_base(
        IndexedFramebuffer& framebuffer,
        int mode) const;

    // Faithful read-only portion of CODE.2 1F494. This draws the inactive
    // panel or normal portrait/frame, HP bar, and status outlines. The
    // combat/action-hand widgets handled by 22F40/1EA90 remain excluded.
    void draw_party_panels(
        IndexedFramebuffer& framebuffer,
        const std::array<PartyHudPanel, 4>& panels) const;

    [[nodiscard]] const PortraitMaps& portrait_maps() const noexcept {
        return portrait_maps_;
    }

private:
    const RavenloftBootAssets& assets_;
    PortraitMaps portrait_maps_{};
};

}  // namespace rloft
