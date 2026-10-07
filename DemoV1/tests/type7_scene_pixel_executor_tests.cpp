#include "rloft/type7_scene_pixel_executor.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
    rloft::RendererTables tables{};
    tables.scale_blocks.resize(1U);
    auto& block=tables.scale_blocks.front();
    block.rasterizer_x86={
        0x8A,0x86,0,0,0,0,             // AL=[ESI]
        0x88,0xC4,                    // AH=AL
        0x29,0xC8,                    // EAX-=ECX
        0x88,0x87,0x50,0,0,0,         // [EDI+80]=AL
        0x88,0xA7,0xA0,0,0,0,         // [EDI+160]=AH
        0xC3,
    };
    block.code_patch_offset.fill(0x0204U);

    rloft::WorldSceneRenderPlan plan{};
    plan.traversal.scale_block_index=0U;
    plan.rasterizer_setup.drawable=true;
    plan.rasterizer_setup.patch_from=128U;
    plan.visible_runs={{31U,0U,128U,1U}};

    rloft::Type7VgaSceneTrace trace{};
    rloft::Type7VgaSceneRun run{};
    run.source_x=31U;
    run.source_column_index=0U;
    run.viewport_x=128U;
    run.width=1U;
    rloft::Type7MicrocodeDraw d{};
    d.source_x=31U;
    d.source_column_index=0U;
    d.entry_code_offset=0U;
    d.stop_code_offset=16U;
    d.signed_source_adjustment=0;
    run.descriptors.push_back(d);

    // Second descriptor starts from the next real RES3 source byte and
    // ends at the original terminal RET. This overwrites row one's color
    // and writes row two, exactly as the literal bytecode requires.
    d.stop_code_offset=22U;
    d.signed_source_adjustment=1;
    run.descriptors.push_back(d);
    run.dispatch.calls.push_back({
        128,rloft::VgaPlanarMemory::kPhysicalBase,
        0x0102U,0x01U,0U
    });
    trace.runs.push_back(run);

    rloft::VgaPlanarMemory video;
    const auto result=rloft::Type7ScenePixelExecutor::draw(
        plan,trace,{0xA5U,0xB1U},tables,video);
    assert(result.descriptor_invocations==2U);
    assert(result.vga_passes==2U);
    assert(result.source_reads==2U);
    assert(result.indexed_writes==3U);
    assert(video.read(
        rloft::VgaPlanarMemory::kPhysicalBase+80U,0U)==0xAFU);
    assert(video.read(
        rloft::VgaPlanarMemory::kPhysicalBase+160U,0U)==0xB0U);
    assert(video.read(
        rloft::VgaPlanarMemory::kPhysicalBase+80U,1U)==0U);

    // A 2-plane grouped pass duplicates each indexed SINTAB write in the
    // two selected VGA lanes without inventing a different source color.
    rloft::VgaPlanarMemory grouped;
    trace.runs[0].dispatch.calls[0].sequencer_mask=0x03U;
    const auto two=rloft::Type7ScenePixelExecutor::draw(
        plan,trace,{0xA5U,0xB1U},tables,grouped);
    assert(two.indexed_writes==3U);
    assert(grouped.read(
        rloft::VgaPlanarMemory::kPhysicalBase+80U,1U)==0xAFU);
    assert(grouped.read(
        rloft::VgaPlanarMemory::kPhysicalBase+160U,1U)==0xB0U);

    // A trace cannot be applied to unrelated visible scene columns.
    auto bad=plan;
    bad.visible_runs[0].source_x=30U;
    bool rejected=false;
    try {
        (void)rloft::Type7ScenePixelExecutor::draw(
            bad,trace,{0xA5U,0xB1U},tables,video);
    } catch (const std::invalid_argument&) {
        rejected=true;
    }
    assert(rejected);
    // Fully hidden sprites produce no writes or RES3 source reads.
    rloft::WorldSceneRenderPlan hidden{};
    rloft::Type7VgaSceneTrace empty{};
    assert(rloft::Type7ScenePixelExecutor::draw(
        hidden,empty,{},tables,video).indexed_writes==0U);
}
