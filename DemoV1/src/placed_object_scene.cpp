#include "rloft/placed_object_scene.hpp"

#include "rloft/first_person_entity_projection.hpp"
#include "rloft/scene_shape_selector.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {

std::vector<WorldSceneRecord>
PlacedObjectSceneModel::build(
    const LevelWorldState& world,
    const ExplorationSessionState& session,
    const PlacedSceneCatalog& catalog,
    const RendererTables& tables,
    const SceneSourceBanks& banks,
    std::uint8_t render_threshold) {
    std::vector<WorldSceneRecord> out;
    out.reserve(world.placed_objects.size());

    for (std::size_t object_index = 0;
         object_index < world.placed_objects.size();
         ++object_index) {
        const auto& object =
            world.placed_objects[object_index];

        const auto cell_index =
            static_cast<std::size_t>(
                object.cached_cell_index);
        if (cell_index >=
            session.level.cell_words.size()) {
            throw std::out_of_range(
                "placed-object cached cell leaves level");
        }
        if ((session.level.cell_words[cell_index] &
             0x0040U) == 0U) {
            continue;
        }

        const auto& descriptor =
            catalog.at(object.object_type_id);
        if (descriptor.shape_stride == 0U) {
            throw std::logic_error(
                "visible placed object has zero scene shape stride");
        }

        const auto live_variant =
            static_cast<std::uint16_t>(
                (object.packed_runtime_state >> 6U) &
                7U);
        auto local_resource =
            static_cast<std::uint16_t>(
                descriptor.local_shape_base +
                static_cast<std::uint16_t>(
                    live_variant *
                    descriptor.shape_stride));

        const auto projection =
            FirstPersonEntityProjectionModel::project(
                object.y_position_16_16,
                object.x_position_16_16,
                session,
                tables);
        if (!projection.visible ||
            projection.projected_extent == 0U ||
            projection.projected_extent <=
                render_threshold) {
            continue;
        }

        local_resource =
            SceneShapeSelectorModel::select_bank_local(
                local_resource,
                projection.screen_x,
                object.packed_runtime_state,
                object.object_type_id,
                1U,
                descriptor.shape_stride,
                session,
                tables);

        WorldSceneRecord record{};
        record.screen_x =
            projection.screen_x;
        record.projected_extent =
            projection.projected_extent;
        record.resource_id =
            static_cast<std::uint16_t>(
                banks.at(1U) +
                local_resource);
        record.source_index =
            static_cast<std::uint16_t>(
                object_index);
        record.source_class = 1U;
        record.source_type_id = object.object_type_id;
        record.render_scale_lookup_bias = 0U;

        // 2BBA0 only precomputes 21BF0 rectangles for four class-1 type IDs
        // (0x52, 0x6E, 0x71, 0x91). Level 22 uses only 62..64, so its class-1
        // records correctly retain zero emitter-time bounds.
        out.push_back(record);
    }

    return out;
}

}  // namespace rloft
