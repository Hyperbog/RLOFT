#include "rloft/surface_prepass.hpp"

#include <cassert>

int main() {
    rloft::RendererTables tables{};
    tables.scale_blocks.resize(16);

    auto& block = tables.scale_blocks[15];
    block.serialized_size = 0x400;
    for (std::size_t i = 0;
         i < block.code_patch_offset.size();
         ++i) {
        block.code_patch_offset[i] =
            static_cast<std::uint16_t>(
                0x204U + i);
    }

    // Extent 64, vertical offset 96:
    // rounded row shift = (64*96 + 64)/128 = 48.
    // 337C8 accumulates 10 + 20 + 30 at slots 96,95,94,
    // then backs up one slot to patch entry 93.
    block.scale_step[96] = 10;
    block.scale_step[95] = 20;
    block.scale_step[94] = 30;

    rloft::LevelState level{};
    level.renderer_vertical_offset = 96;

    rloft::SurfaceShadeTable shades{};
    shades.packed.fill(0x0101);
    shades.fallback = 0x0808;

    rloft::ViewSweepRun run{};
    run.pass = rloft::SweepRenderPass::Prepass;
    run.first_ray = 12;
    run.ray_count = 4;
    run.depth = 128; // extent = 64
    run.resource_id = 7;
    run.texture_column_offset = 0x100;
    run.texture_source_byte_offset = 0x200;
    run.texture_column_index = 4;
    run.shade_decrement = 0;

    const auto plan =
        rloft::SurfacePrepassModel::build(
            run, level, tables, shades);
    assert(plan.has_value());
    assert(plan->extent == 64);
    assert(plan->row_shift == 48);
    assert(plan->screen_x == 44);
    assert(plan->screen_base_row == -44);
    assert(plan->scale_block_index == 15);
    assert(plan->entry_segment_index == 93);
    assert(plan->stop_segment_index == 97);
    assert(plan->entry_offset == 0x204 + 93);
    assert(plan->stop_offset == 0x204 + 97);
    assert(plan->texture_column_index == 4);
    assert(plan->texture_source_byte_offset == 0x200);
    assert(plan->subtract_pair == 0x0101);

    // Runtime slot zero aliases the first serialized scale block.
    tables.scale_blocks.resize(1);
    assert(tables.scale_block_index_for_extent(0) == 0);
    assert(tables.scale_block_index_for_extent(4) == 0);

    rloft::ViewSweepRun main = run;
    main.pass = rloft::SweepRenderPass::Main;
    assert(!rloft::SurfacePrepassModel::build(
                main, level, tables, shades)
                .has_value());
}
