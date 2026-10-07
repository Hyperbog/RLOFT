#include "rloft/environment_presenter.hpp"

#include <cassert>
#include <cstdint>
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
