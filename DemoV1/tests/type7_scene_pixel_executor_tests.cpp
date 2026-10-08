#include "rloft/type7_scene_pixel_executor.hpp"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iterator>
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
    plan.traversal.lookup_value=0x0102U;
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
    // Grouped CODE.2 36B14 always swaps AL/AH in its saved ECX after
    // the entire descriptor list, even when the run is one pixel wide.
    run.dispatch.lookup_after_dispatch = 0x0201U;
    trace.runs.push_back(run);
    trace.final_lookup_word = 0x0201U;
    // Production plans contain one complete descriptor list for every
    // accepted physical pixel, not merely a representative VGA trace.
    plan.microcode_draws = trace.runs[0].descriptors;
    for (auto& entry : plan.microcode_draws) {
        entry.viewport_x = 128U;
    }

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
    // A true two-lane mask belongs to a TWO-pixel grouped run. The
    // original 36A2B tables cannot generate mask 0x03 for width one.
    plan.visible_runs[0].width = 2U;
    trace.runs[0].width = 2U;
    for (auto entry : trace.runs[0].descriptors) {
        entry.viewport_x = 129U;
        plan.microcode_draws.push_back(entry);
    }
    trace.runs[0].dispatch.calls[0].sequencer_mask=0x03U;
    const auto two=rloft::Type7ScenePixelExecutor::draw(
        plan,trace,{0xA5U,0xB1U},tables,grouped);
    assert(two.indexed_writes==3U);
    assert(grouped.read(
        rloft::VgaPlanarMemory::kPhysicalBase+80U,1U)==0xAFU);
    assert(grouped.read(
        rloft::VgaPlanarMemory::kPhysicalBase+160U,1U)==0xB0U);

    // Revert to an ordinary one-pixel mask to test source-mismatch
    // rejection without triggering an independent mask inconsistency.
    plan.visible_runs[0].width = 1U;
    trace.runs[0].width = 1U;
    plan.microcode_draws.resize(2U);
    trace.runs[0].dispatch.calls[0].sequencer_mask=0x01U;

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

    // This scene has valid SINTAB bytes and valid individual mask passes,
    // but its purported final lookup word is corrupted. The complete
    // 36B14 execution record must be rejected BEFORE any pixel is written.
    auto wrong_state=trace;
    wrong_state.final_lookup_word=0x1234U;
    rloft::VgaPlanarMemory untouched;
    rejected=false;
    try {
        (void)rloft::Type7ScenePixelExecutor::draw(
            plan,wrong_state,{0xA5U,0xB1U},tables,untouched);
    } catch (const std::invalid_argument&) {
        rejected=true;
    }
    assert(rejected);
    assert(untouched.read(
        rloft::VgaPlanarMemory::kPhysicalBase+80U,0U)==0U);
    assert(untouched.read(
        rloft::VgaPlanarMemory::kPhysicalBase+160U,0U)==0U);

    // The second source descriptor fails AFTER the first descriptor
    // has already executed a valid VGA store. Direct draw() callers
    // must receive precisely their original plane contents, rather
    // than only relying on ExplorationPresenter's outer transaction.
    auto late_failure=trace;
    late_failure.runs[0].descriptors[1].
        signed_source_adjustment=32767;
    rloft::VgaPlanarMemory preserved;
    const auto vga_base=rloft::VgaPlanarMemory::kPhysicalBase;
    preserved.write(vga_base+80U,0x01U,0x5AU);
    preserved.write(vga_base+160U,0x02U,0x71U);
    rejected=false;
    try {
        (void)rloft::Type7ScenePixelExecutor::draw(
            plan,late_failure,{0xA5U,0xB1U},tables,preserved);
    } catch (const std::out_of_range&) {
        rejected=true;
    }
    assert(rejected);
    assert(preserved.read(vga_base+80U,0U)==0x5AU);
    assert(preserved.read(vga_base+160U,1U)==0x71U);
    assert(preserved.read(vga_base+160U,0U)==0U);

    // Fully hidden sprites produce no writes or RES3 source reads.
    rloft::WorldSceneRenderPlan hidden{};
    rloft::Type7VgaSceneTrace empty{};
    assert(rloft::Type7ScenePixelExecutor::draw(
        hidden,empty,{},tables,video).indexed_writes==0U);

    // Optional canonical SINTAB pixel execution regression, using an
    // independent Python-generated reference from the original file:
    //   python3 DemoV1/tools/reference_sintab_pixels.py /path/to/SINTAB
    // No SINTAB bytes are embedded in this repository.
    const auto original_path = std::getenv("RLOFT_SINTAB_PATH");
    if (original_path != nullptr && original_path[0] != 0) {
        std::ifstream input(original_path, std::ios::binary);
        if (!input) {
            throw std::runtime_error("canonical SINTAB cannot be opened");
        }
        const std::vector<std::uint8_t> bytes(
            (std::istreambuf_iterator<char>(input)),
            std::istreambuf_iterator<char>());
        const auto canonical = rloft::RendererTables::parse(bytes);
        const auto& actual_block = canonical.scale_blocks.at(31U);

        rloft::WorldSceneRenderPlan real_plan{};
        real_plan.traversal.scale_block_index = 31U;
        real_plan.traversal.lookup_value = 0x0102U;
        real_plan.rasterizer_setup.drawable = true;
        real_plan.rasterizer_setup.patch_from = 128U;
        real_plan.visible_runs.push_back(
            rloft::Type7VisibleRun{31U,0U,128U,1U});

        rloft::Type7VgaSceneTrace real_trace{};
        rloft::Type7VgaSceneRun real_run{};
        real_run.source_x = 31U;
        real_run.source_column_index = 0U;
        real_run.viewport_x = 128U;
        real_run.width = 1U;
        rloft::Type7MicrocodeDraw real_descriptor{};
        real_descriptor.source_x = 31U;
        real_descriptor.source_column_index = 0U;
        real_descriptor.entry_code_offset =
            actual_block.code_patch_offset[0U]-0x204U;
        real_descriptor.stop_code_offset =
            actual_block.code_patch_offset[16U]-0x204U;
        real_run.descriptors.push_back(real_descriptor);
        real_run.dispatch.calls.push_back({
            128,
            rloft::VgaPlanarMemory::kPhysicalBase,
            0x0102U,
            0x01U,
            0U,
        });
        real_run.dispatch.lookup_after_dispatch = 0x0201U;
        real_trace.runs.push_back(real_run);
        real_trace.final_lookup_word = 0x0201U;
        real_descriptor.viewport_x = 128U;
        real_plan.microcode_draws.push_back(real_descriptor);

        std::vector<std::uint8_t> source(4096U);
        for(std::size_t k=0U;k<source.size();++k) {
            source[k] = static_cast<std::uint8_t>(
                (k*31U+17U)&0xFFU);
        }
        constexpr std::uint64_t fnv_init =
            0xCBF29CE484222325ULL;
        constexpr std::uint64_t fnv_mul =
            0x100000001B3ULL;
        const auto checksum = [](const rloft::VgaPlanarMemory& memory) {
            std::uint64_t hash = fnv_init;
            for(std::uint32_t offset=0U;offset<21000U;++offset) {
                const auto pixel = memory.read(
                    rloft::VgaPlanarMemory::kPhysicalBase+offset,0U);
                hash = (hash^pixel)*fnv_mul;
            }
            return hash;
        };

        struct Case final {
            bool nonzero_mode;
            std::size_t patch_from;
            std::size_t patch_before;
            std::size_t reads;
            std::size_t stores;
            std::uint64_t fingerprint;
        };
        const Case scenarios[] = {
            {false,128U,0U,16U,16U,0xD17D98730D9A77C5ULL},
            {false,8U,0U,8U,8U,0xFF8A6C8D22C4A0C5ULL},
            {true,128U,4U,12U,12U,0x44E1511AB24CEDC5ULL},
        };
        for (const auto& scenario : scenarios) {
            real_plan.rasterizer_setup.nonzero_mode =
                scenario.nonzero_mode;
            real_plan.rasterizer_setup.patch_from =
                scenario.patch_from;
            real_plan.rasterizer_setup.patch_before =
                scenario.patch_before;
            rloft::VgaPlanarMemory destination;
            const auto report =
                rloft::Type7ScenePixelExecutor::draw(
                    real_plan,
                    real_trace,
                    source,
                    canonical,
                    destination);
            assert(report.source_reads==scenario.reads);
            assert(report.indexed_writes==scenario.stores);
            assert(checksum(destination)==scenario.fingerprint);
        }
    }
}
