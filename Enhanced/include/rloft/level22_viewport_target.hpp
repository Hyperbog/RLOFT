#pragma once

#include <cstddef>
#include <optional>

#include "rloft/level_world_state.hpp"
#include "rloft/world_scene_selection.hpp"

namespace rloft {

// The original 223F4 scene hit test returns a source class and its live
// record index, NOT a pointer to an invented 2D collision target.
// Validate that identity against the mutable Level 22 world before
// dispatching a combat or pickup action.
class Level22ViewportTargetModel final {
public:
    [[nodiscard]] static std::optional<std::size_t> assassin_index(
        const std::optional<WorldSceneTarget>& target,
        const LevelWorldState& world);

    [[nodiscard]] static std::optional<std::size_t> loose_item_index(
        const std::optional<WorldSceneTarget>& target,
        const LevelWorldState& world);
};

} // namespace rloft
