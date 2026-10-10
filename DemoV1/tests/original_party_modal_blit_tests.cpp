#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_party_modal_blit.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {
std::uint8_t pixel(const rloft::IndexedFramebuffer& fb,
                   int row,int column) {
    return fb.pixels().at(
        static_cast<std::size_t>(row)*320U+
        static_cast<std::size_t>(column));
}
void capture_draw(void* ctx,const rloft::IndexedDrawAuditCall& c) {
    static_cast<std::vector<rloft::IndexedDrawAuditCall>*>(ctx)->push_back(c);
}
}

int main() {
    // First-party Level22 diagnostics confirmed RES0 contains real
    // modal rasters encoded as 200 one-row frames of 320 pixels.
    // Original CODE.2 0FF54 -> 2E510 -> 2E4A4 iterates ALL 200.
    rloft::Type1Raster original{};
    original.count=200;
    original.width=320;
    original.height=1;
    original.transparent_index=0xFFU;
    original.pixels.assign(320U*200U,0U);
    for(std::size_t y=0U;y<200U;++y)
        for(std::size_t x=0U;x<320U;++x)
            original.pixels[y*320U+x]=
                static_cast<std::uint8_t>(y+1U);

    rloft::IndexedFramebuffer fb;
    fb.clear(0U);
    std::vector<rloft::IndexedDrawAuditCall> audit;
    fb.set_draw_audit(&capture_draw,&audit);
    rloft::OriginalPartyModalBlit::complete_2e510(
        fb,original,0,0);
    assert(pixel(fb,0,0)==1U);
    assert(pixel(fb,1,0)==2U);
    assert(pixel(fb,69,56)==70U);
    assert(pixel(fb,113,160)==114U);
    assert(pixel(fb,199,319)==200U);
    assert(pixel(fb,200,1)==0U);
    assert(audit.size()==200U);
    for(std::size_t i=0U;i<audit.size();++i) {
        assert(audit[i].operation[0]=='t');
        assert(audit[i].frame==static_cast<int>(i));
        assert(audit[i].y==static_cast<int>(i));
        assert(audit[i].x==0);
        assert(audit[i].width==320);
        assert(audit[i].height==1);
    }

    // CODE.2 1119F -> 2975C emits a ONE-frame chrome badge, not
    // every source raster frame. Do not globally replace draw_frame.
    fb.clear(0U);
    audit.clear();
    rloft::OriginalPartyModalBlit::frame_2975c(
        fb,original,17,0x45,0x38);
    assert(pixel(fb,0x45,0x38)==18U);
    assert(pixel(fb,0x45,319)==18U);
    assert(pixel(fb,0x44,0x38)==0U);
    assert(pixel(fb,0x46,0x38)==0U);
    assert(pixel(fb,0x45,0x37)==0U);
    assert(audit.size()==1U);
    assert(audit.front().frame==17);
    assert(audit.front().y==0x45);
    assert(audit.front().x==0x38);

    // Original 2E4A4 advances by descriptor WORD+4, the per-frame
    // raster HEIGHT; nontrivial 2-row frames clip in the native VGA
    // 320x400 viewport without wrapping to the top of the screen.
    rloft::Type1Raster two_rows{};
    two_rows.count=3;
    two_rows.width=8;
    two_rows.height=2;
    two_rows.transparent_index=0xFFU;
    two_rows.pixels.assign(3U*8U*2U,0U);
    for(std::size_t frame=0;frame<3U;++frame)
        for(std::size_t i=0;i<16U;++i)
            two_rows.pixels[frame*16U+i]=
                static_cast<std::uint8_t>(frame+50U);
    fb.clear(0U);
    audit.clear();
    rloft::OriginalPartyModalBlit::complete_2e510(
        fb,two_rows,396,79);
    assert(pixel(fb,396,79)==50U);
    assert(pixel(fb,397,86)==50U);
    assert(pixel(fb,398,80)==51U);
    assert(pixel(fb,399,80)==51U);
    assert(pixel(fb,0,80)==0U);
    assert(pixel(fb,399,87)==0U);
    assert(audit.size()==3U);
    assert(audit[0].y==396);
    assert(audit[1].y==398);
    assert(audit[2].y==400);
    // Destroying/clearing the optional observer changes no game state.
    fb.set_draw_audit(nullptr,nullptr);
}
