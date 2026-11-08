#pragma once

#include <cstdint>

#include "rloft/exploration_session.hpp"
#include "rloft/renderer_tables.hpp"

namespace rloft {

struct FirstPersonEntityProjection final {
    bool visible{};
    std::uint32_t camera_depth_16_16{};
    std::uint32_t camera_lateral_16_16{};
    std::uint16_t screen_x{};
    std::uint16_t projected_extent{};
};

class FirstPersonEntityProjectionModel final {
public:
    // CODE.2 35798. Projects one world-space 16.16 point through the same
    // camera basis prepared by 334C4 for first-person wall traversal.
    [[nodiscard]] static FirstPersonEntityProjection project(
        std::uint32_t world_y_16_16,
        std::uint32_t world_x_16_16,
        const ExplorationSessionState& session,
        const RendererTables& tables);
};

}  // namespace rloft
