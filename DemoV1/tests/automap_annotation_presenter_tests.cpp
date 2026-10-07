#include "rloft/automap_annotation_presenter.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace {

rloft::Type2Font font() {
    rloft::Type2Font f{};
    f.widths.fill(1);
    f.widths[static_cast<std::size_t>('A')] = 3;
    f.widths[static_cast<std::size_t>('B')] = 2;
    f.widths[0x0D] = 4;

    f.raster.count = 128;
    f.raster.width = 8;
    f.raster.height = 2;
    f.raster.transparent_index = 0;
    f.raster.pixels.assign(128U * 16U, 0);

    auto set = [&](unsigned ch, int y, int x, std::uint8_t value) {
        f.raster.pixels[
            static_cast<std::size_t>(ch) * 16U +
            static_cast<std::size_t>(y) * 8U +
            static_cast<std::size_t>(x)] = value;
    };
    set('A', 0, 0, 0xA1);
    set('A', 1, 1, 0xA2);
    set('B', 0, 0, 0xB1);
    return f;
}

}  // namespace

int main() {
    const auto f = font();

    rloft::AutomapAnnotation a{};
    a.raw_x = 16;
    a.raw_y = 10;
    a.payload = {'A', 0x0D, 'B'};

    assert(
        rloft::AutomapAnnotationPresenter::bounded_extent(
            a, f) == 9);
    assert(
        rloft::AutomapAnnotationPresenter::visible(
            a, f, 0, 0));
    assert(
        !rloft::AutomapAnnotationPresenter::visible(
            a, f, 100, 100));

    rloft::IndexedFramebuffer fb;
    fb.clear(0x55);
    rloft::AutomapAnnotationPresenter::draw_with_font(
        fb, {a}, 0, 0, f);

    const auto& px = fb.pixels();
    auto at = [&](int row, int column) {
        return px[
            static_cast<std::size_t>(row) * 320U +
            static_cast<std::size_t>(column)];
    };

    assert(at(16, 10) == 0xA1);
    assert(at(17, 11) == 0xA2);
    assert(at(28, 10) == 0xB1);

    rloft::AutomapAnnotation b{};
    b.raw_x = 20;
    b.raw_y = 20;
    b.payload = {'A', 'B'};
    fb.clear(0);
    rloft::AutomapAnnotationPresenter::draw_with_font(
        fb, {b}, 0, 0, f);
    assert(at(20, 20) == 0xA1);
    assert(at(20, 23) == 0xB1);

    rloft::AutomapAnnotation clipped{};
    clipped.raw_x = 5;
    clipped.raw_y = 3;
    clipped.payload = {'A'};
    fb.clear(0x22);
    rloft::AutomapAnnotationPresenter::draw_with_font(
        fb, {clipped}, 0, 0, f);
    assert(at(5, 3) == 0x22);
    assert(at(6, 4) == 0xA2);
}
