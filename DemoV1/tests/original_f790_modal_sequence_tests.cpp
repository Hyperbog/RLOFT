#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_f790_modal_sequence.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace {
std::uint8_t pixel(const rloft::IndexedFramebuffer& fb,int y,int x) {
    return fb.pixels().at(static_cast<std::size_t>(y)*320U+
                          static_cast<std::size_t>(x));
}
}
int main() {
    rloft::IndexedFramebuffer screen;
    rloft::OriginalF790ModalSequence modal;
    rloft::OriginalRandom rng(0x12345678U);
    int tick_reads=0;
    auto tick=[&]() -> std::uint32_t {++tick_reads;return 0x89ABCDEFU;};
    rloft::Type1Raster tiles{};
    tiles.count=6;tiles.width=8;tiles.height=3;
    tiles.transparent_index=0xFFU;
    tiles.pixels.resize(6U*8U*3U);
    for (std::size_t i=0;i<tiles.pixels.size();++i)
        tiles.pixels[i]=static_cast<std::uint8_t>(10+i/24U);
    rloft::Type2Font font{};
    font.widths.fill(1U);font.widths['A']=80U;
    font.raster.count=128;font.raster.width=80;font.raster.height=1;
    font.raster.transparent_index=0U;
    font.raster.pixels.assign(128U*80U,0U);
    font.raster.pixels[static_cast<std::size_t>('A')*80U]=200U;

    modal.enter_modal_13118();
    bool refused=false;
    try {modal.source_event_f790(screen,true,-1,"AA",tiles,font,rng,tick);}
    catch (const std::logic_error&) {refused=true;}
    assert(refused && tick_reads==0 && modal.f790_event_count()==0U);
    screen.clear(0x20U);
    screen.fill_rect(0x62,0,16,320,0x30U);
    modal.capture_after_actor_columns_112ec(screen);
    // 112EC is a one-shot modal entry; a second capture would freeze
    // previously drawn feedback into the page used for 2932C restores.
    refused=false;
    try {modal.capture_after_actor_columns_112ec(screen);}
    catch(const std::logic_error&) {refused=true;}
    assert(refused);
    const auto initial_rng=rng.state();
    modal.source_event_f790(screen,false,-1,"",tiles,font,rng,tick);
    assert(modal.f790_event_count()==1U && rng.state()==initial_rng);
    assert(pixel(screen,0x62,0)==0x30U);
    assert(tick_reads==0);

    modal.source_event_f790(screen,true,-1,"AA",tiles,font,rng,tick);
    assert(modal.f790_event_count()==2U && tick_reads==0);
    assert(pixel(screen,0x62,0x44)==10U); // first F66C tile
    assert(pixel(screen,0x62,0xF4)==15U); // terminal F66C tile
    assert(pixel(screen,0x65,80)==200U); // original font 1 text
    const auto after_pending=rng.state();
    for(int i=0;i<1000;++i) {
        // New host framebuffer constructed at VGA cadence, but not a
        // source F790 event. Replay the cached ribbon without any RNG.
        screen.clear(0x20U);
        screen.fill_rect(0x62,0,16,320,0x30U);
        modal.replay_after_host_redraw(screen,tiles,font);
        assert(pixel(screen,0x62,0x44)==10U);
        assert(pixel(screen,0x65,80)==200U);
    }
    assert(rng.state()==after_pending && tick_reads==0);
    assert(modal.f790_event_count()==2U);

    modal.source_event_f790(screen,false,74,"AA",tiles,font,rng,tick);
    assert(modal.f790_event_count()==3U);
    assert(tick_reads==1 && rng.state()==0x89ABCDEFU);
    assert(pixel(screen,0x62,0x44)==10U);
    assert(pixel(screen,0x65,160)==200U);

    modal.source_event_f790(screen,false,-1,"",tiles,font,rng,tick);
    assert(modal.f790_event_count()==4U);
    assert(tick_reads==1 && rng.state()==0x89ABCDEFU);
    assert(pixel(screen,0x62,0x44)==0x30U); // 2932C erases old ribbon
    assert(pixel(screen,0x65,80)==0x30U);
    assert(pixel(screen,0x61,80)==0x20U); // no outside-row erasure

    modal.leave_modal();
    assert(!modal.captured());
    refused=false;
    try {modal.source_event_f790(screen,false,-1,"",tiles,font,rng,tick);}
    catch(const std::logic_error&) {refused=true;}
    assert(refused);
    screen.clear(0x42U);
    modal.enter_modal_13118();
    modal.capture_after_actor_columns_112ec(screen);
    screen.fill_rect(0x62,0,16,320,0x13U);
    modal.source_event_f790(screen,false,-1,"",tiles,font,rng,tick);
    assert(pixel(screen,0x62,0x44)==0x42U);
    assert(tick_reads==1);
}
