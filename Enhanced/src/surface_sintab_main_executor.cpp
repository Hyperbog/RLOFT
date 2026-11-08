#include "rloft/surface_sintab_main_executor.hpp"

#include "rloft/sintab_pixel_microcode.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

namespace rloft {

SurfaceSintabMainReport SurfaceSintabMainExecutor::draw(
    const ViewSweepRun& run,
    const SurfaceTexture& texture,
    const RendererTables& tables,
    const SurfaceShadeTable& shades,
    const Type7VgaPlaneMaskModel& masks,
    unsigned video_page,
    VgaPlanarMemory& video) {
    // Original 336F7..337BF never splits C6F4 on this path.
    // Its 32-byte per-plane tables also permit exactly eight columns
    // at phases 0..2. The phase-3 width-eight read exceeds the
    // canonical table and is deliberately left unsupported.
    if (run.pass != SweepRenderPass::Main || !run.drawable() ||
        video_page > 1U || run.first_ray >= 256U ||
        run.ray_count == 0U || run.ray_count > 8U ||
        run.ray_count > 256U - run.first_ray) {
        throw std::out_of_range(
            "CODE.2 3263C main-pass page or grouped-ray geometry");
    }

    const auto extent = (run.depth & 0xFFF8U) >> 1U;
    if (extent == 0U || extent > tables.runtime_lookup_max) {
        throw std::out_of_range(
            "CODE.2 3263C main-pass extent is outside SINTAB range");
    }
    if (texture.width <= 0 ||
        run.texture_column_index >=
            static_cast<std::uint32_t>(texture.width) ||
        run.texture_source_byte_offset !=
            run.texture_column_index *
                static_cast<std::uint32_t>(SurfaceTexture::kHeight) ||
        texture.texels_column_major.size() !=
            static_cast<std::size_t>(texture.width) *
                SurfaceTexture::kHeight) {
        throw std::invalid_argument(
            "CODE.2 RES4 main texture/source column mismatch");
    }

    // 326F9..32708: extent & FFFCh selects the original runtime
    // rasterizer. FCD0=0 calls it from the START; only FCD0=1
    // temporarily patches a RET at the scale-block selector.
    const auto& block = tables.scale_block_for_extent(extent);
    (void)SintabPixelMicrocode::audit(block);
    const auto& code = block.rasterizer_x86;

    // 3268B..326F6 performs the original two-byte shade/decrement,
    // 0808h clamp and parity swap. 3270B..32719 computes the
    // unshifted main-pass EDI; unlike prepass, there is no row shift.
    const auto lookup = SurfaceShadingModel::packed_subtract(
        shades, run.resource_id, extent, run.shade_decrement,
        run.first_ray);
    const auto lanes = masks.for_surface_main_run(
        static_cast<std::uint32_t>(run.first_ray),
        static_cast<std::uint8_t>(run.ray_count));

    const std::uint64_t destination =
        static_cast<std::uint64_t>(VgaPlanarMemory::kPhysicalBase) +
        static_cast<std::uint64_t>(video_page) *
            VgaPlanarMemory::kPageBytes +
        static_cast<std::uint64_t>(tables.c6e0()) +
        static_cast<std::uint64_t>(run.first_ray >> 2U);
    if (destination >
        static_cast<std::uint64_t>(
            std::numeric_limits<std::uint32_t>::max()) - 2U) {
        throw std::out_of_range(
            "CODE.2 SINTAB main VGA destination overflows DWORD");
    }

    // All three 3263C mask passes use the SAME ESI/ECX and generated
    // code entry. Later passes increment EDI's physical BYTE offset.
    // Stage the four planes so even a late bad original resource/code
    // read cannot tear a visible frame.
    auto staged = video;
    SurfaceSintabMainReport report{};
    for (std::size_t pass = 0U; pass < lanes.planes.size(); ++pass) {
        const auto mask = lanes.planes[pass];
        if (pass != 0U && mask == 0U) continue;
        // 3285E..3293C: each active pass emits OUT 03C4h,
        // AX=(mask<<8)|02h before calling generated code.
        staged.set_sequencer_map_mask_02(mask);
        const auto result = SintabPixelMicrocode::execute(
            code, 0U, texture.texels_column_major,
            static_cast<std::int64_t>(
                run.texture_source_byte_offset),
            static_cast<std::uint32_t>(lookup),
            static_cast<std::uint32_t>(destination + pass),
            mask, staged);
        ++report.vga_calls;
        report.source_reads += result.source_reads;
        report.indexed_writes += result.destination_writes;
    }
    video = std::move(staged);
    return report;
}

} // namespace rloft
