#include "rloft/micro_rasterizer_plan.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace {

rloft::RendererScaleBlock block() {
    rloft::RendererScaleBlock b{};
    b.serialized_size = 0x220U;
    b.rasterizer_x86.assign(0x1CU, 0x90U);
    b.rasterizer_x86.back() = 0xC3U;

    b.code_patch_offset.fill(0x220U);
    b.scale_step.fill(0);

    // Four executable source-row segments followed by a common boundary.
    b.code_patch_offset[0] = 0x204U;
    b.code_patch_offset[1] = 0x208U;
    b.code_patch_offset[2] = 0x208U;
    b.code_patch_offset[3] = 0x20EU;
    b.code_patch_offset[4] = 0x212U;
    for (std::size_t i = 5;
         i < b.code_patch_offset.size();
         ++i) {
        b.code_patch_offset[i] = 0x220U;
    }

    b.scale_step[0] = 2;
    b.scale_step[1] = 0;
    b.scale_step[2] = 3;
    b.scale_step[3] = 1;
    return b;
}

}  // namespace

int main() {
    rloft::RendererTables tables{};
    tables.scale_blocks.push_back(block());

    const auto full =
        rloft::MicroRasterizerPlanModel::full(
            tables, 4);
    assert(full.scale_block_index == 0);
    assert(full.first_segment == 0);
    assert(full.stop_segment == 128);
    assert(full.cumulative_step_total == 6);
    assert(full.spans.size() == 128);

    assert(full.spans[0].source_index == 0);
    assert(full.spans[0].step_value == 2);
    assert(full.spans[0].cumulative_step_begin == 0);
    assert(full.spans[0].cumulative_step_end == 2);
    assert(full.spans[0].patch_offset == 0x204U);
    assert(full.spans[0].next_patch_offset == 0x208U);
    assert(full.spans[0].code_begin == 0);
    assert(full.spans[0].code_end == 4);

    assert(full.spans[1].source_index == 1);
    assert(full.spans[1].step_value == 0);
    assert(full.spans[1].cumulative_step_begin == 2);
    assert(full.spans[1].cumulative_step_end == 2);
    assert(full.spans[1].code_begin == 4);
    assert(full.spans[1].code_end == 4);

    assert(full.spans[2].step_value == 3);
    assert(full.spans[2].cumulative_step_begin == 2);
    assert(full.spans[2].cumulative_step_end == 5);

    const auto partial =
        rloft::MicroRasterizerPlanModel::range(
            tables.scale_blocks[0],
            0,
            1,
            4);
    assert(partial.spans.size() == 3);
    assert(partial.cumulative_step_total == 4);
    assert(partial.spans[0].source_index == 1);
    assert(partial.spans[1].source_index == 2);
    assert(partial.spans[1].cumulative_step_begin == 0);
    assert(partial.spans[1].cumulative_step_end == 3);
    assert(partial.spans[2].cumulative_step_end == 4);

    rloft::SurfacePrepassPlan prepass{};
    prepass.scale_block_index = 0;
    prepass.entry_segment_index = 1;
    prepass.stop_segment_index = 4;
    const auto pre =
        rloft::MicroRasterizerPlanModel::prepass(
            tables, prepass);
    assert(pre.first_segment == 1);
    assert(pre.stop_segment == 4);
    assert(pre.cumulative_step_total == 4);

    bool rejected = false;
    try {
        (void)rloft::MicroRasterizerPlanModel::range(
            tables.scale_blocks[0],
            0,
            7,
            6);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected);
}
