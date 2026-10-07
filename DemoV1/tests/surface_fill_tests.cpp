#include "rloft/surface_fill.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
    constexpr std::size_t start =
        0x75F24U + 0xFE94U;
    std::vector<std::uint8_t> code2(
        start + 129U, 0);
    for (std::size_t i = 0; i < 129U; ++i) {
        code2[start + i] =
            static_cast<std::uint8_t>(i);
    }

    const auto table =
        rloft::SurfaceFillTable::from_code2(
            code2);
    assert(table.subtract[1] == 1);
    assert(table.subtract[128] == 128);

    rloft::IndexedFramebuffer fb;
    fb.clear(0xEE);

    rloft::SurfaceFillModel::draw_upper(
        fb, table, 200);
    const auto& px = fb.pixels();
    auto at = [&](int row, int col) {
        return px[
            static_cast<std::size_t>(row) * 320U +
            static_cast<std::size_t>(col)];
    };

    assert(at(4, 32) ==
           static_cast<std::uint8_t>(200 - 128));
    assert(at(131, 287) ==
           static_cast<std::uint8_t>(200 - 1));
    assert(at(3, 32) == 0xEE);
    assert(at(4, 31) == 0xEE);
    assert(at(4, 288) == 0xEE);
    assert(at(132, 32) == 0xEE);

    rloft::SurfaceFillModel::draw_lower(
        fb, table, 10);
    assert(at(259, 32) ==
           static_cast<std::uint8_t>(10 - 128));
    assert(at(132, 287) == 9);\n    assert(at(260, 32) == 0xEE);\n    // Row 131 belongs to the already-drawn upper half; lower stops at 132.\n    assert(at(131, 32) ==\n           static_cast<std::uint8_t>(200 - 1));

    bool rejected = false;
    try {
        std::vector<std::uint8_t> short_code2(16);
        (void)rloft::SurfaceFillTable::from_code2(
            short_code2);
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    assert(rejected);
}
