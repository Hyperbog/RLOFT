#include "rloft/type7_microcode_plan.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

rloft::Type7SceneShape shape() {
    rloft::Type7SceneShape out{};
    out.geometry = {64U, 31, 32, 0, 128};
    rloft::Type7ColumnDisplayList left{};
    left.source_x = 31;
    left.list_offset = 60U;
    left.descriptors = {
        {4, -3, 2},  // source rows [1,2)
        {10, 7, 6},  // source rows [3,5)
    };
    rloft::Type7ColumnDisplayList right{};
    right.source_x = 32;
    right.list_offset = 90U;
    right.descriptors = {{256, -128, 0}}; // [0,128)
    out.columns = {left, right};
    return out;
}

rloft::RendererTables tables() {
    rloft::RendererTables out{};
    out.scale_blocks.resize(1U);
    auto& block = out.scale_blocks.front();
    block.rasterizer_x86.resize(258U, 0x90U);
    for (std::size_t i = 0; i < 129U; ++i) {
        block.code_patch_offset[i] =
            static_cast<std::uint16_t>(0x204U + 2U * i);
    }
    return out;
}

} // namespace

int main() {
    const auto t = tables();
    auto native = shape();
    rloft::Type7ColumnTraversalPlan traversal{};
    traversal.scale_block_index = 0U;

    const std::vector<rloft::Type7VisibleColumn> visible{
        {31U, 0U, 127U},
        {31U, 0U, 126U},
        {32U, 1U, 128U},
    };

    const auto calls =
        rloft::Type7MicrocodePlanModel::build(
            native, traversal, visible, t);
    assert(calls.size() == 5U);
    assert(calls[0].viewport_x == 127U);
    assert(calls[0].source_x == 31U);
    assert(calls[0].source_row_begin == 1U);
    assert(calls[0].source_row_end == 2U);
    assert(calls[0].signed_source_adjustment == -3);
    assert(calls[0].entry_selector == 2U);
    assert(calls[0].stop_selector == 4U);
    assert(calls[0].entry_patch_offset == 0x206U);
    assert(calls[0].stop_patch_offset == 0x208U);
    assert(calls[0].entry_code_offset == 2U);
    assert(calls[0].stop_code_offset == 4U);
    assert(calls[1].descriptor_index == 1U);
    assert(calls[1].source_row_begin == 3U);
    assert(calls[1].source_row_end == 5U);
    assert(calls[2].viewport_x == 126U);
    assert(calls[3].descriptor_index == 1U);
    assert(calls[4].viewport_x == 128U);
    assert(calls[4].source_row_begin == 0U);
    assert(calls[4].source_row_end == 128U);
    assert(calls[4].stop_code_offset == 256U);
    assert(calls[4].signed_source_adjustment == -128);

    assert(rloft::Type7MicrocodePlanModel::build(
        native, traversal, {}, {}).empty());

    // A descriptor cannot execute arbitrary instructions outside its
    // native source range or the selected SINTAB block.
    native.columns[0].descriptors[0].raster_stop_selector = 3;
    bool rejected = false;
    try {
        (void)rloft::Type7MicrocodePlanModel::build(
            native, traversal, visible, t);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);

    // CODE.2 36EA8 subtracts 0x204 from the generated-code pointer
    // before handing SINTAB's original 129-word prefix to 369D8.
    // Descriptor selectors are therefore NONNEGATIVE byte offsets
    // 0..0x100 into the patch table, not signed offsets from the code
    // entrypoint. Negative even selectors would read unrelated memory.
    native = shape();
    native.columns[0].descriptors[0].raster_entry_selector = -2;
    rejected = false;
    try {
        (void)rloft::Type7MicrocodePlanModel::build(
            native, traversal, visible, t);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);

    native = shape();
    auto invalid_block = tables();
    invalid_block.scale_blocks[0].code_patch_offset[2] = 0xFFFFU;
    rejected = false;
    try {
        (void)rloft::Type7MicrocodePlanModel::build(
            native, traversal, visible, invalid_block);
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    assert(rejected);

    rejected = false;
    try {
        (void)rloft::Type7MicrocodePlanModel::build(
            native, traversal, {{33U, 0U, 127U}}, t);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);
}
