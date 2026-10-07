#pragma once

#include <cstdint>
#include <vector>

#include "rloft/level_world_state.hpp"
#include "rloft/world_scene_record.hpp"

namespace rloft {

// The six cdecl arguments passed to CODE.2 36CEC by 2C4A3..2C6B6,
// with source identity retained for diagnostics and later interaction.
struct WorldSceneDrawCall final {
    std::uint16_t source_class{};
    std::uint16_t source_index{};
    std::uint16_t screen_x{};
    std::uint16_t resource_id{};
    std::uint32_t scale_extent{};
    std::uint16_t render_scale_lookup_bias{};
    std::uint32_t render_mode{};
    std::uint16_t raw_depth{};
};

class WorldSceneDrawCallModel final {
public:
    // Ranks ascend in draw order, preserving 2C294's record ranks.
    // This is an invocation plan; it does not imply pixels were rendered.
    [[nodiscard]] static std::vector<WorldSceneDrawCall> build(
        const std::vector<WorldSceneRecord>& records,
        const LevelWorldState& world);
};

}  // namespace rloft
