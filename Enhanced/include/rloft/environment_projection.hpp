#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "rloft/resource_assets.hpp"

namespace rloft {

enum class EnvironmentDrawKind {
    RandomBackdropStrip,
    ProfilePlacement,
};

struct EnvironmentDrawCommand final {
    EnvironmentDrawKind kind{EnvironmentDrawKind::ProfilePlacement};
    int resource_id{};
    int x{};
    int y{};
    int width{};
    int shade{};
};

class EnvironmentProjectionModel final {
public:
    // CODE.2 314B0 draw-command reconstruction. rng15 supplies the 32
    // 15-bit random values consumed by the optional backdrop-strip prepass.
    [[nodiscard]] static std::vector<EnvironmentDrawCommand> build(
        const EnvironmentProfile& profile,
        int camera_degrees,
        int mode,
        const std::array<std::uint16_t, 32>& rng15 = {});

    [[nodiscard]] static int projected_x(
        int world_azimuth_degrees,
        int camera_degrees,
        int width_pixels) noexcept;
};

}  // namespace rloft
