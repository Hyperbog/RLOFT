#include "rloft/micro_rasterizer_plan.hpp"

#include <cstdint>
#include <stdexcept>

namespace rloft {

MicroRasterizerPlan MicroRasterizerPlanModel::range(
    const RendererScaleBlock& block,
    std::size_t block_index,
    std::size_t first_segment,
    std::size_t stop_segment) {
    if (first_segment > stop_segment ||
        stop_segment >
            MicroRasterizerLayout::kSegmentCount) {
        throw std::out_of_range(
            "micro-rasterizer segment range");
    }

    MicroRasterizerPlan out{};
    out.scale_block_index = block_index;
    out.first_segment = first_segment;
    out.stop_segment = stop_segment;
    out.spans.reserve(stop_segment - first_segment);

    std::uint32_t cursor = 0;
    for (std::size_t index = first_segment;
         index < stop_segment;
         ++index) {
        const auto segment =
            MicroRasterizerLayout::segment(
                block, index);

        const auto begin = cursor;
        cursor +=
            static_cast<std::uint32_t>(
                segment.scale_step);

        out.spans.push_back(
            MicroRasterizerSpan{
                index,
                segment.scale_step,
                begin,
                cursor,
                segment.patch_offset,
                segment.next_patch_offset,
                segment.code_begin,
                segment.code_end,
            });
    }
    out.cumulative_step_total = cursor;
    return out;
}

MicroRasterizerPlan MicroRasterizerPlanModel::full(
    const RendererTables& tables,
    std::uint32_t extent) {
    const auto block_index =
        tables.scale_block_index_for_extent(
            extent);
    const auto& block =
        tables.scale_blocks[block_index];

    return range(
        block,
        block_index,
        0,
        MicroRasterizerLayout::kSegmentCount);
}

MicroRasterizerPlan MicroRasterizerPlanModel::prepass(
    const RendererTables& tables,
    const SurfacePrepassPlan& prepass_plan) {
    if (prepass_plan.scale_block_index >=
        tables.scale_blocks.size()) {
        throw std::out_of_range(
            "prepass scale block index");
    }

    const auto& block =
        tables.scale_blocks[
            prepass_plan.scale_block_index];

    return range(
        block,
        prepass_plan.scale_block_index,
        prepass_plan.entry_segment_index,
        prepass_plan.stop_segment_index);
}

}  // namespace rloft
