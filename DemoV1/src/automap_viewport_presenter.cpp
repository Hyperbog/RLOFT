#include "rloft/automap_viewport_presenter.hpp"

#include <algorithm>
#include <cstddef>
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

}  // namespace rloft
