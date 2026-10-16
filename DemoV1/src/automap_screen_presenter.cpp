#include "rloft/automap_screen_presenter.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>

namespace rloft {

// Certified object-2 CODE.2 DGROUP begins at physical file 75F24h.
// Original 01834 -> 01928 indexes this 13-byte table by EBX and
// passes its byte as the RES0+34h frame in 2975C.
std::array<std::uint8_t,13>
AutomapScreenPresenter::legend_frames_from_code2(
    const std::vector<std::uint8_t>& code2) {
    constexpr std::size_t kDgroup=0x75F24U;
    constexpr std::size_t kFrames=0x5FCCU;
    constexpr std::size_t kCount=13U;
    if(code2.size()<kDgroup+kFrames+kCount)
        throw std::runtime_error("original CODE.2 automap legend truncated");
    std::array<std::uint8_t,kCount> frames{};
    std::copy_n(code2.begin()+static_cast<std::ptrdiff_t>(
        kDgroup+kFrames),kCount,frames.begin());
    return frames;
}

bool AutomapScreenPresenter::legend_entry_visible(
    std::size_t index,std::uint8_t mode) noexcept {
    return index<13U && (index!=9U || mode==0xFFU);
}

void AutomapScreenPresenter::draw_original_legend(
    IndexedFramebuffer& framebuffer,std::uint8_t mode) const {
    // Original 018E6..01950:
    // ESI=1Ah (VGA row), EDI=E2h (T string index), EBX=0..12.
    // 1C368 dereferences T's DWORD index and XORs text with DCh;
    // RavenloftBootAssets::text performs the same original decode.
    // 2975C draws source frame [5FCC+EBX] at row ESI / column F6h;
    // 3060C draws font1 string at ESI / column FFh.
    const auto tiles=assets_.type1(
        0,static_cast<int>(res0_base_)+0x34);
    for(std::size_t index=0;index<legend_frames_.size();++index) {
        if(!legend_entry_visible(index,mode))continue;
        const int row=0x1A+static_cast<int>(index)*0x0A;
        const int icon=static_cast<int>(legend_frames_[index]);
        if(icon<0 || icon>=tiles.count)
            throw std::runtime_error(
                "original CODE.2 automap legend frame missing from RES0");
        framebuffer.draw_frame(tiles,icon,row,0xF6);
        IndexedFontPresenter::draw(
            framebuffer,legend_font_,row,0xFF,
            assets_.text(0xE2+static_cast<int>(index)));
    }
}

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
    // The native viewport retains its previous frame between modal
    // transitions. Clear it before assembling the automap so undrawn
    // areas cannot leak old trees, terrain, or UI pixels through
    // transparent RES0 chrome and unexplored map cells.
    framebuffer.clear(0U);

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

    // CODE.2 01834 paints thirteen genuine RES0 map symbols and
    // matching 1C368 T labels in the right-hand KEY panel. The
    // previously incomplete host omitted the entire original loop.
    draw_original_legend(framebuffer,session.level.header[0x290U]);

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
