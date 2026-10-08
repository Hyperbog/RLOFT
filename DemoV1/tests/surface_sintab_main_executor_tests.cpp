#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/surface_sintab_main_executor.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
constexpr std::size_t kDgroup = 0x75F24U;
constexpr std::array<std::array<std::uint8_t,32>,3> kMasks{{
    {{0,1,3,7,15,15,15,15,15,2,6,14,14,14,14,14,
      14,4,12,12,12,12,12,12,12,8,8,8,8,8,8,8}},
    {{8,0,0,0,0,1,3,7,15,0,0,0,1,3,7,15,
      15,0,0,1,3,7,15,15,15,0,1,3,7,15,15,15}},
    {{15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      1,0,0,0,0,0,0,1,3,0,0,0,0,0,1,3}}
}};
rloft::Type7VgaPlaneMaskModel masks() {
    std::vector<std::uint8_t> code2(kDgroup+0xEF4EU,0U);
    for (std::size_t lane=0;lane<3;++lane)
        for (std::size_t i=0;i<32;++i)
            code2[kDgroup+0xC668U+lane*32U+i]=kMasks[lane][i];
    return rloft::Type7VgaPlaneMaskModel::from_code2(code2);
}
rloft::RendererTables tables() {
    rloft::RendererTables t{};
    t.runtime_lookup_max=508U;
    t.constants[4]=328U; // shipped C6E0
    t.scale_blocks.resize(1);
    auto& b=t.scale_blocks[0];
    b.code_patch_offset.fill(0x204U);
    b.rasterizer_x86={
        0x8A,0x86,0,0,0,0, // MOV AL,[ESI]
        0x88,0xC4,         // MOV AH,AL
        0x29,0xC8,         // SUB EAX,ECX
        0x88,0x87,80,0,0,0,// store AL at EDI+80
        0x88,0xA7,160,0,0,0,// store AH at EDI+160
        0xC3
    };
    return t;
}
rloft::SurfaceTexture texture() {
    rloft::SurfaceTexture out{};
    out.width=2;
    out.texels_column_major.resize(256U,0U);
    out.texels_column_major[128U]=0xA5U;
    return out;
}
rloft::ViewSweepRun main_run() {
    rloft::ViewSweepRun run{};
    run.pass=rloft::SweepRenderPass::Main;
    run.first_ray=3U;
    run.ray_count=3U;
    run.depth=8U; // (8 & FFF8h)>>1 = extent 4
    run.resource_id=0U;
    run.texture_column_index=1U;
    run.texture_source_byte_offset=128U;
    return run;
}
} // namespace

int main() {
    using namespace rloft;
    auto t=tables();
    const auto original_code=t.scale_blocks[0].rasterizer_x86;
    const auto lane_masks=masks();
    const auto source=texture();
    auto run=main_run();
    SurfaceShadeTable shades{};
    shades.packed.fill(0x0102U);
    shades.fallback=0x0102U;
    VgaPlanarMemory video{};

    // Main pass executes the full natural-RET SINTAB body, WITHOUT
    // the prepass's synthetic RET patch or vertical row shift.
    const auto report=SurfaceSintabMainExecutor::draw(
        run,source,t,shades,lane_masks,1U,video);
    assert(report.vga_calls==2U);
    assert(report.source_reads==2U);
    assert(report.indexed_writes==4U);
    assert(t.scale_blocks[0].rasterizer_x86==original_code);
    constexpr auto dest=VgaPlanarMemory::kPhysicalBase+
        VgaPlanarMemory::kPageBytes+328U;
    // x phase 3 and count 3 selects 08h + 03h VGA masks.
    // Odd x swaps the packed shade's low/high bytes.
    assert(video.read(dest+80U,3U)==0xA4U);
    assert(video.read(dest+81U,0U)==0xA4U);
    assert(video.read(dest+81U,1U)==0xA4U);
    assert(video.read(dest+160U,3U)==0xA3U);
    assert(video.read(dest+161U,0U)==0xA3U);
    assert(video.read(dest+161U,1U)==0xA3U);
    assert(video.read(dest+80U,2U)==0U);
    assert(video.read(VgaPlanarMemory::kPhysicalBase+
                      328U+80U,3U)==0U);

    // Bounds and source-byte authenticity are checked BEFORE drawing.
    auto bad=run;
    bad.pass=SweepRenderPass::Prepass;
    bool rejected=false;
    try {
        (void)SurfaceSintabMainExecutor::draw(
            bad,source,t,shades,lane_masks,1U,video);
    } catch (const std::out_of_range&) {rejected=true;}
    assert(rejected);
    bad=run;
    bad.ray_count=8U;
    rejected=false;
    try {
        (void)SurfaceSintabMainExecutor::draw(
            bad,source,t,shades,lane_masks,1U,video);
    } catch (const std::out_of_range&) {rejected=true;}
    assert(rejected);
    bad=run;
    bad.texture_source_byte_offset=127U;
    rejected=false;
    try {
        (void)SurfaceSintabMainExecutor::draw(
            bad,source,t,shades,lane_masks,1U,video);
    } catch (const std::invalid_argument&) {rejected=true;}
    assert(rejected);

    // Late SINTAB read failure must preserve earlier original video.
    auto broken=t;
    broken.scale_blocks[0].rasterizer_x86={
        0x8A,0x86,0,0,0,0,0x88,0xC4,0x29,0xC8,
        0x88,0x87,80,0,0,0,
        0x8A,0x86,0,0,1,0,0x88,0xC4,0x29,0xC8,
        0x88,0xA7,160,0,0,0,0xC3
    };
    VgaPlanarMemory unchanged{};
    unchanged.write(dest+80U,0x08U,0x77U);
    rejected=false;
    try {
        (void)SurfaceSintabMainExecutor::draw(
            run,source,broken,shades,lane_masks,1U,unchanged);
    } catch (const std::out_of_range&) {rejected=true;}
    assert(rejected);
    assert(unchanged.read(dest+80U,3U)==0x77U);
    assert(unchanged.read(dest+161U,0U)==0U);
}
