#pragma once
#include "rloft_native/native_assets.hpp"
#include <cstdint>
#include <vector>
namespace rloft_native {
// Independent EH6 decoder; original 32x128 / 64x128 first-person textures
// are 128-texel vertical columns, transposed for an inspection viewport.
[[nodiscard]] NativeType1Raster decode_native_type6_texture(
    const std::vector<std::uint8_t>& entry);
} // namespace rloft_native
