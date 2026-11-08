#include "rloft/surface_column_scaler.hpp"

#include <cassert>
#include <cstdint>

int main() {
    rloft::SurfaceTexture texture{};
    texture.width = 2;
    texture.texels_column_major.resize(256);
    for (int y = 0; y < 128; ++y) {
        texture.texels_column_major[
            static_cast<std::size_t>(y)] =
            static_cast<std::uint8_t>(y);
        texture.texels_column_major[
            128U + static_cast<std::size_t>(y)] =
            static_cast<std::uint8_t>(255 - y);
    }

    rloft::SurfaceScaleState state{};

    // 128 output rows: step = 1.0 source texel, start row 80.
    auto scaled = rloft::SurfaceColumnScaler::scale(
        texture, 0, 0x0201U, 128U, state);
    assert(scaled.start_row == 80);
    assert(scaled.pixels.size() == 128);
    assert(scaled.pixels[0] == 255);  // 0 - 1 wraps
    assert(scaled.pixels[1] == 255);  // 1 - 2 wraps
    assert(scaled.pixels[2] == 1);    // 2 - 1
    assert(scaled.pixels[127] == 125);// 127 - 2

    // 64 output rows: source advances by two texels, start row 104.
    state.phase_residue = 0;
    scaled = rloft::SurfaceColumnScaler::scale(
        texture, 0, 0, 64U, state);
    assert(scaled.start_row == 104);
    assert(scaled.pixels.size() == 64);
    assert(scaled.pixels[0] == 0);
    assert(scaled.pixels[1] == 2);
    assert(scaled.pixels[63] == 126);

    // 256 requests are clipped at the 256-line working region: row 32..255.
    state.phase_residue = 0;
    scaled = rloft::SurfaceColumnScaler::scale(
        texture, 1, 0, 256U, state);
    assert(scaled.start_row == 32);
    assert(scaled.pixels.size() == 224);
    assert(scaled.pixels.front() == 255);
    assert(scaled.pixels.back() == 144);

    bool rejected = false;
    try {
        (void)rloft::SurfaceColumnScaler::scale(
            texture, 2, 0, 64U, state);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected);
}
