#include "rloft/type7_vga_frame_trace.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

rloft::Type7VgaPlaneMaskModel fixture_masks() {
    constexpr std::size_t base = 0x75F24U;
    constexpr std::size_t offsets[3] = {
        0xC668U, 0xC688U, 0xC6A8U
    };
    std::vector<std::uint8_t> raw(base + 0xEF4DU + 1U);
    raw[base + 0xC029U] = 1U;
    for (std::size_t phase=0U; phase<4U; ++phase)
        for (std::size_t n=1U; n<=7U; ++n)
            for (std::size_t k=0U; k<n; ++k) {
                const auto x=phase+k;
                raw[base+offsets[x/4U]+phase*8U+n] |=
                    static_cast<std::uint8_t>(1U << (x&3U));
            }
    return rloft::Type7VgaPlaneMaskModel::from_code2(raw);
}

rloft::RendererTables tables() {
    rloft::RendererTables out{};
    out.scale_blocks.resize(1U);
    auto& block=out.scale_blocks.front();
    block.rasterizer_x86.assign(258U,0x90U);
    for(std::size_t k=0;k<129U;++k)
        block.code_patch_offset[k]=
            static_cast<std::uint16_t>(0x204U+2U*k);
    return out;
}

rloft::Type7SceneShape shape() {
    rloft::Type7SceneShape s{};
    s.geometry = {64U,31,31,0,128};
    rloft::Type7ColumnDisplayList column{};
    column.source_x=31;
    column.descriptors={{4,-3,2}};
    s.columns={column};
    return s;
}

rloft::WorldSceneRenderPlan plan(
    std::uint16_t id,
    std::uint16_t width) {
    rloft::WorldSceneRenderPlan p{};
    p.call.resource_id=id;
    p.traversal.scale_block_index=0U;
    p.traversal.lookup_value=0x1234U;
    p.rasterizer_setup.drawable=true;
    p.visible_runs={{31U,0U,128U,width}};
    return p;
}

} // namespace

int main() {
    const auto masks=fixture_masks();
    const auto t=tables();
    const auto sh=shape();
    auto visible=plan(15U,2U);
    auto wide=plan(16U,8U);
    auto hidden=plan(17U,1U);
    hidden.visible_runs.clear();

    unsigned lookups=0U;
    const auto lookup=[&](std::uint16_t id)
        -> const rloft::Type7SceneShape& {
        assert(id==15U);
        ++lookups;
        return sh;
    };
    const auto trace=rloft::Type7VgaFrameTraceModel::build(
        {visible,wide,hidden},t,masks,0U,true,lookup);
    assert(trace.scene_calls.size()==3U);
    assert(trace.scene_calls[0].has_value());
    assert(trace.scene_calls[0]->runs.size()==1U);
    assert(trace.scene_calls[0]->runs[0].dispatch.calls.size()==2U);
    assert(!trace.scene_calls[1].has_value());
    assert(trace.unresolved_wide_runs==1U);
    assert(trace.scene_calls[2].has_value());
    assert(trace.scene_calls[2]->runs.empty());
    assert(lookups==1U);

    bool rejected=false;
    try {
        (void)rloft::Type7VgaFrameTraceModel::build(
            {visible},t,masks,2U,true,lookup);
    } catch (const std::out_of_range&) {rejected=true;}
    assert(rejected);

    // A scene that is not projected never needs a shape lookup callback.
    assert(rloft::Type7VgaFrameTraceModel::build(
        {},t,masks,0U,true,{}).scene_calls.empty());

    // Type-7 tracing is optional presentation diagnostics. Runtime
    // construction must capture a malformed page or missing RES3
    // lookup without losing an otherwise valid exploration frame.
    auto success=rloft::Type7VgaFrameTraceModel::try_build(
        {visible,wide,hidden},t,masks,0U,true,lookup);
    assert(success.trace.has_value());
    assert(!success.failure.has_value());
    assert(success.trace->unresolved_wide_runs==1U);

    auto failure=rloft::Type7VgaFrameTraceModel::try_build(
        {visible},t,masks,2U,true,lookup);
    assert(!failure.trace.has_value());
    assert(failure.failure.has_value());
    assert(failure.failure->find("VGA page")!=std::string::npos);

    failure=rloft::Type7VgaFrameTraceModel::try_build(
        {visible},t,masks,0U,true,{});
    assert(!failure.trace.has_value());
    assert(failure.failure.has_value());
    assert(failure.failure->find("shape lookup")!=std::string::npos);
}
