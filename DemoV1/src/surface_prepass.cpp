#include "rloft/surface_prepass.hpp"

#include <cstdint>
#include <stdexcept>

namespace rloft {

int SurfacePrepassModel::rounded_row_shift(
    std::uint32_t extent,
    std::int8_t vertical_offset) noexcept {
    // CODE.2 app83_mul_idiv128: multiply, add 0x40, signed-magnitude
    // divide by 0x80. Shipped level headers use +96.
    const auto factor =
        static_cast<std::int64_t>(vertical_offset);
    const auto value =
        static_cast<std::int64_t>(extent) * factor +
        (factor >= 0 ? 0x40 : -0x40);
    return static_cast<int>(value / 0x80);
}

SurfacePrepassModel::EntrySelection
SurfacePrepassModel::entry_selection(
    const RendererScaleBlock& block,
    int vertical_offset,
    std::uint32_t target) {
    if (vertical_offset < 0 ||
        vertical_offset > 127) {
        throw std::out_of_range(
            "renderer vertical offset leaves SINTAB prefix");
    }

    int slot = vertical_offset;
    std::uint16_t sum = 0;

    for (;;) {
        sum = static_cast<std::uint16_t>(
            sum + block.scale_step[
                static_cast<std::size_t>(slot)]);
        if (static_cast<std::uint32_t>(sum) >= target) {
            break;
        }
        // Original CODE.2 337EF..337F9 exits the search after
        // decrementing below zero and returns patch selector zero;
        // it does not signal a failed render. The final unconditional
        // decrement/clamp at 337F4..337F9 is reproduced below.
        if (slot == 0) {
            break;
        }
        --slot;
    }

    if (slot >= 1) {
        --slot;
    } else {
        slot = 0;
    }
    const auto index =
        static_cast<std::size_t>(slot);
    return EntrySelection{
        index,
        block.code_patch_offset[index],
    };
}

std::optional<SurfacePrepassPlan> SurfacePrepassModel::build(
    const ViewSweepRun& run,
    const LevelState& level,
    const RendererTables& tables,
    const SurfaceShadeTable& shades) {
    if (run.pass != SweepRenderPass::Prepass ||
        !run.drawable()) {
        return std::nullopt;
    }

    const auto extent =
        (run.depth & 0xFFF8U) >> 1U;
    if (extent == 0U) {
        return std::nullopt;
    }

    const auto half = extent >> 1U;
    const auto top = (extent >> 3U) + 0x80U;
    if (static_cast<std::int32_t>(top - half) <= 0) {
        return std::nullopt;
    }

    int row_shift = rounded_row_shift(
        extent,
        level.renderer_vertical_offset);

    const auto remaining =
        static_cast<std::int32_t>(
            top - half -
            static_cast<std::uint32_t>(row_shift));
    if (remaining < 0) {
        row_shift += remaining;
    }
    if (row_shift < 0) {
        throw std::runtime_error(
            "negative prepass row shift is unsupported by shipped data");
    }

    const auto block_index =
        tables.scale_block_index_for_extent(extent);
    const auto& block =
        tables.scale_blocks[block_index];

    const int patch_index =
        static_cast<int>(
            level.renderer_vertical_offset) + 1;
    if (patch_index < 0 || patch_index > 128) {
        throw std::out_of_range(
            "renderer prepass patch index");
    }

    SurfacePrepassPlan out{};
    out.first_ray = run.first_ray;
    out.ray_count = run.ray_count;
    out.resource_id = run.resource_id;
    out.extent = extent;
    out.texture_column_index =
        run.texture_column_index;
    out.texture_source_byte_offset =
        run.texture_source_byte_offset;
    out.subtract_pair =
        SurfaceShadingModel::packed_subtract(
            shades,
            run.resource_id,
            extent,
            run.shade_decrement,
            run.first_ray);

    out.screen_x =
        32 + static_cast<int>(run.first_ray);
    out.screen_base_row = 4 - row_shift;
    out.row_shift = row_shift;
    out.scale_block_index = block_index;
    out.stop_segment_index =
        static_cast<std::size_t>(patch_index);
    out.stop_offset =
        block.code_patch_offset[
            out.stop_segment_index];

    const auto entry = entry_selection(
        block,
        static_cast<int>(
            level.renderer_vertical_offset),
        static_cast<std::uint32_t>(row_shift));
    out.entry_segment_index =
        entry.segment_index;
    out.entry_offset = entry.offset;
    return out;
}

}  // namespace rloft
