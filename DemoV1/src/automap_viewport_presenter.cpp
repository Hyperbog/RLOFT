#include "rloft/automap_viewport_presenter.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {

AutomapViewportOrigin
AutomapViewportPresenter::initial_origin(
    const ExplorationSessionState& session) noexcept {
    // CODE.2 03634/app05_select_level_camera:
    // high-word A/B minus 18h, each clamped only at -40.
    int column =
        static_cast<std::int16_t>(
            session.coordinate_a >> 16U) -
        0x18;
    int row =
        static_cast<std::int16_t>(
            session.coordinate_b >> 16U) -
        0x18;

    if (column < -40) column = -40;
    if (row < -40) row = -40;

    return AutomapViewportOrigin{column, row};
}

AutomapTileDraw AutomapViewportPresenter::tile_draw(
    const LevelState& level,
    CellPosition cell,
    AutomapViewportOrigin origin) {
    if (!ExplorationWorldModel::in_bounds(
            level, cell)) {
        throw std::out_of_range(
            "automap viewport cell");
    }

    const auto index =
        ExplorationWorldModel::cell_index(
            level, cell);
    if (index >= level.automap_cache.size()) {
        throw std::runtime_error(
            "automap cache does not match level dimensions");
    }

    auto frame = level.automap_cache[index];
    if (frame == 0xFFU) {
        throw std::invalid_argument(
            "unexplored automap cell has no tile draw");
    }
    if (frame < 0x20U) {
        frame =
            AutomapPresentationModel::resolve_cell(
                level, cell);
    }

    const auto word =
        ExplorationWorldModel::cell_word(
            level, cell);
    const auto style_index =
        static_cast<std::uint8_t>(
            word & 0x003FU);
    const auto banks =
        EnvironmentViewModel::resource_banks(
            level);
    const auto style =
        banks.automap_style[style_index];

    return AutomapTileDraw{
        cell,
        kBaseY +
            (cell.column - origin.column) *
                kColumnPitch,
        kBaseX +
            (cell.row - origin.row) *
                kRowPitch,
        frame,
        static_cast<std::uint8_t>(
            style & 0xFFU),
        static_cast<std::uint8_t>(
            style >> 8U),
    };
}

std::size_t AutomapViewportPresenter::draw_tile_clipped(
    IndexedFramebuffer& framebuffer,
    const Type1Raster& raster,
    int frame,
    int y,
    int x,
    const std::array<std::uint8_t,256>& color_map) {
    if (frame<0 || frame>=raster.count || raster.width<=0 ||
        raster.height<=0) {
        throw std::out_of_range("automap tile frame or geometry invalid");
    }
    const auto width=static_cast<std::size_t>(raster.width);
    const auto height=static_cast<std::size_t>(raster.height);
    const auto pixels_per_frame=width*height;
    const auto index=static_cast<std::size_t>(frame);
    if (pixels_per_frame>raster.pixels.size() ||
        index>raster.pixels.size()/pixels_per_frame ||
        pixels_per_frame>raster.pixels.size()-index*pixels_per_frame) {
        throw std::invalid_argument("automap tile resource payload truncated");
    }
    const auto start=index*pixels_per_frame;
    auto& destination=framebuffer.mutable_pixels();
    std::size_t writes=0U;
    for (int yy=0;yy<raster.height;++yy) {
        const auto row=static_cast<long long>(y)+yy;
        if(row<kMapMinY || row>kMapMaxY)continue;
        for(int xx=0;xx<raster.width;++xx) {
            const auto col=static_cast<long long>(x)+xx;
            if(col<kMapMinX || col>kMapMaxX)continue;
            const auto value=raster.pixels[
                start+static_cast<std::size_t>(yy)*width+
                static_cast<std::size_t>(xx)];
            if(raster.transparent_index!=0xFFU &&
               value==raster.transparent_index)continue;
            destination[static_cast<std::size_t>(row)*
                IndexedFramebuffer::kWidth+
                static_cast<std::size_t>(col)]=color_map[value];
            ++writes;
        }
    }
    return writes;
}

void AutomapViewportPresenter::draw_tiles(
    IndexedFramebuffer& framebuffer,
    const ExplorationSessionState& session,
    AutomapViewportOrigin origin) const {
    const auto tiles =
        assets_.type1(
            0,
            tile_resource_id(
                assets_.init_u16(0x00)));

    std::array<std::uint8_t, 256> remap{};
    for (std::size_t i = 0;
         i < remap.size();
         ++i) {
        remap[i] =
            static_cast<std::uint8_t>(i);
    }

    const int end_column =
        origin.column + kVisibleColumns;
    const int end_row =
        origin.row + kVisibleRows;

    for (int column = origin.column;
         column < end_column;
         ++column) {
        if (column < 0 ||
            column >= session.level.width) {
            continue;
        }

        for (int row = origin.row;
             row < end_row;
             ++row) {
            if (row < 0 ||
                row >= session.level.height) {
                continue;
            }

            const CellPosition cell{
                column, row};
            const auto index =
                ExplorationWorldModel::cell_index(
                    session.level, cell);
            if (index >=
                session.level.automap_cache.size()) {
                throw std::runtime_error(
                    "automap cache does not match level dimensions");
            }
            if (session.level.automap_cache[
                    index] == 0xFFU) {
                continue;
            }

            const auto draw =
                tile_draw(
                    session.level,
                    cell,
                    origin);
            if (static_cast<int>(draw.frame)>=tiles.count) {
                throw std::runtime_error(
                    "automap tile frame exceeds RES0 resource");
            }

            // 759C is initialized to identity in 03634. 00B6C temporarily
            // replaces exactly identity entries B1/B5 with the two bytes from
            // the level's fifth 64-word lookup bank, then restores B1/B5.
            remap[0xB1] = draw.remap_b1;
            remap[0xB5] = draw.remap_b5;

            // Source palette remapping is preserved, but the pixel
            // blit cannot overwrite border/chrome outside the map field.
            (void)draw_tile_clipped(
                framebuffer,tiles,draw.frame,draw.screen_y,
                draw.screen_x,remap);

            remap[0xB1] = 0xB1U;
            remap[0xB5] = 0xB5U;
        }
    }
}

void AutomapViewportPresenter::draw_player_marker(
    IndexedFramebuffer& framebuffer,
    const ExplorationSessionState& session,
    const RendererTables& tables,
    AutomapViewportOrigin origin) const {
    const int player_column =
        static_cast<std::int16_t>(session.coordinate_a >> 16U);
    const int player_row =
        static_cast<std::int16_t>(session.coordinate_b >> 16U);

    if (player_column < origin.column ||
        player_column >= origin.column + kVisibleColumns ||
        player_row < origin.row ||
        player_row >= origin.row + kVisibleRows) {
        return;
    }

    const auto frac_a =
        static_cast<std::uint16_t>(session.coordinate_a);
    const auto frac_b =
        static_cast<std::uint16_t>(session.coordinate_b);

    const int start_y =
        kBaseY +
        (player_column - origin.column) * kColumnPitch +
        static_cast<int>(frac_a) / 0x2000;
    const int start_x =
        kBaseX +
        (player_row - origin.row) * kRowPitch +
        static_cast<int>(frac_b) / 0x3333;

    const auto tiles =
        assets_.type1(
            0,
            tile_resource_id(assets_.init_u16(0x00)));
    if (0x34 >= tiles.count) {
        throw std::runtime_error(
            "automap resource lacks player marker frame");
    }
    // The normal type-1 framebuffer clip covers the entire 320x400
    // image. On the automap, this can paint the marker into side controls
    // when the player is near the final visible tile. Clip to the actual
    // 48x47 cell map field instead.
    const auto stride=static_cast<std::size_t>(tiles.width)*
                      static_cast<std::size_t>(tiles.height);
    const auto offset=0x34U*stride;
    if (tiles.width<=0 || tiles.height<=0 ||
        offset>tiles.pixels.size() ||
        stride>tiles.pixels.size()-offset) {
        throw std::runtime_error("automap player marker raster invalid");
    }
    auto& framebuffer_pixels=framebuffer.mutable_pixels();
    for(int sy=0;sy<tiles.height;++sy) {
        const auto dy=start_y-2+sy;
        if(dy<kMapMinY || dy>kMapMaxY)continue;
        for(int sx=0;sx<tiles.width;++sx) {
            const auto dx=start_x+sx;
            if(dx<kMapMinX || dx>kMapMaxX)continue;
            const auto color=tiles.pixels[
                offset+static_cast<std::size_t>(sy)*tiles.width+sx];
            if(tiles.transparent_index!=0xFFU &&
               color==tiles.transparent_index)continue;
            framebuffer_pixels[
                static_cast<std::size_t>(dy)*IndexedFramebuffer::kWidth+
                static_cast<std::size_t>(dx)]=color;
        }
    }

    const auto step =
        ExplorationMotionModel::direction_vector(
            0x00010000U,
            session.facing,
            false,
            tables);

    std::uint32_t acc_y =
        static_cast<std::uint32_t>(
            static_cast<std::uint16_t>(start_y))
        << 16U;
    std::uint32_t acc_x =
        static_cast<std::uint32_t>(
            static_cast<std::uint16_t>(start_x))
        << 16U;

    for (int i = 0; i < 8; ++i) {
        acc_y += step.delta_x;
        acc_x += step.delta_y;
    }

    const int end_y =
        static_cast<std::int16_t>(acc_y >> 16U);
    const int end_x =
        static_cast<std::int16_t>(acc_x >> 16U);

    // Enhanced PC map: preserve the visible portion of the facing
    // arrow when the tip crosses the map edge. The bounded Bresenham
    // painter already clips safely to the map field; returning here
    // made the entire marker arrow disappear at some edge positions.
    draw_line(
        framebuffer,
        start_y,
        start_x,
        end_y,
        end_x,
        0xFFU,
        true);
}

void AutomapViewportPresenter::draw_line(
    IndexedFramebuffer& framebuffer,
    int y0,
    int x0,
    int y1,
    int x1,
    std::uint8_t color,
    bool clip_to_map) {
    int dc = x1 - x0;

    auto put = [&](int y, int x) {
        if (clip_to_map &&
            (y<kMapMinY || y>kMapMaxY ||
             x<kMapMinX || x>kMapMaxX))return;
        if (y >= 0 &&
            y < IndexedFramebuffer::kHeight &&
            x >= 0 &&
            x < IndexedFramebuffer::kWidth) {
            framebuffer.mutable_pixels()[
                static_cast<std::size_t>(y) *
                    IndexedFramebuffer::kWidth +
                static_cast<std::size_t>(x)] =
                color;
        }
    };

    if (dc == 0) {
        int dr = y1 - y0;
        if (dr < 0) {
            dr = -dr;
            y0 = y1;
        }
        for (int i = 0; i <= dr; ++i) {
            put(y0 + i, x0);
        }
        return;
    }

    if (dc < 0) {
        dc = -dc;
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dr = y1 - y0;
    if (dr == 0) {
        for (int x = x0; x <= x1; ++x) {
            put(y0, x);
        }
        return;
    }

    const int row_step = dr < 0 ? -1 : 1;
    if (dr < 0) dr = -dr;

    const bool row_major = dr > dc;
    const int major = row_major ? dr : dc;
    const int minor = row_major ? dc : dr;
    const int twice = minor * 2;
    int error = twice - major;
    const int diagonal = error - major;

    int y = y0;
    int x = x0;
    for (int i = 0; i <= major; ++i) {
        put(y, x);

        if (row_major) {
            y += row_step;
            if (error < 0) {
                error += twice;
            } else {
                error += diagonal;
                ++x;
            }
        } else {
            ++x;
            if (error < 0) {
                error += twice;
            } else {
                error += diagonal;
                y += row_step;
            }
        }
    }
}

}  // namespace rloft
