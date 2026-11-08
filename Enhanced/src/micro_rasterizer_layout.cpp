#include "rloft/micro_rasterizer_layout.hpp"

#include <stdexcept>

namespace rloft {

MicroRasterizerSegment MicroRasterizerLayout::segment(
    const RendererScaleBlock& block,
    std::size_t index) {
    if (index >= kSegmentCount) {
        throw std::out_of_range(
            "SINTAB rasterizer segment index");
    }

    const auto begin =
        block.code_patch_offset[index];
    const auto end =
        block.code_patch_offset[index + 1U];

    if (begin < 0x204U ||
        end < begin ||
        end < 0x204U) {
        throw std::runtime_error(
            "invalid SINTAB rasterizer patch boundary");
    }

    const auto code_begin =
        static_cast<std::size_t>(begin - 0x204U);
    const auto code_end =
        static_cast<std::size_t>(end - 0x204U);

    if (code_end > block.rasterizer_x86.size()) {
        throw std::runtime_error(
            "SINTAB rasterizer segment exceeds code body");
    }

    return MicroRasterizerSegment{
        index,
        begin,
        end,
        block.scale_step[index],
        code_begin,
        code_end,
    };
}

std::vector<MicroRasterizerSegment>
MicroRasterizerLayout::segments(
    const RendererScaleBlock& block) {
    std::vector<MicroRasterizerSegment> out;
    out.reserve(kSegmentCount);
    for (std::size_t i = 0;
         i < kSegmentCount;
         ++i) {
        out.push_back(segment(block, i));
    }
    return out;
}

}  // namespace rloft
