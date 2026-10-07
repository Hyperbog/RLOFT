#include "rloft/loose_item_scene.hpp"

#include "rloft/first_person_entity_projection.hpp"
#include "rloft/scene_shape_selector.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {

std::vector<WorldSceneRecord>
LooseItemSceneModel::build(
    const LevelWorldState& world,
    const ExplorationSessionState& session,
    const ObjectCatalog& catalog,
    const RendererTables& tables,
    const SceneSourceBanks& banks,
    std::uint8_t render_threshold) {
    std::vector<WorldSceneRecord> out;
    out.reserve(world.loose_items.size());

    for (std::size_t item_index = 0;
         item_index < world.loose_items.size();
         ++item_index) {
        const auto& item =
            world.loose_items[item_index];

        const auto cell_index =
            static_cast<std::size_t>(
                item.cached_cell_index);
        if (cell_index >=
            session.level.cell_words.size()) {
            throw std::out_of_range(
                "loose-item cached cell leaves level");
        }
        if ((session.level.cell_words[cell_index] &
             0x0040U) == 0U) {
            continue;
        }

        const auto& definition =
            catalog.at(item.object_type_id);
        if (definition.scene_shape_stride == 0U) {
            // 2B2F4 divides by this field on the generic class-2 path.
            // Shipped records reaching this emitter must therefore carry a
            // nonzero stride; reject malformed/custom state deterministically.
            throw std::logic_error(
                "visible loose item has zero scene shape stride");
        }
        const auto projection =
            FirstPersonEntityProjectionModel::project(
                item.y_position_16_16,
                item.x_position_16_16,
                session,
                tables);
        if (!projection.visible ||
            projection.projected_extent == 0U ||
            projection.projected_extent <=
                render_threshold) {
            continue;
        }

        const auto live_selector =
            static_cast<std::uint16_t>(
                (item.packed_runtime_state >> 6U) &
                7U);
        auto local_resource =
            static_cast<std::uint16_t>(
                definition.scene_shape_base_local +
                static_cast<std::uint16_t>(
                    live_selector *
                    definition.scene_shape_stride));

        local_resource =
            SceneShapeSelectorModel::select_bank_local(
                local_resource,
                projection.screen_x,
                item.packed_runtime_state,
                item.object_type_id,
                2U,
                definition.scene_shape_stride,
                session,
                tables);

        WorldSceneRecord record{};
        record.screen_x =
            projection.screen_x;
        record.projected_extent =
            projection.projected_extent;
        record.resource_id =
            static_cast<std::uint16_t>(
                banks.at(2U) +
                local_resource);
        record.source_index =
            static_cast<std::uint16_t>(
                item_index);
        record.source_class = 2U;
        record.render_scale_lookup_bias = 0U;
        out.push_back(record);
    }

    return out;
}

}  // namespace rloft
