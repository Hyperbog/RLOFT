#include "rloft/type7_vga_scene_trace.hpp"
#include "rloft/type7_vga_trace_validation.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

rloft::Type7VgaPlaneMaskModel masks() {
    constexpr std::size_t object2 = 0x75F24U;
    constexpr std::size_t locations[3] = {
        0xC668U, 0xC688U, 0xC6A8U
    };
    std::vector<std::uint8_t> file(
        object2 + 0xEF4DU + 1U);
    for (unsigned phase=0U; phase<4U; ++phase) {
        for (unsigned count=1U; count<=7U; ++count) {
            for (unsigned k=0U; k<count; ++k) {
                const auto x = phase + k;
                file[object2 + locations[x / 4U] +
                     phase*8U + count] |=
                    static_cast<std::uint8_t>(
                        1U << (x & 3U));
            }
        }
    }
    return rloft::Type7VgaPlaneMaskModel::
        from_code2(file);
}

rloft::Type7SceneShape shape() {
    rloft::Type7SceneShape s{};
    s.geometry = {64U, 31, 32, 0, 128};
    rloft::Type7ColumnDisplayList left{};
    left.source_x = 31;
    left.descriptors = {
        {4, -7, 2},
        {10, 3, 6},
    };
    rloft::Type7ColumnDisplayList right{};
    right.source_x = 32;
    right.descriptors = {{16, -10, 12}};
    s.columns = {left, right};
    return s;
}

rloft::RendererTables tables() {
    rloft::RendererTables t{};
    t.constants[4] = 328U; // c6e0
    t.scale_blocks.resize(1U);
    auto& block = t.scale_blocks.front();
    block.rasterizer_x86.resize(258U, 0x90U);
    for (std::size_t k=0; k<129U; ++k) {
        block.code_patch_offset[k] =
            static_cast<std::uint16_t>(
                0x204U + 2U*k);
    }
    return t;
}

rloft::WorldSceneRenderPlan plan() {
    rloft::WorldSceneRenderPlan p{};
    p.traversal.lookup_value = 0x1234U;
    p.traversal.scale_block_index = 0U;
    p.rasterizer_setup.drawable = true;
    p.rasterizer_setup.render_phase_after_mode = 5U;
    p.visible_runs = {
        {31U,0U,128U,2U},
        {32U,1U,130U,1U},
    };
    return p;
}

} // namespace

int main() {
    const auto t = tables();
    const auto m = masks();
    const auto sh = shape();
    const auto p = plan();
    // The original 369D8 walks TWO descriptors inside each VGA pass,
    // not two independent 36B14 dispatches that would XCHG ECX twice.
    const auto grouped =
        rloft::Type7VgaSceneTraceModel::build(
            p, sh, t, m, 0U, false);
    assert(grouped.runs.size()==2U);
    for (const auto& run : grouped.runs) {
        rloft::Type7VgaTraceValidator::validate(run);
    }
    assert(grouped.runs[0].width==2U);
    assert(!grouped.runs[0].individual_columns_mode);
    assert(!grouped.runs[1].individual_columns_mode);
    assert(grouped.runs[0].descriptors.size()==2U);
    assert(grouped.runs[0].dispatch.calls.size()==1U);
    assert(grouped.runs[0].dispatch.calls[0].
           sequencer_mask==3U);
    assert(grouped.runs[0].descriptors[0].
           entry_selector==2U);
    assert(grouped.runs[0].descriptors[1].
           stop_selector==10U);
    assert(grouped.runs[1].descriptors.size()==1U);
    assert(grouped.runs[1].dispatch.calls.size()==1U);
    assert(grouped.runs[1].dispatch.calls[0].
           ecx_lookup==0x3412U);
    assert(grouped.runs[1].dispatch.calls[0].
           sequencer_mask==4U);
    assert(grouped.final_lookup_word==0x1234U);

    // Default CODE.2 C029=1: each destination column gets one
    // rasterizer call, with parity-swapped lookup bytes, while the
    // global lookup remains unchanged between source-column runs.
    const auto individual =
        rloft::Type7VgaSceneTraceModel::build(
            p, sh, t, m, 1U, true);
    for (const auto& run : individual.runs) {
        rloft::Type7VgaTraceValidator::validate(run);
    }
    assert(individual.runs[0].individual_columns_mode);
    assert(individual.runs[1].individual_columns_mode);
    assert(individual.runs[0].dispatch.calls.size()==2U);
    assert(individual.runs[0].dispatch.calls[0].
           ecx_lookup==0x1234U);
    assert(individual.runs[0].dispatch.calls[1].
           ecx_lookup==0x3412U);
    assert(individual.runs[1].dispatch.calls[0].
           ecx_lookup==0x1234U);
    assert(individual.final_lookup_word==0x1234U);
    const auto vga_base =
        rloft::VgaPlanarMemory::kPhysicalBase +
        rloft::VgaPlanarMemory::kPageBytes +
        5U*80U + 328U + 32U;
    assert(individual.runs[0].dispatch.calls[0].
           physical_address==vga_base);
    assert(individual.runs[0].dispatch.calls[1].
           physical_address==vga_base);

    auto skipped=p;
    skipped.visible_runs.clear();
    assert(rloft::Type7VgaSceneTraceModel::build(
        skipped,sh,t,m,0U,false).runs.empty());

    auto invalid=p;
    invalid.rasterizer_setup.drawable=false;
    bool rejected=false;
    try {
        (void)rloft::Type7VgaSceneTraceModel::build(
            invalid,sh,t,m,0U,false);
    } catch (const std::logic_error&) {
        rejected=true;
    }
    assert(rejected);
}
