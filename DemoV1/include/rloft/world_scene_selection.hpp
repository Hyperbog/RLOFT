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
    using Class1Eligibility =
        std::function<bool(std::uint16_t source_index)>;

    // CODE.2 2AC80: only these class-1 placed-object type IDs are eligible
    // for the 223F4 viewport interaction path.
    [[nodiscard]] static bool class1_type_is_interactive(
        std::uint16_t object_type_id) noexcept;

    // CODE.2 223F4 selection order. Highest render_order is tested first.
    // Class 3 is skipped. Class 1 is prefiltered through 2AC80 before its
    // rectangle is tested. Class 2 requires action_flags bit 0, matching the
    // pickup gate at 224EF..224F4. Class 0 remains targetable independently.
    [[nodiscard]] static std::optional<WorldSceneTarget> target_at(
        const std::vector<WorldSceneRecord>& records,
        int row,
        int column,
        std::uint8_t action_flags,
        const GeometryLookup& geometry,
        const Class1Eligibility& class1_eligible = {});
};

}  // namespace rloft
