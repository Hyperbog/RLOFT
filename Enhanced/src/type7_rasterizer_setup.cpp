#include "rloft/type7_rasterizer_setup.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {

std::size_t Type7RasterizerSetupModel::search_backward(
    const RendererScaleBlock& block,
    std::int32_t threshold) noexcept {
    std::uint16_t accumulator = 0U;
    int index = 127;
    // 36BBB accumulates in AX, while 36BBF compares the full EAX.
    for (; index >= 0; --index) {
        accumulator = static_cast<std::uint16_t>(
            accumulator + block.scale_step[
                static_cast<std::size_t>(index)]);
        if (static_cast<std::int32_t>(
                accumulator) > threshold) {
            break;
        }
    }
    // 36BC7 increments the selected index, then caps it at 0x7F.
    return std::min<std::size_t>(
        static_cast<std::size_t>(index + 1),
        127U);
}

std::size_t Type7RasterizerSetupModel::search_forward(
    const RendererScaleBlock& block,
    std::int32_t threshold) noexcept {
    std::uint16_t accumulator = 0U;
    std::size_t index = 0U;
    for (; index < 128U; ++index) {
        accumulator = static_cast<std::uint16_t>(
            accumulator + block.scale_step[index]);
        if (static_cast<std::int32_t>(
                accumulator) > threshold) {
            break;
        }
    }
    // 36C09 returns the stopping index, potentially 0x80.
    return index;
}

Type7RasterizerSetup Type7RasterizerSetupModel::build(
    const Type7ColumnTraversalPlan& traversal,
    const RendererTables& tables,
    std::uint32_t render_mode) {
    Type7RasterizerSetup out{};
    out.nonzero_mode = render_mode != 0U;
    out.render_phase_after_mode =
        traversal.renderer_phase_adjustment;
    if (traversal.initial_scale == 0U) {
        return out;
    }

    const auto& block = tables.scale_blocks.at(
        traversal.scale_block_index);
    const auto byte_index =
        traversal.scale_pointer_index * 4U;
    const auto signed_byte_index =
        static_cast<std::int32_t>(byte_index);
    // CODE.2 36DCD/36E2A uses arithmetic SAR of (256-index*4).
    // The index is a multiple of 4, making this quotient exact.
    const auto center =
        static_cast<std::int32_t>(
            traversal.renderer_phase_adjustment) +
        (256 - signed_byte_index) / 2 +
        static_cast<std::int32_t>(byte_index >> 3U);
    out.source_center = center;

    if (render_mode == 0U) {
        // 36DDE..36E0D adds the block-pointer byte index, then patches
        // 36C10's RET sites starting at the backward-search boundary.
        const auto stop_target =
            center + signed_byte_index;
        out.patch_from =
            stop_target > 256
                ? search_backward(block, stop_target - 256)
                : 128U;
        out.drawable = true;
        return out;
    }

    // 36E2E returns immediately if the nonzero-mode center is not
    // positive, without accessing the compiled scene shape.
    if (center <= 0) {
        return out;
    }

    // 36E3A..36E51: SHL EAX, MUL arg5, add 0x40 with carry, SHRD by 8.
    // A uint64 preserves precisely the low 32 bits and carry of this
    // 64-bit product; the machine truncates the final shifted result.
    const auto product =
        static_cast<std::uint64_t>(
            byte_index * 2U) *
        static_cast<std::uint64_t>(
            render_mode) +
        0x40ULL;
    out.mode_scaled_offset =
        static_cast<std::uint32_t>(
            product >> 8U);
    out.render_phase_after_mode -=
        out.mode_scaled_offset;

    const auto remaining =
        static_cast<std::int64_t>(
            center) -
        static_cast<std::int64_t>(
            out.mode_scaled_offset);
    if (remaining < 0) {
        // 36BD8 finds the number of leading segments to patch. 36C44
        // replaces each of their first two code bytes with short JMP.
        out.patch_before = search_forward(
            block,
            static_cast<std::int32_t>(
                -remaining));
    }
    out.drawable = true;
    return out;
}

} // namespace rloft
