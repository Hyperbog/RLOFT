#include "rloft/world_scene_draw_calls.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

rloft::WorldSceneRecord record(
    std::uint16_t source_class,
    std::uint16_t source_index,
    std::uint16_t resource_id,
    std::uint16_t extent,
    std::uint16_t rank) {
    rloft::WorldSceneRecord out{};
    out.source_class = source_class;
    out.source_index = source_index;
    out.resource_id = resource_id;
    out.screen_x = 144U;
    out.projected_extent = extent;
    out.render_order = rank;
    out.render_scale_lookup_bias = 3U;
    return out;
}

}  // namespace

int main() {
    rloft::LevelWorldState world{};
    rloft::LiveNpcRecord assassin{};
    assassin.npc_type_id = 9U;
    world.npcs.push_back(assassin);
    rloft::LiveNpcRecord special_npc{};
    special_npc.npc_type_id = 8U;
    world.npcs.push_back(special_npc);

    rloft::LiveObjectRecord placed{};
    placed.object_type_id = 62U;
    world.placed_objects.push_back(placed);
    rloft::LiveObjectRecord relic{};
    relic.object_type_id = 0x88U;
    world.loose_items.push_back(relic);

    const auto calls =
        rloft::WorldSceneDrawCallModel::build(
            {
                record(1U, 0U, 2276U, 420U, 2U),
                record(2U, 0U, 4010U, 320U, 0U),
                record(0U, 0U, 195U, 512U, 3U),
                record(0U, 1U, 236U, 400U, 1U),
            },
            world);

    assert(calls.size() == 6U);

    // Lower render rank draws first. One ordinary class-2 relic call.
    assert(calls[0].source_class == 2U);
    assert(calls[0].resource_id == 4010U);
    assert(calls[0].scale_extent == 320U);
    assert(calls[0].render_mode == 0U);
    assert(calls[0].raw_depth == 320U);

    // NPC type 8 uses two passes and the original integer scale conversion.
    assert(calls[1].source_class == 0U);
    assert(calls[1].resource_id == 254U);
    assert(calls[1].scale_extent == 222U);
    assert(calls[1].render_mode == 0x58U);
    assert(calls[2].resource_id == 236U);
    assert(calls[2].scale_extent == 222U);
    assert(calls[2].render_mode == 0U);

    // Level 22 placed-object type 62 draws the +8 pass before its base.
    assert(calls[3].source_class == 1U);
    assert(calls[3].resource_id == 2284U);
    assert(calls[3].render_mode == 0x58U);
    assert(calls[4].resource_id == 2276U);
    assert(calls[4].render_mode == 0U);

    // Assassin type 9 uses the ordinary one-pass call.
    assert(calls[5].source_class == 0U);
    assert(calls[5].resource_id == 195U);
    assert(calls[5].scale_extent == 512U);
    assert(calls[5].render_scale_lookup_bias == 3U);
    assert(calls[5].render_mode == 0U);

    // CODE.2 2C5C3's family-C byte +0D bit test selects mode 0x3C.
    world.loose_items[0].cached_cell_index = 0x0200U;
    const auto special_loose =
        rloft::WorldSceneDrawCallModel::build(
            {record(2U, 0U, 4010U, 320U, 0U)},
            world);
    assert(special_loose.size() == 1U);
    assert(special_loose[0].render_mode == 0x3CU);

    // An unimplemented NPC overlay must never be silently omitted.
    world.npcs[0].runtime_flags = 0x08U;
    bool rejected = false;
    try {
        (void)rloft::WorldSceneDrawCallModel::build(
            {record(0U, 0U, 195U, 512U, 0U)},
            world);
    } catch (const std::logic_error&) {
        rejected = true;
    }
    assert(rejected);

    return 0;
}
