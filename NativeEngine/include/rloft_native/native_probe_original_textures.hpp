#pragma once
#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_code2_backdrop.hpp"
#include "rloft_native/native_scene_view.hpp"
#include <cstdint>
#include <map>
#include <vector>

namespace rloft_native {

// An INTERACTIVE HOST PREVIEW using AUTHENTIC original RES4 EH6 textures and
// CODE.2 source-level COLORS palette. Camera/ray traversal, wall side and
// projected height remain deliberately PROVISIONAL and not DOS verified.
struct NativeProbeTexturedFrame {
    unsigned width{};
    unsigned height{};
    std::vector<std::uint8_t> bgra;
    unsigned wall_columns{};
    unsigned preview_upper_wall_columns{};
    unsigned original_texel_samples{};
    unsigned preview_floor_texels{};
    unsigned preview_ceiling_texels{};

    unsigned source_texture_decompressions{};
    bool original_code2_sampler_used{false};
    bool source_quadrant_state_used{false};
    bool original_code2_backdrop_used{false};
    bool original_environment_surface_source_verified{false};
    bool original_runtime_camera_verified{false};
    bool original_dos_frame_verified{false};
};

class NativeProbeOriginalTextures {
public:
    NativeProbeOriginalTextures(const NativeMedia& media,
                                unsigned original_wide_count=600U);
    [[nodiscard]] NativeProbeTexturedFrame render(
        const NativeWorldRuntime& world,const NativeProbeView& view,
        unsigned width,unsigned height);
    [[nodiscard]] std::size_t cache_size() const noexcept {
        return textures_.size();
    }
private:
    NativeResArchive source_;
    NativeCode2Backdrop backdrop_;
    std::vector<std::uint8_t> original_colors_;
    unsigned wide_count_;
    std::map<unsigned,NativeType1Raster> textures_;
    std::map<unsigned,NativePalette6> palettes_;
};

} // namespace rloft_native
