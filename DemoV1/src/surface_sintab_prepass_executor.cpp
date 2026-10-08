#include "rloft/surface_sintab_prepass_executor.hpp"

#include "rloft/sintab_pixel_microcode.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {

SurfaceSintabPrepassReport SurfaceSintabPrepassExecutor::draw(
    const SurfacePrepassPlan& plan,
    const SurfaceTexture& texture,
    const RendererTables& tables,
    const Type7VgaPlaneMaskModel& masks,
    unsigned video_page,
    VgaPlanarMemory& video) {
    // 3263C's surface-graphics dispatch addresses one grouped run using
    // the same 32-entry plane-mask tables as 369D8. The original
    // high-level run producer is responsible for splitting long runs.
    if (video_page > 1U || plan.first_ray >= 256U ||
        plan.ray_count == 0U || plan.ray_count > 7U ||
        plan.ray_count > 256U - plan.first_ray) {
        throw std::out_of_range(
            "CODE.2 surface SINTAB prepass page or run geometry");
    }
    if (plan.screen_x != 32 + static_cast<int>(plan.first_ray) ||
        plan.screen_base_row != 4 - plan.row_shift ||
        plan.extent == 0U ||
        plan.extent > tables.runtime_lookup_max ||
        plan.scale_block_index !=
            tables.scale_block_index_for_extent(plan.extent)) {
        throw std::invalid_argument(
            "surface prepass plan is not original SINTAB geometry");
    }
    if (texture.width <= 0 ||
        plan.texture_column_index >=
            static_cast<std::uint32_t>(texture.width) ||
        plan.texture_source_byte_offset !=
            plan.texture_column_index *
                static_cast<std::uint32_t>(SurfaceTexture::kHeight) ||
        texture.texels_column_major.size() !=
            static_cast<std::size_t>(texture.width) *
                SurfaceTexture::kHeight) {
        throw std::invalid_argument(
            "RES4 decoded texture/source pointer does not match prepass");
    }
    const auto& block =
        tables.scale_blocks.at(plan.scale_block_index);
    if (plan.entry_segment_index >=
            block.code_patch_offset.size() ||
        plan.stop_segment_index >=
            block.code_patch_offset.size() ||
        block.code_patch_offset[plan.entry_segment_index] !=
            plan.entry_offset ||
        block.code_patch_offset[plan.stop_segment_index] !=
            plan.stop_offset ||
        plan.entry_offset < 0x204U ||
        plan.stop_offset < 0x204U) {
        throw std::invalid_argument(
            "surface prepass SINTAB entry/stop selectors mismatch");
    }

    // Audit the immutable original block BEFORE installing the one-byte
    // temporary RET; aliased patch offsets are common in shipped SINTAB.
    (void)SintabPixelMicrocode::audit(block);
    auto code = block.rasterizer_x86;
    const auto entry = static_cast<std::size_t>(
        plan.entry_offset - 0x204U);
    const auto stop = static_cast<std::size_t>(
        plan.stop_offset - 0x204U);
    if (entry >= code.size() || stop >= code.size()) {
        throw std::out_of_range(
            "surface prepass SINTAB code offset outside generated body");
    }
    const auto original_stop = code[stop];
    code[stop] = 0xC3U;

    const auto lanes = masks.for_run(
        static_cast<std::uint32_t>(plan.first_ray),
        static_cast<std::uint8_t>(plan.ray_count));

    // 32653..32669: page-select A0000/A8000. 3270B..32719 adds
    // (first_ray>>2)+C6E0. 32774..32784 subtracts row_shift*80
    // from the destination byte pointer. The pointer may begin before
    // the visible page: SINTAB stores use positive vertical offsets.
    const auto destination =
        static_cast<std::int64_t>(
            VgaPlanarMemory::kPhysicalBase) +
        static_cast<std::int64_t>(
            video_page * VgaPlanarMemory::kPageBytes) +
        static_cast<std::int64_t>(tables.c6e0()) +
        static_cast<std::int64_t>(plan.first_ray >> 2U) -
        static_cast<std::int64_t>(plan.row_shift) *
            static_cast<std::int64_t>(VgaPlanarMemory::kRowBytes);
    if (destination < 0 ||
        destination >
            static_cast<std::int64_t>(UINT32_MAX) - 2) {
        throw std::out_of_range(
            "CODE.2 prepass VGA destination arithmetic overflow");
    }

    // One source column is a 128-byte interval in the original
    // decompressed type-6 RES4 texture. The DOS caller has already
    // loaded that column's pixel pointer into dgroup 255E4.
    const auto source_base = static_cast<std::int64_t>(
        plan.texture_source_byte_offset);

    // Preserve the calling program's complete 4-plane memory when any
    // later SINTAB pass fails. Do not mutate canonical generated code.
    auto candidate = video;
    SurfaceSintabPrepassReport report{};
    for (std::size_t pass = 0U; pass < lanes.planes.size(); ++pass) {
        const auto mask = lanes.planes[pass];
        if (pass != 0U && mask == 0U) continue;
        const auto execution = SintabPixelMicrocode::execute(
            code,
            entry,
            texture.texels_column_major,
            source_base,
            static_cast<std::uint32_t>(plan.subtract_pair),
            static_cast<std::uint32_t>(destination +
                                       static_cast<std::int64_t>(pass)),
            mask,
            candidate);
        ++report.vga_calls;
        report.source_reads += execution.source_reads;
        report.indexed_writes += execution.destination_writes;
    }

    code[stop] = original_stop;
    if (code != block.rasterizer_x86) {
        throw std::runtime_error(
            "CODE.2 SINTAB prepass temporary RET was not restored");
    }
    video = std::move(candidate);
    return report;
}

} // namespace rloft
