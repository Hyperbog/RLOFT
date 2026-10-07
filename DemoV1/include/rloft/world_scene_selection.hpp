#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

#include "rloft/resource_assets.hpp"
#include "rloft/world_scene_record.hpp"

namespace rloft {

struct WorldSceneTarget final {
    std::size_t record_index{};
    std::uint16_t source_class{};
    std::uint16_t source_index{};
};

class WorldSceneSelectionModel final {
public:
    using GeometryLookup =
        std::function<Type7Geometry(std::uint16_t absolute_res3_id)>;

    // CODE.2 223F4 selection order for the integrated source classes.
    // Highest render_order is tested first. Class 3 is non-interactive.
    // Class 1 has an additional 2AC80 eligibility predicate and is kept
    // outside this API until that predicate is promoted.
    [[nodiscard]] static std::optional<WorldSceneTarget> target_at(
        const std::vector<WorldSceneRecord>& records,
        int row,
        int column,
        const GeometryLookup& geometry);
};

}  // namespace rloft
