#pragma once

#include <cstdint>

#include "rloft/resource_assets.hpp"

namespace rloft {

struct SceneInteractionBounds final {
    bool valid{};
    std::uint16_t top{};
    std::uint16_t bottom{};
    std::uint16_t left{};
    std::uint16_t right{};

    [[nodiscard]] bool contains(
        int row,
        int column) const noexcept {
        return valid &&
               row >= static_cast<int>(top) &&
               row <= static_cast<int>(bottom) &&
               column >= static_cast<int>(left) &&
               column <= static_cast<int>(right);
    }
};

class SceneShapeBoundsModel final {
public:
    // CODE.2 21BF0 plus the NPC caller's +32 adjustment to the two
    // horizontal bounds at 2BB73..2BB82.
    [[nodiscard]] static SceneInteractionBounds emitter_type7(
        std::uint16_t screen_x,
        std::uint16_t projected_extent,
        const Type7Geometry& geometry);

    [[nodiscard]] static SceneInteractionBounds interaction_type7(
        std::uint16_t screen_x,
        std::uint16_t projected_extent,
        const Type7Geometry& geometry);

    // Compatibility name retained for existing NPC-emitter callers/tests.
    [[nodiscard]] static SceneInteractionBounds npc_type7(
        std::uint16_t screen_x,
        std::uint16_t projected_extent,
        const Type7Geometry& geometry) {
        return emitter_type7(
            screen_x,
            projected_extent,
            geometry);
    }
};

}  // namespace rloft
