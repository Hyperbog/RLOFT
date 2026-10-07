#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace rloft {

struct SurfaceShadeTable final {
    static constexpr std::size_t kCount = 176;

    std::array<std::uint16_t, kCount> packed{};
    std::uint16_t fallback{};
};

class SurfaceShadingModel final {
public:
    static constexpr int kDepthLimit = 175;
    static constexpr int kSpan = 127;

    // CODE.2 31A14, using the executable's F954 table.
    [[nodiscard]] static SurfaceShadeTable build(
        int environment_shade,
        int fade);

    // CODE.2 3263C shade selection for one coalesced run.
    [[nodiscard]] static std::uint16_t packed_subtract(
        const SurfaceShadeTable& table,
        std::uint32_t resource_id,
        std::uint32_t extent,
        std::uint32_t shade_decrement,
        std::size_t first_ray) noexcept;
};

}  // namespace rloft
