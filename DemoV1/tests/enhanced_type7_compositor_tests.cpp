#ifdef NDEBUG
#undef NDEBUG
#endif

#include "rloft/enhanced_type7_compositor.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

rloft::Type7NativeRaster sprite() {
    rloft::Type7NativeRaster s{};
    s.width=2U;
    s.height=128U;
    s.pixels.assign(256U,0U);
    s.mask.assign(256U,0U);
    s.pixels[64U*2U+0U]=42U;
    s.mask[64U*2U+0U]=0xFFU;
    s.pixels[65U*2U+1U]=83U;
    s.mask[65U*2U+1U]=0xFFU;
    return s;
}

std::uint8_t pixel(const rloft::IndexedFramebuffer& fb,int y,int x) {
    return fb.pixels()[static_cast<std::size_t>(y)*
        rloft::IndexedFramebuffer::kWidth+static_cast<std::size_t>(x)];
}

} // namespace

int main() {
    using rloft::EnhancedType7Compositor;
    using rloft::Type7VisibleColumn;
    using rloft::IndexedFramebuffer;
    const std::vector<Type7VisibleColumn> columns={
        {0U,0U,0U}, {1U,1U,255U}
    };
    auto source=sprite();
    std::array<std::uint16_t,129U> steps{};
    steps.fill(1U);
    IndexedFramebuffer fb{};
    fb.clear(7U);
    auto count=EnhancedType7Compositor::draw(fb,source,columns,steps);
    assert(count==2U);
    assert(pixel(fb,132,32)==42U);
    assert(pixel(fb,133,287)==83U);
    assert(pixel(fb,131,32)==7U);
    assert(pixel(fb,133,32)==7U);
    assert(pixel(fb,132,287)==7U);
    assert(pixel(fb,133,31)==7U);
    assert(pixel(fb,133,288)==7U);

    // Scene planner computed source_top is per-object. The same pixel
    // should move with its authored projected vertical position, not
    // remain glued to a hard-coded global horizon. A literal 132 is
    // a valid origin and MUST NOT be interpreted as a sentinel.
    fb.clear(21U);
    count=EnhancedType7Compositor::draw(fb,source,columns,steps,50);
    assert(count==2U);
    assert(pixel(fb,114,32)==42U);
    assert(pixel(fb,115,287)==83U);
    assert(pixel(fb,132,32)==21U);
    fb.clear(24U);
    count=EnhancedType7Compositor::draw(fb,source,columns,steps,132);
    assert(count==2U);
    assert(pixel(fb,196,32)==42U);
    assert(pixel(fb,197,287)==83U);
    assert(pixel(fb,132,32)==24U);

    // The active mask, not a guessed color-key, owns transparency.
    source.mask[65U*2U+1U]=0U;
    fb.clear(9U);
    count=EnhancedType7Compositor::draw(fb,source,columns,steps);
    assert(count==1U);
    assert(pixel(fb,132,32)==42U);
    assert(pixel(fb,133,287)==9U);

    // Two output pixels per source row. The horizon at source row 64
    // remains at screen y132, independent of sprite scale.
    source.mask[65U*2U+1U]=0xFFU;
    steps.fill(2U);
    fb.clear(3U);
    count=EnhancedType7Compositor::draw(fb,source,columns,steps);
    assert(count==4U);
    assert(pixel(fb,132,32)==42U && pixel(fb,133,32)==42U);
    assert(pixel(fb,134,287)==83U && pixel(fb,135,287)==83U);

    // A fully masked pixel can contain palette index zero; source
    // transparency is never inferred from an arbitrary palette index.
    source.pixels[64U*2U+0U]=0U;
    fb.clear(12U);
    count=EnhancedType7Compositor::draw(fb,source,columns,steps);
    assert(count==4U);
    assert(pixel(fb,132,32)==0U);

    // Tiny distant sprite: zero-sized vertical SINTAB rows are legal
    // and must not cause writes or a divide-by-zero.
    steps.fill(0U);
    steps[64U]=1U;
    fb.clear(19U);
    count=EnhancedType7Compositor::draw(fb,source,columns,steps);
    assert(count==1U);
    assert(pixel(fb,132,32)==0U);
    assert(pixel(fb,132,287)==19U);

    // Ordered scenes composite back-to-front in the *existing*
    // source-authored draw-call order, not a new depth sort.
    auto overlay=sprite();
    overlay.mask.assign(overlay.mask.size(),0U);
    overlay.mask[64U*2U]=0xFFU;
    overlay.pixels[64U*2U]=172U;
    fb.clear(4U);
    count=EnhancedType7Compositor::draw(fb,source,columns,steps);
    assert(count==1U);
    count=EnhancedType7Compositor::draw(fb,overlay,columns,steps);
    assert(count==1U && pixel(fb,132,32)==172U);

    // Planned columns do not guarantee any visible opaque pixels: a
    // transparent RES3 source or a vertically offscreen sprite writes zero.
    // The caller must not label either draw as a visible rasterization.
    steps.fill(1U);
    auto all_transparent=sprite();
    all_transparent.mask.assign(all_transparent.mask.size(),0U);
    fb.clear(31U);
    count=EnhancedType7Compositor::draw(
        fb,all_transparent,columns,steps);
    assert(count==0U);
    assert(pixel(fb,132,32)==31U);
    fb.clear(32U);
    count=EnhancedType7Compositor::draw(
        fb,source,columns,steps,-256);
    assert(count==0U);
    assert(pixel(fb,132,32)==32U);

    // Reject invalid source/pixel/column/scale BEFORE any write.
    fb.clear(30U);
    auto invalid=sprite();
    invalid.mask.pop_back();
    try {
        (void)EnhancedType7Compositor::draw(fb,invalid,columns,steps);
        assert(false);
    } catch(const std::invalid_argument&) {}
    assert(pixel(fb,132,32)==30U);
    try {
        (void)EnhancedType7Compositor::draw(fb,source,
            {{3U,0U,0U}},steps);
        assert(false);
    } catch(const std::out_of_range&) {}
    assert(pixel(fb,132,32)==30U);
    try {
        (void)EnhancedType7Compositor::draw(fb,source,
            {{0U,0U,256U}},steps);
        assert(false);
    } catch(const std::out_of_range&) {}
    assert(pixel(fb,132,32)==30U);
    steps[80U]=5000U;
    try {
        (void)EnhancedType7Compositor::draw(fb,source,columns,steps);
        assert(false);
    } catch(const std::out_of_range&) {}
    assert(pixel(fb,132,32)==30U);

    // Clip tall/near sprites to the source viewport, not the 400-line HUD.
    steps.fill(4U);
    fb.clear(77U);
    source=sprite();
    source.mask.assign(source.mask.size(),0xFFU);
    source.pixels.assign(source.pixels.size(),11U);
    count=EnhancedType7Compositor::draw(fb,source,columns,steps);
    assert(count!=0U);
    assert(pixel(fb,4,32)==11U);
    assert(pixel(fb,259,287)==11U);
    assert(pixel(fb,3,32)==77U);
    assert(pixel(fb,260,287)==77U);
    assert(pixel(fb,133,0)==77U);
    return 0;
}
