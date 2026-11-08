#pragma once

#include <array>
#include <cstdint>

#include "rloft/automap_presentation.hpp"
#include "rloft/environment_view.hpp"
#include "rloft/exploration_motion.hpp"
#include "rloft/exploration_session.hpp"
#include "rloft/renderer_tables.hpp"
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
    // The 48x47-cell tile field occupies rows 6..389, columns 4..238.
    // The player arrow/marker must not overwrite the right-hand map
    // controls or the perimeter chrome on a near-edge position.
    static constexpr int kMapMinY = kBaseY;
    static constexpr int kMapMaxY = kBaseY + kVisibleColumns*kColumnPitch - 1;
    static constexpr int kMapMinX = kBaseX;
    static constexpr int kMapMaxX = kBaseX + kVisibleRows*kRowPitch - 1;

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

    // Keep decoded RES0 terrain tiles inside the native map field.
    // The unbounded draw_frame_mapped() can overwrite the surrounding
    // automap controls when the final visible cell crosses an edge.
    static std::size_t draw_tile_clipped(
        IndexedFramebuffer& framebuffer,
        const Type1Raster& raster,
        int frame,
        int y,
        int x,
        const std::array<std::uint8_t,256>& color_map);


    void draw_player_marker(
        IndexedFramebuffer& framebuffer,
        const ExplorationSessionState& session,
        const RendererTables& tables,
        AutomapViewportOrigin origin) const;

    static void draw_line(
        IndexedFramebuffer& framebuffer,
        int y0,
        int x0,
        int y1,
        int x1,
        std::uint8_t color,
        bool clip_to_map = false);

private:
    const RavenloftBootAssets& assets_;
};

}  // namespace rloft
