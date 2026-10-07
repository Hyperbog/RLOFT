#include "rloft/world_scene_render_plan.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

rloft::Type7SceneShape shape() {
    rloft::Type7SceneShape out{};
    out.geometry = {64U, 31, 32, 10, 110};
    for (int x = 31; x <= 32; ++x) {
        rloft::Type7ColumnDisplayList column{};
        column.source_x = static_cast<std::int16_t>(x);
        column.list_offset = static_cast<std::uint16_t>(200 + x);
        column.descriptors.push_back(
            rloft::Type7RasterDescriptor{4, -3, 2});
        out.columns.push_back(column);
    }
    return out;
}

rloft::RendererTables tables() {
    rloft::RendererTables out{};
    out.scale_block_count = 127U;
    out.runtime_lookup_max = 508U;
    out.runtime_lookup.assign(509U, 0U);
    out.runtime_lookup[257] = 123U;
    out.scale_blocks.resize(127U);
    for (auto& block : out.scale_blocks) {
        block.scale_step[31] = 1U;
        block.scale_step[32] = 2U;
        block.rasterizer_x86.assign(258U, 0x90U);
        for (std::size_t i = 0; i < 129U; ++i) {
            block.code_patch_offset[i] =
                static_cast<std::uint16_t>(
                    0x204U + i * 2U);
        }
    }
    return out;
}

}  // namespace

int main() {
    const auto t = tables();
    std::vector<rloft::ViewSweepRecord> rays(256U);
    rays[128].depth = 400U; // Equal depth occludes.
    rays[129].depth = 401U;

    rloft::WorldSceneDrawCall first{};
    first.screen_x = 128U;
    first.resource_id = 4010U;
    first.source_class = 2U;
    first.source_index = 3U;
    first.scale_extent = 512U;
    first.raw_depth = 400U;
    first.render_mode = 0U;
    first.render_scale_lookup_bias = 1U;

    rloft::WorldSceneDrawCall second = first;
    second.screen_x = 64U;
    second.resource_id = 4011U;
    second.source_index = 4U;
    second.render_mode = 0x3CU;

    const auto native_shape = shape();
    std::vector<std::uint16_t> looked_up;
    const auto plans = rloft::WorldSceneRenderPlanModel::build(
        {first, second},
        t,
        rays,
        [&looked_up, &native_shape](std::uint16_t id) -> const rloft::Type7SceneShape& {
            looked_up.push_back(id);
            return native_shape;
        });

    assert(plans.size() == 2U);
    assert(looked_up.size() == 2U);
    assert(looked_up[0] == 4010U);
    assert(looked_up[1] == 4011U);
    assert(plans[0].call.resource_id == 4010U);
    assert(plans[0].traversal.scale_block_index == 55U);
    assert(plans[0].traversal.lookup_index == 257U);
    assert(plans[0].traversal.lookup_value == 123U);
    assert(plans[0].rasterizer_setup.drawable);
    assert(!plans[0].rasterizer_setup.nonzero_mode);
    assert(plans[0].visible_columns.size() == 1U);
    assert(plans[0].visible_columns[0].viewport_x == 127U);
    assert(plans[0].visible_columns[0].source_x == 31U);
    assert(plans[0].microcode_draws.size() == 1U);
    assert(plans[0].microcode_draws[0].viewport_x == 127U);
    assert(plans[0].microcode_draws[0].entry_patch_offset == 0x206U);
    assert(plans[0].microcode_draws[0].stop_patch_offset == 0x208U);

    assert(plans[1].call.resource_id == 4011U);
    assert(plans[1].call.render_mode == 0x3CU);
    assert(plans[1].rasterizer_setup.drawable);
    assert(plans[1].rasterizer_setup.nonzero_mode);
    assert(plans[1].visible_columns.size() == 3U);
    assert(plans[1].visible_columns[0].viewport_x == 63U);
    assert(plans[1].visible_columns[1].viewport_x == 64U);
    assert(plans[1].visible_columns[2].viewport_x == 65U);
    assert(plans[1].microcode_draws.size() == 3U);
    assert(plans[1].microcode_draws[0].viewport_x == 63U);
    assert(plans[1].microcode_draws[2].viewport_x == 65U);

    // A nonzero render-mode call that returns at CODE.2 36E31 must not
    // produce visible columns or execute 369D8 descriptors.
    auto suppressed = second;
    suppressed.scale_extent = 65535U;
    const auto suppressed_plan =
        rloft::WorldSceneRenderPlanModel::build(
            {suppressed},
            t,
            rays,
            [&native_shape](std::uint16_t)
                -> const rloft::Type7SceneShape& {
                return native_shape;
            });
    assert(suppressed_plan.size() == 1U);
    assert(!suppressed_plan[0].rasterizer_setup.drawable);
    assert(suppressed_plan[0].visible_columns.empty());
    assert(suppressed_plan[0].microcode_draws.empty());

    // An empty world does not require access to RES3.
    assert(rloft::WorldSceneRenderPlanModel::build(
        {}, t, rays, {}).empty());

    bool rejected = false;
    try {
        (void)rloft::WorldSceneRenderPlanModel::build(
            {first}, t, rays, {});
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);

    return 0;
}
