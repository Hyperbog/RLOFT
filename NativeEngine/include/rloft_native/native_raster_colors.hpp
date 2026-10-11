#pragma once
#include "rloft_native/native_assets.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft_native {

// Indexed game artwork is NOT self-paletted. The caller must explicitly
// choose its original COLORS bank, and must request CODE.2's menu cursor
// translation only for the original INIT-selected menu cursor.
enum class NativeRasterColorMode {
    RawIndices,
    Code2TitleCursor
};

struct NativeBgraRaster {
    std::uint16_t width{};
    std::uint16_t height{};
    std::vector<std::uint8_t> bgra; // Windows BI_RGB, bottom byte B
};

// The original CODE.2 title menu patches nineteen cursor indexes and
// translates only frame 0. Transparent pixels are not translated.
[[nodiscard]] NativeType1Raster remap_native_code2_title_cursor(
    const NativeType1Raster& source);

// Convert original indexed pixels + explicit original DAC6 palette bank to
// Windows BGRA. No implicit palette selection, color inversion or R/B swap.
[[nodiscard]] NativeBgraRaster colorize_native_raster(
    const NativeType1Raster& source,
    const NativePalette6& palette,
    std::size_t frame,
    NativeRasterColorMode mode = NativeRasterColorMode::RawIndices);

} // namespace rloft_native
