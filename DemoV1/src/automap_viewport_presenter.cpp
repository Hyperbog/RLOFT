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
            if (draw.frame >=
                static_cast<std::uint8_t>(
                    tiles.count)) {
                throw std::runtime_error(
                    "automap tile frame exceeds RES0 resource");
            }

            // 759C is initialized to identity in 03634. 00B6C temporarily
            // replaces exactly identity entries B1/B5 with the two bytes from
            // the level's fifth 64-word lookup bank, then restores B1/B5.
            remap[0xB1] = draw.remap_b1;
            remap[0xB5] = draw.remap_b5;

            framebuffer.draw_frame_mapped(
                tiles,
                draw.frame,
                draw.screen_y,
                draw.screen_x,
                remap);

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
    framebuffer.draw_frame(
        tiles,
        0x34,
        start_y - 2,
        start_x);

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

    if (end_y < 0 || end_x < 0 || end_y > 0x18F) {
        return;
    }

    draw_line(
        framebuffer,
        start_y,
        start_x,
        end_y,
        end_x,
        0xFFU);
}

void AutomapViewportPresenter::draw_line(
    IndexedFramebuffer& framebuffer,
    int y0,
    int x0,
    int y1,
    int x1,
    std::uint8_t color) {
    int dc = x1 - x0;

    auto put = [&](int y, int x) {
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
