#pragma once

#include <array>
#include <cstdint>

#include "rloft/automap_presentation.hpp"
#include "rloft/environment_view.hpp"
#include "rloft/exploration_session.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"

namespace rloft {

struct AutomapViewportOrigin final {
    int column{};
    int row{};
};

struct AutomapTileDraw final {
    CellPosition cell{};
    int screen_y{};
    int screen_x{};
    std::uint8_t frame{};
    std::uint8_t remap_b1{};
    std::uint8_t remap_b5{};
};

class AutomapViewportPresenter final {
public:
    static constexpr int kVisibleColumns = 0x30;
    static constexpr int kVisibleRows = 0x2F;
    static constexpr int kColumnPitch = 8;
    static constexpr int kRowPitch = 5;
    static constexpr int kBaseY = 6;
    static constexpr int kBaseX = 4;

    explicit AutomapViewportPresenter(
        const RavenloftBootAssets& assets)
        : assets_(assets) {}

    [[nodiscard]] static int tile_resource_id(
        std::uint16_t res0_raster_base) noexcept {
        // CODE.2 00B6C: runtime RES0 base 258DC + local 34h.
        return static_cast<int>(res0_raster_base) + 0x34;
    }

    [[nodiscard]] static AutomapViewportOrigin initial_origin(
        const ExplorationSessionState& session) noexcept;

    [[nodiscard]] static AutomapTileDraw tile_draw(
        const LevelState& level,
        CellPosition cell,
        AutomapViewportOrigin origin);

    // Exact 0139C/00B6C first pass only. This deliberately omits 00CB0 world
    // record overlays (NPC/items) per the current non-combat milestone.
    void draw_tiles(
        IndexedFramebuffer& framebuffer,
        const ExplorationSessionState& session,
        AutomapViewportOrigin origin) const;

private:
    const RavenloftBootAssets& assets_;
};

}  // namespace rloft
