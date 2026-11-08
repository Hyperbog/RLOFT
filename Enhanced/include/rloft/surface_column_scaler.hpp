#pragma once

#include <cstdint>
#include <vector>

#include "rloft/surface_texture.hpp"

namespace rloft {

struct SurfaceScaleState final {
    // Mirrors CODE.2 338C8's persistent/uninitialized [EBP-8] residue.
    // Normal shipped extents <= 256 do not rewrite it; callers may seed zero
    // for deterministic first-call behavior.
    std::uint32_t phase_residue{};
};

struct ScaledSurfaceColumn final {
    int start_row{};
    std::vector<std::uint8_t> pixels;
};

class SurfaceColumnScaler final {
public:
    // Portable CODE.2 338C8. Source is one 128-texel RES4 column.
    [[nodiscard]] static ScaledSurfaceColumn scale(
        const SurfaceTexture& texture,
        int source_column,
        std::uint16_t subtract_pair,
        std::uint32_t output_height,
        SurfaceScaleState& state);
};

}  // namespace rloft
