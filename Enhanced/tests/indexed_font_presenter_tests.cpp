#include "rloft/indexed_font_presenter.hpp"

#include <cassert>
#include <cstddef>

int main() {
    rloft::Type2Font font{};
    font.widths.fill(1);
    font.widths[static_cast<std::size_t>('A')] = 3;
    font.widths[static_cast<std::size_t>('B')] = 2;
    font.raster.count = 128;
    font.raster.width = 3;
    font.raster.height = 1;
    font.raster.transparent_index = 0;
    font.raster.pixels.assign(
        128U * 3U, 0);
    font.raster.pixels[
        static_cast<std::size_t>('A') * 3U] = 0x41;
    font.raster.pixels[
        static_cast<std::size_t>('B') * 3U] = 0x42;

    assert(
        rloft::IndexedFontPresenter::measure(
            font, "AB") == 5);

    rloft::IndexedFramebuffer framebuffer;
    framebuffer.clear(0);
    rloft::IndexedFontPresenter::draw_centered(
        framebuffer,
        font,
        10,
        20,
        20,
        "AB");

    const auto& pixels = framebuffer.pixels();
    auto at = [&](int row, int column) {
        return pixels[
            static_cast<std::size_t>(row) * 320U +
            static_cast<std::size_t>(column)];
    };

    // (20 - 5) / 2 = 7: A begins at 27, B at 30.
    assert(at(10, 27) == 0x41);
    assert(at(10, 30) == 0x42);
}
