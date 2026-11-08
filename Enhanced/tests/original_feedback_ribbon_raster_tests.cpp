#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_feedback_ribbon_raster.hpp"
#include "rloft/original_feedback_ribbon_epoch.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <tuple>
#include <stdexcept>
#include <vector>

int main() {
    using Raster = rloft::OriginalFeedbackRibbonRaster;
    rloft::OriginalRandom rng(1U);
    rloft::OriginalFeedbackRibbonEpoch epoch;
    std::array<std::uint8_t,128> widths{};
    widths.fill(1U);
    widths['A'] = 80U;
    epoch.enter_modal_13118();
    epoch.source_f790_call(true,-1,widths,"AA",rng,
                           [] { return 100U; });
    const auto* prepared = epoch.present_vga_frame();
    assert(prepared && prepared->text_width == 160);
    assert(prepared->tiles.size() == 23U);
    const auto state_after_source_event = rng.state();
    using TileCall = std::tuple<int,int,int,int,int>;
    std::vector<TileCall> tiles;
    struct TextCall { int font,row,column,span,center; };
    std::vector<TextCall> texts;
    for (int redraw = 0; redraw < 1000; ++redraw) {
        Raster::replay(*prepared,
            [&](int archive,int resource,int frame,int row,int column) {
                tiles.emplace_back(archive,resource,frame,row,column);
            },
            [&](int font,int row,int column,int span,int center) {
                texts.push_back({font,row,column,span,center});
            });
        assert(rng.state() == state_after_source_event);
    }
    assert(tiles.size() == 23000U);
    assert(texts.size() == 1000U);
    assert(std::get<0>(tiles.front()) == 2); // RES2
    assert(std::get<1>(tiles.front()) == 3); // entry 3
    assert(std::get<2>(tiles.front()) == 0); // starting cap
    assert(std::get<3>(tiles.front()) == 0x62);
    assert(std::get<4>(tiles.front()) == 0x44);
    assert(std::get<2>(tiles[22]) == 5); // terminal cap
    assert(std::get<4>(tiles[22]) == 0xf4);
    for (std::size_t i=0;i<prepared->tiles.size();++i) {
        assert(std::get<2>(tiles[i]) == prepared->tiles[i].frame);
        assert(std::get<4>(tiles[i]) == prepared->tiles[i].column);
    }
    for (const auto& text : texts) {
        assert(text.font == 1);
        assert(text.row == 0x65);
        assert(text.column == 0);
        assert(text.span == 320);
        assert(text.center == 80);
    }

    // Exercise the actual native indexed blitter with six synthetic raster
    // frames and a native font descriptor. No original art is fabricated.
    rloft::Type1Raster ribbon{};
    ribbon.count = 6;
    ribbon.width = 8;
    ribbon.height = 3;
    ribbon.transparent_index = 0xFFU;
    ribbon.pixels.resize(6U*8U*3U);
    for (std::size_t i=0;i<ribbon.pixels.size();++i)
        ribbon.pixels[i] = static_cast<std::uint8_t>(10+i/24U);
    rloft::Type2Font font{};
    font.widths = widths;
    font.raster.count = 128;
    font.raster.width = 80;
    font.raster.height = 1;
    font.raster.transparent_index = 0U;
    font.raster.pixels.assign(128U*80U,0U);
    font.raster.pixels[static_cast<std::size_t>('A')*80U] = 200U;
    rloft::IndexedFramebuffer framebuffer{};
    framebuffer.clear(0U);
    Raster::draw_prepared(framebuffer,ribbon,font,"AA",*prepared);
    const auto& px = framebuffer.pixels();
    const auto at = [&](int y,int x) {
        return px[static_cast<std::size_t>(y)*320U+
                  static_cast<std::size_t>(x)];
    };
    assert(at(0x62,0x44) == 10U);  // original frame 0 at tile origin
    assert(at(0x62,0xf4) == 15U);  // original frame 5 at tile end
    assert(at(0x65,80) == 200U);  // 30508 centered font-one glyph A
    assert(at(0x65,160) == 200U); // second glyph + original advance 80
    assert(at(0x65,79) == 0U);
    assert(rng.state() == state_after_source_event);

    // Validation precedes all writes when a resource is missing frames.
    const auto unchanged = framebuffer.pixels();
    ribbon.count = 5;
    bool missing_frame = false;
    try {
        Raster::draw_prepared(framebuffer,ribbon,font,"AA",*prepared);
    } catch (const std::invalid_argument&) { missing_frame = true; }
    assert(missing_frame);
    assert(framebuffer.pixels() == unchanged);
}
