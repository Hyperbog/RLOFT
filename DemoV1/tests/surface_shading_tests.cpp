#include "rloft/surface_shading.hpp"

#include <cassert>

int main() {
    const auto daylight =
        rloft::SurfaceShadingModel::build(0, 0);

    assert(daylight.packed[0] == 0x0101);
    assert(daylight.packed[64] == 0x0101);
    assert(daylight.packed[100] == 0x0000);
    assert(daylight.packed[175] == 0x0000);
    assert(daylight.fallback == 0x0000);

    const auto dusk =
        rloft::SurfaceShadingModel::build(8, 0);
    assert(dusk.packed[0] == 0x0909);
    assert(dusk.packed[9] == 0x0809);
    assert(dusk.fallback == 0x0000);

    // Normal resources use extent-indexed table entries, packed subtraction,
    // signed-byte clamp, then odd-ray byte swap.
    rloft::SurfaceShadeTable synthetic{};
    synthetic.packed.fill(0x0305);
    synthetic.fallback = 0x0708;

    auto shade =
        rloft::SurfaceShadingModel::packed_subtract(
            synthetic, 100, 10, 1, 2);
    assert(shade == 0x0204);

    shade =
        rloft::SurfaceShadingModel::packed_subtract(
            synthetic, 100, 10, 1, 3);
    assert(shade == 0x0402);

    // Packed 16-bit subtraction preserves the original borrow behavior.
    synthetic.packed[10] = 0x0101;
    shade =
        rloft::SurfaceShadingModel::packed_subtract(
            synthetic, 100, 10, 2, 0);
    assert(shade == 0x0000);

    // Special resource range 0x242..0x24D bypasses fallback/decrement/swap.
    synthetic.packed[0] = 0x0304;
    shade =
        rloft::SurfaceShadingModel::packed_subtract(
            synthetic, 0x242, 10, 3, 1);
    assert(shade == 0x0304);

    // Final executable cap.
    synthetic.packed[0] = 0x0909;
    shade =
        rloft::SurfaceShadingModel::packed_subtract(
            synthetic, 0x242, 0, 0, 0);
    assert(shade == 0x0808);
}
