#include "rloft/type7_column_traversal.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

rloft::Type7SceneShape shape(
    std::uint16_t width,
    std::int16_t x_min,
    std::int16_t x_max) {
    rloft::Type7SceneShape out{};
    out.geometry = {
        width, x_min, x_max, 0, 128
    };
    for (int x = x_min; x <= x_max; ++x) {
        rloft::Type7ColumnDisplayList column{};
        column.source_x =
            static_cast<std::int16_t>(x);
        column.list_offset =
            static_cast<std::uint16_t>(
                200 + x);
        out.columns.push_back(column);
    }
    return out;
}

rloft::RendererTables tables() {
    rloft::RendererTables out{};
    out.scale_block_count = 127U;
    out.scale_blocks.resize(127U);
    for (auto& block : out.scale_blocks) {
        block.scale_step[30] = 1U;
        block.scale_step[31] = 0U;
        block.scale_step[32] = 3U;
        block.scale_step[33] = 2U;
        block.scale_step[66] = 4U;
        block.scale_step[67] = 5U;
    }
    return out;
}

}  // namespace

int main() {
    const auto t = tables();

    // CODE.2 36D13..36DAD:
    // extent 512 -> scale 64 -> adjust (64>>2)>>1 = 8 -> pointer 56.
    // SINTAB pointer 56 refers to serialized block 55.
    const auto plan =
        rloft::Type7ColumnTraversalModel::build(
            shape(64U, 30, 33),
            t,
            4010U,
            512U);
    assert(plan.initial_scale == 64U);
    assert(plan.clamped_scale == 64U);
    assert(plan.scale_pointer_index == 56U);
    assert(plan.scale_block_index == 55U);
    assert(plan.columns.size() == 4U);

    // 36EBD..370AE walks left from center-1 (31), then from 30;
    // 370B6..372C3 walks the right side from 32 through 33.
    assert(plan.columns[0].side ==
           rloft::Type7ColumnSide::Left);
    assert(plan.columns[0].source_x == 31U);
    assert(plan.columns[0].scale_step == 0U);
    assert(plan.columns[0].source_column_index == 1U);
    assert(plan.columns[1].source_x == 30U);
    assert(plan.columns[1].scale_step == 1U);
    assert(plan.columns[2].side ==
           rloft::Type7ColumnSide::Right);
    assert(plan.columns[2].source_x == 32U);
    assert(plan.columns[2].scale_step == 3U);
    assert(plan.columns[3].source_x == 33U);
    assert(plan.columns[3].scale_step == 2U);
    assert(plan.columns[3].display_list_offset == 233U);

    // 36D13 clamps large extents to 127 before selecting the scale block.
    const auto near =
        rloft::Type7ColumnTraversalModel::build(
            shape(64U, 30, 33),
            t,
            4010U,
            65535U);
    assert(near.clamped_scale == 127U);
    assert(near.scale_pointer_index == 112U);
    assert(near.scale_block_index == 111U);

    // A shape entirely right of center must not invent left-side columns.
    const auto right_only =
        rloft::Type7ColumnTraversalModel::build(
            shape(128U, 66, 67),
            t,
            4010U,
            512U);
    assert(right_only.columns.size() == 2U);
    assert(right_only.columns[0].source_x == 66U);
    assert(right_only.columns[0].scale_step == 4U);
    assert(right_only.columns[1].source_x == 67U);
    assert(right_only.columns[1].scale_step == 5U);

    // Extents below 8 exit before accessing the SINTAB code-pointer table.
    const auto tiny =
        rloft::Type7ColumnTraversalModel::build(
            shape(64U, 30, 33),
            rloft::RendererTables{},
            4010U,
            7U);
    assert(tiny.initial_scale == 0U);
    assert(tiny.columns.empty());

    // Preserve 36D23's special resource arithmetic and reject an
    // unrepresentable wrapped native pointer rather than dereferencing it.
    bool rejected = false;
    try {
        (void)rloft::Type7ColumnTraversalModel::build(
            shape(64U, 30, 33),
            t,
            0x117U,
            64U);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected);

    return 0;
}
