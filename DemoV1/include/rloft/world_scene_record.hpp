#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// Portable representation of the stable fields in CODE.2's generated
// 0x16-byte first-person scene record.
struct WorldSceneRecord final {
    std::uint16_t screen_x{};
    std::uint16_t projected_extent{};
    std::uint16_t resource_id{};
    std::uint16_t source_index{};
    std::uint16_t source_class{};
    std::uint16_t render_order{};
    std::uint16_t render_scale_lookup_bias{};

    // Emitter-time 21BF0 bounds. Interaction selection deliberately
    // recomputes its own bounds with the original lower threshold.
    std::uint16_t top{};
    std::uint16_t bottom{};
    std::uint16_t left{};
    std::uint16_t right{};
};

class WorldSceneOrderModel final {
public:
    // CODE.2 2C294 ordering pass. Rank 0 is the smallest projected extent;
    // higher ranks are nearer/larger and are tested first by 223F4.
    // Equal extents retain source emission order because the original search
    // only replaces its candidate for a strictly smaller extent.
    static void assign_render_order(
        std::vector<WorldSceneRecord>& records) noexcept;
};

}  // namespace rloft
