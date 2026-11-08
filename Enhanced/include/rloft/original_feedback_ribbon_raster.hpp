#pragma once

#include "rloft/indexed_font_presenter.hpp"
#include "rloft/original_feedback_ribbon_text.hpp"
#include "rloft/original_party_modal_blit.hpp"

#include <stdexcept>
#include <string_view>
#include <utility>

namespace rloft {

// Replay original F66C's graphics into the indexed framebuffer, using only
// preselected frame indices. This intentionally never reads or advances RNG.
// F790's preceding 2932C background restore is a separate event dependency.
class OriginalFeedbackRibbonRaster final {
public:
    using Layout = OriginalFeedbackRibbonLayout;
    using Prepared = OriginalFeedbackRibbonText::Prepared;

    // Directly mirrors F6E4..F748 (2975C) then F748..F75B (30508).
    // The two sinks allow exact source call-order testing without graphics
    // fixtures or recomputing randomness at VGA refresh rate.
    template<class DrawTile,class DrawText>
    static void replay(
        const Prepared& prepared,
        DrawTile&& tile,
        DrawText&& text) {
        for (const auto& step : prepared.tiles) {
            tile(Layout::kArchiveSlot, Layout::kResourceId,
                 step.frame, Layout::kTileRow, step.column);
        }
        text(Layout::kFont, Layout::kTextRow,
             Layout::kTextColumn, Layout::kTextWidth,
             prepared.text_column);
    }

    // A functional native indexed-raster adapter. Call ONLY with art
    // prepared at an authentic F790 event. RES2 entry 3 contains six
    // source-selected frames; font1 comes from unchanged RES0 font 1.
    static void draw_prepared(
        IndexedFramebuffer& framebuffer,
        const Type1Raster& ribbon_res2_3,
        const Type2Font& font1,
        std::string_view message,
        const Prepared& prepared) {
        if (ribbon_res2_3.count < 6)
            throw std::invalid_argument("CODE.2 F66C RES2:3 has fewer than six frames");
        // Check the original font widths and native raster glyph domain
        // before mutating framebuffer pixels. The epoch text and metrics
        // must not silently disagree with the current unmodified assets.
        if (OriginalFeedbackRibbonText::measure(font1.widths,message) !=
                prepared.text_width ||
            IndexedFontPresenter::measure(font1,message) !=
                prepared.text_width)
            throw std::invalid_argument("CODE.2 F66C prepared text/font mismatch");
        replay(prepared,
            [&](int archive,int resource,int frame,int row,int column) {
                (void)archive; (void)resource;
                OriginalPartyModalBlit::frame_2975c(
                    framebuffer,ribbon_res2_3,frame,row,column);
            },
            [&](int font,int row,int column,int span,int centered_column) {
                (void)font; (void)column; (void)span;
                IndexedFontPresenter::draw(
                    framebuffer,font1,row,centered_column,message);
            });
    }
};

}  // namespace rloft
