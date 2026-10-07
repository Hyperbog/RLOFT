#include "rloft/micro_rasterizer_layout.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>

int main() {
    rloft::RendererScaleBlock block{};
    block.serialized_size = 0x20AU;
    block.rasterizer_x86 = {
        0x90, 0x90, 0x90,
        0x90, 0x90, 0xC3,
    };

    block.code_patch_offset.fill(0x204U);
    block.scale_step.fill(0);

    // segment 0 consumes bytes 0..1
    block.code_patch_offset[0] = 0x204U;
    block.code_patch_offset[1] = 0x206U;
    block.scale_step[0] = 3;

    // segment 1 is deliberately empty (duplicate boundary).
    block.code_patch_offset[2] = 0x206U;
    block.scale_step[1] = 0;

    // segment 2 consumes bytes 2..4.
    block.code_patch_offset[3] = 0x209U;
    block.scale_step[2] = 2;

    // Remaining boundaries stay at the final boundary.
    for (std::size_t i = 3;
         i < block.code_patch_offset.size();
         ++i) {
        block.code_patch_offset[i] = 0x209U;
    }

    const auto segments =
        rloft::MicroRasterizerLayout::segments(block);
    assert(segments.size() == 128);

    assert(segments[0].index == 0);
    assert(segments[0].patch_offset == 0x204U);
    assert(segments[0].next_patch_offset == 0x206U);
    assert(segments[0].code_begin == 0);
    assert(segments[0].code_end == 2);
    assert(segments[0].scale_step == 3);
    assert(!segments[0].empty());

    assert(segments[1].code_begin == 2);
    assert(segments[1].code_end == 2);
    assert(segments[1].empty());

    assert(segments[2].code_begin == 2);
    assert(segments[2].code_end == 5);
    assert(segments[2].scale_step == 2);

    bool rejected = false;
    try {
        (void)rloft::MicroRasterizerLayout::segment(
            block, 128);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected);
}
