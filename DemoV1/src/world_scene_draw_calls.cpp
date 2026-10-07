#include "rloft/world_scene_draw_calls.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace rloft {

std::vector<WorldSceneDrawCall>
WorldSceneDrawCallModel::build(
    const std::vector<WorldSceneRecord>& records,
    const LevelWorldState& world) {
    std::vector<WorldSceneDrawCall> out;
    out.reserve(records.size() * 2U);

    std::vector<std::size_t> indices(records.size());
    std::iota(indices.begin(), indices.end(), 0U);
    std::stable_sort(
        indices.begin(),
        indices.end(),
        [&records](std::size_t a, std::size_t b) {
            return records[a].render_order <
                   records[b].render_order;
        });

    for (const auto index : indices) {
        const auto& record = records[index];
        const auto append =
            [&out, &record](
                std::uint16_t resource_id,
                std::uint32_t scale_extent,
                std::uint32_t render_mode) {
                out.push_back(
                    WorldSceneDrawCall{
                        record.source_class,
                        record.source_index,
                        record.screen_x,
                        resource_id,
                        scale_extent,
                        record.render_scale_lookup_bias,
                        render_mode,
                        record.projected_extent,
                    });
            };

        switch (record.source_class) {
            case 0U: {
                const auto& npc =
                    world.npcs.at(record.source_index);
                if (npc.npc_type_id == 8U) {
                    // 2C4A3..2C52C draws NPC type 8 twice. Both calls
                    // use round((extent * 120) / 216) as integer arithmetic;
                    // the first offsets the selected RES3 shape by 18.
                    const auto scale =
                        (static_cast<std::uint32_t>(
                             record.projected_extent) *
                             0x78U +
                         0x6CU) /
                        0xD8U;
                    append(
                        static_cast<std::uint16_t>(
                            record.resource_id + 0x12U),
                        scale,
                        0x58U);
                    append(
                        record.resource_id,
                        scale,
                        0U);
                } else {
                    append(
                        record.resource_id,
                        record.projected_extent,
                        0U);
                }

                // 2C677..2C6B6 can append a third/effect draw when live
                // NPC flags byte +0x08 has bit 3 set. That path references
                // runtime global 27106 and is not active for Level 22's
                // canonical Assassin; never silently omit it if requested.
                if ((npc.runtime_flags & 0x00000008U) != 0U) {
                    throw std::logic_error(
                        "NPC auxiliary type-7 draw requires 27106 effect state");
                }
                break;
            }
            case 1U: {
                const auto& placed =
                    world.placed_objects.at(
                        record.source_index);
                if (placed.object_type_id >= 0x3CU &&
                    placed.object_type_id <= 0x43U) {
                    // 2C541..2C5B3: a leading special pass uses ID+8
                    // and mode 0x58. The ordinary pass always follows.
                    append(
                        static_cast<std::uint16_t>(
                            record.resource_id + 8U),
                        record.projected_extent,
                        0x58U);
                }
                append(
                    record.resource_id,
                    record.projected_extent,
                    0U);
                break;
            }
            case 2U: {
                const auto& item =
                    world.loose_items.at(
                        record.source_index);
                // 2C5C3 tests live family-C record byte +0x0D bit 0x02.
                // That byte is the high byte of cached_cell_index in the
                // reconstructed 0x16-byte family-C record.
                append(
                    record.resource_id,
                    record.projected_extent,
                    (item.cached_cell_index & 0x0200U) != 0U
                        ? 0x3CU
                        : 0U);
                break;
            }
            case 3U:
                append(
                    record.resource_id,
                    record.projected_extent,
                    0U);
                break;
            default:
                throw std::logic_error(
                    "unrecognized generated scene source class");
        }
    }

    return out;
}

}  // namespace rloft
