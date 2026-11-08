#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_f790_background_page.hpp"

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
    using Page = rloft::OriginalF790BackgroundPage;
    static_assert(Page::kSavedPixels == 0x1400,"original 3104C allocation");
    Page page;
    rloft::IndexedFramebuffer frame;
    frame.clear(0x11U);
    page.enter_modal_13118();
    assert(!page.ready());
    bool needs_capture=false;
    try { page.restore_2932c(frame); }
    catch (const std::logic_error&) { needs_capture=true; }
    assert(needs_capture);

    // 112EC draws its final actor-column pixels; capture 0x1400 bytes.
    frame.fill_rect(0x62,0,0x10,320,0x23U);
    frame.fill_rect(0x62,80,0x10,80,0x35U);
    page.capture_112ec(frame);
    assert(page.ready());
    // F790/F66C overlays only temporarily alter the visible VGA page.
    frame.fill_rect(0x62,0,0x10,320,0x77U);
    frame.fill_rect(0x61,0,1,320,0x55U);
    frame.fill_rect(0x72,0,1,320,0x66U);
    page.restore_2932c(frame);
    assert(pixel(frame,0x62,0)==0x23U);
    assert(pixel(frame,0x62,79)==0x23U);
    assert(pixel(frame,0x62,80)==0x35U);
    assert(pixel(frame,0x71,159)==0x35U);
    assert(pixel(frame,0x71,319)==0x23U);
    assert(pixel(frame,0x61,319)==0x55U);
    assert(pixel(frame,0x72,319)==0x66U);

    // Subsequent original F790 calls restore the SAME saved background.
    for (int i=0;i<1000;++i) {
        frame.fill_rect(0x62,0,0x10,320,0xA0U);
        page.restore_2932c(frame);
        assert(pixel(frame,0x62,80)==0x35U);
        assert(pixel(frame,0x62,319)==0x23U);
    }
    page.leave_modal();
    assert(!page.ready());
    needs_capture=false;
    try { page.restore_2932c(frame); }
    catch(const std::logic_error&) { needs_capture=true; }
    assert(needs_capture);

    // Re-entry cannot use the previous session's stale 7F68 snapshot.
    frame.clear(0x49U);
    page.enter_modal_13118();
    page.capture_112ec(frame);
    frame.clear(0x21U);
    page.restore_2932c(frame);
    assert(pixel(frame,0x62,80)==0x49U);
    assert(pixel(frame,0x71,319)==0x49U);
    assert(pixel(frame,0x61,80)==0x21U);
}
