#include "rloft/automap_screen_presenter.hpp"

#include <stdexcept>

namespace rloft {

void AutomapScreenPresenter::draw_frame(
    IndexedFramebuffer& framebuffer,
    int local_resource,
    int frame,
    int row,
    int column) const {
    const auto raster =
        assets_.type1(
            0,
            static_cast<int>(res0_base_) +
                local_resource);
    if (frame < 0 || frame >= raster.count) {
        throw std::runtime_error(
            "automap chrome frame exceeds RES0 resource");
    }
    framebuffer.draw_frame(
        raster, frame, row, column);
}

void AutomapScreenPresenter::draw(
    IndexedFramebuffer& framebuffer,
    const ExplorationSessionState& session,
    const RendererTables& tables,
    const AutomapScreenState& state,
    bool annotation_overlay_enabled) const {
    // CODE.2 011A8: six copies of RES0 local 3Eh repaint the clipped map
    // field before 0139C emits tiles.
    for (int column = 0;
         column != 0x120;
         column += 0x60) {
        draw_frame(
            framebuffer,
            0x3E, 0,
            0, column);
        draw_frame(
            framebuffer,
            0x3E, 0,
            0xC8, column);
    }

    // CODE.2 01834 fixed chrome. RNG-selected 35h edge fillers and T-text
    // labels are intentionally separate rather than approximated.
    draw_frame(framebuffer, 0x35, 8,    0,     0);
    draw_frame(framebuffer, 0x35, 0x0B, 0,     0x138);
    draw_frame(framebuffer, 0x35, 4,    0x180, 0);
    draw_frame(framebuffer, 0x35, 7,    0x180, 0x138);
    draw_frame(framebuffer, 0x35, 0x10, 0,     0xF0);
    draw_frame(framebuffer, 0x35, 0x11, 0x180, 0xF0);

    draw_frame(framebuffer, 0x37, 0, 0x11D, 0xF0);
    draw_frame(framebuffer, 0x37, 1, 0x11D, 0x130);

    draw_frame(
        framebuffer,
        0x38,
        annotation_overlay_enabled ? 1 : 0,
        0x11A,
        0x100);

    int row = 0x136;
    for (int frame = 2;
         frame != 8;
         frame += 2, row += 0x1C) {
        draw_frame(
            framebuffer,
            0x38,
            frame,
            row,
            0x100);
    }

    draw_frame(framebuffer, 0x39, 0, 3,     0xF3);
    draw_frame(framebuffer, 0x3A, 0, 0xAE,  0x120);
    draw_frame(framebuffer, 0x3B, 0, 0xA8,  0xF8);
    draw_frame(framebuffer, 0x3F, 0, 0xF3,  0xF5);

    const AutomapViewportOrigin origin{
        state.column,
        state.row,
    };
    viewport_.draw_tiles(
        framebuffer,
        session,
        origin);

    if (annotation_overlay_enabled) {
        annotations_.draw(
            framebuffer,
            AutomapAnnotationModel::parse(
                session.level.automap_annotations),
            state.column,
            state.row);
    }

    viewport_.draw_player_marker(
        framebuffer,
        session,
        tables,
        origin);
}

}  // namespace rloft
