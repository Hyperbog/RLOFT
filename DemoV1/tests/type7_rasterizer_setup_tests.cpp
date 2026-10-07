#include "rloft/type7_rasterizer_setup.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace {

rloft::RendererTables tables() {
    rloft::RendererTables out{};
    out.scale_blocks.resize(2U);
    // Constant unit-weight table makes the original strict > searches
    // independently checkable without using any external game assets.
    out.scale_blocks[0].scale_step.fill(1U);
    out.scale_blocks[1].scale_step.fill(1U);
    return out;
}

} // namespace

int main() {
    const auto t = tables();
    rloft::Type7ColumnTraversalPlan traversal{};
    traversal.initial_scale = 64U;
    traversal.scale_pointer_index = 56U;
    traversal.scale_block_index = 0U;
    traversal.renderer_phase_adjustment = 16U;

    // 36DCD..36DE1: center=16+(256-224)/2+(224>>3)=60,
    // normal mode adds 224, so 36B9C searches threshold 28 backward.
    const auto normal =
        rloft::Type7RasterizerSetupModel::build(
            traversal, t, 0U);
    assert(normal.drawable);
    assert(!normal.nonzero_mode);
    assert(normal.source_center == 60);
    assert(normal.patch_from == 100U);
    assert(normal.patch_before == 0U);
    assert(normal.mode_scaled_offset == 0U);
    assert(normal.render_phase_after_mode == 16U);

    // 36E3A uses exact fixed-point multiplication with +0x40 rounding.
    // (224*2*88 + 64)>>8=154. 36BD8 searches threshold 94 forward.
    const auto alternate =
        rloft::Type7RasterizerSetupModel::build(
            traversal, t, 0x58U);
    assert(alternate.drawable);
    assert(alternate.nonzero_mode);
    assert(alternate.source_center == 60);
    assert(alternate.mode_scaled_offset == 154U);
    assert(alternate.patch_before == 94U);
    assert(alternate.patch_from == 128U);
    assert(alternate.render_phase_after_mode ==
           static_cast<std::uint32_t>(0U - 138U));

    // 36B9C returns 127 when the very first backwards addition
    // exceeds the target; 36BD8 uses strict > rather than >=.
    assert(rloft::Type7RasterizerSetupModel::
        search_backward(t.scale_blocks[0], 0) == 127U);
    assert(rloft::Type7RasterizerSetupModel::
        search_forward(t.scale_blocks[0], 0) == 0U);
    assert(rloft::Type7RasterizerSetupModel::
        search_forward(t.scale_blocks[0], 1) == 1U);

    // Original AX accumulation wraps at 16 bits before full-EAX compare.
    auto wrap = t.scale_blocks[0];
    wrap.scale_step.fill(0U);
    wrap.scale_step[0] = 0xFFFFU;
    wrap.scale_step[1] = 2U;
    assert(rloft::Type7RasterizerSetupModel::
        search_forward(wrap, 65535) == 128U);
    assert(rloft::Type7RasterizerSetupModel::
        search_forward(wrap, 0) == 0U);

    // Nonzero mode returns before issuing RES3 column work when center<=0.
    traversal.scale_pointer_index = 112U;
    traversal.renderer_phase_adjustment = 31U;
    const auto early =
        rloft::Type7RasterizerSetupModel::build(
            traversal, t, 0x3CU);
    assert(early.source_center == -9);
    assert(!early.drawable);

    traversal.initial_scale = 0U;
    const auto tiny =
        rloft::Type7RasterizerSetupModel::build(
            traversal, rloft::RendererTables{}, 0U);
    assert(!tiny.drawable);
    return 0;
}
