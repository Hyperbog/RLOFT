#pragma once
#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_sintab_raster.hpp"
#include <array>
#include <cstdint>
#include <vector>

namespace rloft_native {
// CODE.2 33E28 / 33DE0: actual first-person two-band 256x256
// environment fill. This is original palette-index artwork generated
// from per-level header bytes + FE94 lookup, NOT a textured skybox.
class NativeCode2Backdrop {
public:
    explicit NativeCode2Backdrop(const std::vector<std::uint8_t>& original_code2);
    // Source placement is x=32..287, y=4..259 (page 0).
    // screen_pitch_pixels is an OPTIONAL host mouse-look extension;
    // pitch=0 alone exactly reproduces the original 128+128 fills.
    void fill(const NativeLevelPayload& level,
              NativeIndexedVgaPlanes& planes,int screen_pitch_pixels=0) const;
    [[nodiscard]] std::uint8_t source_index(const NativeLevelPayload& level,
                            unsigned viewport_row,int pitch_pixels=0) const;
private:
    std::array<std::uint8_t,129> fe94_{};
};
} // namespace rloft_native
