#include "rloft/environment_presenter.hpp"

#include <cassert>
#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

int main() {
    rloft::IndexedFramebuffer framebuffer;
    framebuffer.clear(0x55);

    rloft::Type1Raster raster{
        1,
        4,
        3,
        0,
        {
            0, 1, 7, 255,
            8, 9, 10, 11,
            12, 13, 14, 15,
        },
    };

    rloft::EnvironmentPresenter::draw_subtracted(
        framebuffer, raster, 0, 4, 32, 8, true);

    const auto& px = framebuffer.pixels();
    auto at = [&](int row, int col) {
        return px[static_cast<std::size_t>(row) * 320U +
                  static_cast<std::size_t>(col)];
    };

    // Transparent source zero is untouched.
    assert(at(4, 32) == 0x55);
    // 8-bit subtraction wraps exactly as CODE.2 SUB AL,imm8.
    assert(at(4, 33) == static_cast<std::uint8_t>(1 - 8));
    assert(at(4, 34) == static_cast<std::uint8_t>(7 - 8));
    assert(at(4, 35) == 247);
    assert(at(5, 32) == 0);
    assert(at(6, 35) == 7);

    // The enhanced PC scene opts into clamped palette subtraction.
    // Small/dark source indices must not wrap into unrelated high
    // palette ramps; original source-mode wrap above stays unchanged.
    framebuffer.clear(0x55);
    rloft::EnvironmentPresenter::draw_subtracted(
        framebuffer, raster, 0, 4, 32, 8, true, true);
    assert(at(4, 32) == 0x55);  // transparency unchanged
    assert(at(4, 33) == 0);
    assert(at(4, 34) == 0);
    assert(at(4, 35) == 247);  // valid nonunderflow shade
    assert(at(5, 32) == 0);
    assert(at(5, 33) == 1);
    assert(at(6, 35) == 7);

    // Environment commands carry an authored strip width. Before this
    // boundary the native renderer projected eight-pixel RES8 strips
    // but drew their entire decoded raster, causing sideways bleed.
    framebuffer.clear(0x44);
    rloft::EnvironmentPresenter::draw_subtracted(
        framebuffer,raster,0,4,32,0,true,false,2);
    assert(at(4,32)==0x44); // source transparency still takes precedence
    assert(at(4,33)==1);
    assert(at(4,34)==0x44); // never draw outside authored width
    assert(at(5,32)==8 && at(5,33)==9);
    assert(at(5,34)==0x44);
    // If the authored record specifies more than the decoded source
    // stride, clipping must never read past the raster source.
    framebuffer.clear(0x44);
    rloft::EnvironmentPresenter::draw_subtracted(
        framebuffer,raster,0,4,32,0,true,false,16);
    assert(at(4,35)==255);
    assert(at(4,36)==0x44);
    const auto untouched=framebuffer.pixels();
    bool invalid_width=false;
    try {
        rloft::EnvironmentPresenter::draw_subtracted(
            framebuffer,raster,0,4,32,0,true,false,0);
    } catch(const std::invalid_argument&) { invalid_width=true; }
    assert(invalid_width && framebuffer.pixels()==untouched);
    // Untrusted authored coordinates cannot overflow int arithmetic and
    // wrap a backdrop strip back into the visible game field.
    rloft::EnvironmentPresenter::draw_subtracted(
        framebuffer,raster,0,std::numeric_limits<int>::max(),
        std::numeric_limits<int>::max(),0,false,false,2);
    rloft::EnvironmentPresenter::draw_subtracted(
        framebuffer,raster,0,std::numeric_limits<int>::min(),
        std::numeric_limits<int>::min(),0,false,false,2);
    assert(framebuffer.pixels()==untouched);

    // Profile placements clip to rows 4..131 and columns 32..287.
    framebuffer.clear(0x44);
    rloft::EnvironmentPresenter::draw_subtracted(
        framebuffer, raster, 0, 3, 31, 0, true);
    assert(at(3, 31) == 0x44);
    assert(at(4, 32) == 9);
    assert(at(5, 34) == 15);

    // Random-backdrop style draws use the ordinary framebuffer bounds rather
    // than the 29564 environment clip wrapper.
    framebuffer.clear(0x33);
    rloft::EnvironmentPresenter::draw_subtracted(
        framebuffer, raster, 0, 3, 31, 0, false);
    assert(at(3, 32) == 1);
    assert(at(4, 31) == 8);
}
