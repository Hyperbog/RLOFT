#include "rloft/surface_sintab_prepass_executor.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
constexpr std::size_t kDgroup = 0x75F24U;
constexpr std::array<std::array<std::uint8_t, 32>, 3> kMasks{{
    {{0,1,3,7,15,15,15,15,15,2,6,14,14,14,14,14,
      14,4,12,12,12,12,12,12,12,8,8,8,8,8,8,8}},
    {{8,0,0,0,0,1,3,7,15,0,0,0,1,3,7,15,
      15,0,0,1,3,7,15,15,15,0,1,3,7,15,15,15}},
    {{15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      1,0,0,0,0,0,0,1,3,0,0,0,0,0,1,3}}
}};
rloft::Type7VgaPlaneMaskModel masks() {
    std::vector<std::uint8_t> data(kDgroup + 0xEF4EU, 0);
    for (std::size_t lane=0; lane<3; ++lane) {
        for (std::size_t i=0; i<32; ++i) {
            data[kDgroup+0xC668U+32U*lane+i] =
                kMasks[lane][i];
        }
    }
    return rloft::Type7VgaPlaneMaskModel::from_code2(data);
}
rloft::RendererTables tables() {
    rloft::RendererTables table{};
    table.runtime_lookup_max = 508;
    table.constants[4] = 328U; // original SINTAB C6E0
    table.scale_blocks.resize(1);
    auto& block = table.scale_blocks[0];
    block.rasterizer_x86 = {
        0x8A,0x86,0,0,0,0, // AL = [ESI]
        0x88,0xC4,         // AH = AL
        0x29,0xC8,         // EAX -= ECX
        0x88,0x87,80,0,0,0,
        0x88,0xA7,160,0,0,0,
        0xC3
    };
    block.code_patch_offset.fill(0x204U);
    for (std::size_t i=97; i<129; ++i) {
        block.code_patch_offset[i] = 0x21AU;
    }
    return table;
}
rloft::SurfaceSintabPrepassReport run(
    const rloft::SurfacePrepassPlan& plan,
    const rloft::RendererTables& table,
    const rloft::Type7VgaPlaneMaskModel& lane_masks,
    rloft::VgaPlanarMemory& video) {
    rloft::SurfaceTexture texture{};
    texture.width = 32;
    texture.texels_column_major.resize(32U*128U,0);
    texture.texels_column_major[128U] = 0xA5;
    return rloft::SurfaceSintabPrepassExecutor::draw(
        plan,texture,table,lane_masks,1U,video);
}
}

int main() {
    const auto lane_masks = masks();
    const auto table = tables();
    const auto original_code =
        table.scale_blocks[0].rasterizer_x86;
    rloft::SurfacePrepassPlan plan{};
    plan.first_ray = 3U;
    plan.ray_count = 3U;
    plan.extent = 4U;  // scale block zero
    plan.texture_column_index = 1U;
    plan.texture_source_byte_offset = 128U;
    plan.screen_x = 35;
    plan.screen_base_row = 4;
    plan.row_shift = 0;
    plan.subtract_pair = 0x0102U;
    plan.entry_segment_index = 0U;
    plan.stop_segment_index = 97U;
    plan.entry_offset = 0x204U;
    plan.stop_offset = 0x21AU;
    rloft::VgaPlanarMemory video;
    const auto report = run(plan,table,lane_masks,video);
    assert(report.vga_calls==2U);
    assert(report.source_reads==2U);
    assert(report.indexed_writes==4U);
    assert(table.scale_blocks[0].rasterizer_x86 == original_code);

    constexpr auto base = rloft::VgaPlanarMemory::kPhysicalBase +
                          rloft::VgaPlanarMemory::kPageBytes +
                          328U;
    // phase=3, width=3: first plane pass mask 08 at byte zero;
    // second mask 03 at the NEXT VGA byte. Both stores are made.
    assert(video.read(base+80U,3U)==0xA3U);
    assert(video.read(base+81U,0U)==0xA3U);
    assert(video.read(base+81U,1U)==0xA3U);
    assert(video.read(base+160U,3U)==0xA4U);
    assert(video.read(base+160U,1U)==0xA4U);
    assert(video.read(base+80U,2U)==0U);
    assert(video.read(rloft::VgaPlanarMemory::kPhysicalBase+328U+80U,
                      3U)==0U); // page zero untouched

    // A different code selector, out-of-domain run, or source column
    // is rejected before modifying the original four VGA planes.
    auto bad = plan;
    bad.stop_offset=0x205U;
    bool rejected=false;
    try {(void)run(bad,table,lane_masks,video);}
    catch (const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    bad=plan;
    bad.ray_count=8U;
    rejected=false;
    try {(void)run(bad,table,lane_masks,video);}
    catch (const std::out_of_range&) {rejected=true;}
    assert(rejected);

    // A later valid rasterizer instruction with an invalid original
    // texture displacement must not commit the earlier successful
    // VGA store. This checks the *public executor's* transaction.
    auto late=table;
    auto& block=late.scale_blocks[0];
    block.rasterizer_x86 = {
        0x8A,0x86,0,0,0,0,
        0x88,0xC4,0x29,0xC8,
        0x88,0x87,80,0,0,0,
        0x8A,0x86,0,0,1,0, // read ESI+65536: outside RES4
        0x88,0xC4,0x29,0xC8,
        0x88,0x87,160,0,0,0,
        0xC3
    };
    for (std::size_t i=97;i<129;++i) {
        block.code_patch_offset[i] = 0x224U;
    }
    bad=plan;
    bad.stop_offset=0x224U;
    rloft::VgaPlanarMemory saved;
    saved.write(base+80U,0x08U,0x77U);
    rejected=false;
    try {(void)run(bad,late,lane_masks,saved);}
    catch (const std::out_of_range&) {rejected=true;}
    assert(rejected);
    assert(saved.read(base+80U,3U)==0x77U);
    assert(saved.read(base+81U,0U)==0U);
}
