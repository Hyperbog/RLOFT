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

    // CODE.2 36F4E/36FCE/37157/371D7 checks every destination ray with
    // signed comparison; equal/greater wall depth suppresses the column.
    // Width-zero source column 31 does not advance the left-hand cursor.
    std::vector<rloft::ViewSweepRecord> rays(256U);
    rays[127].depth = 499U;
    rays[128].depth = 499U;
    rays[129].depth = 500U;
    rays[130].depth = 501U;
    rays[131].depth = 0xFFFFFFFFU; // signed -1: visible
    rays[132].depth = 0x80000000U; // signed INT32_MIN: visible
    const auto visible =
        rloft::Type7ColumnTraversalModel::visible_columns(
            plan, 128, 500U, rays);
    assert(visible.size() == 4U);
    assert(visible[0].viewport_x == 127U);
    assert(visible[0].source_x == 30U);
    assert(visible[1].viewport_x == 128U);
    assert(visible[1].source_x == 32U);
    assert(visible[2].viewport_x == 131U);
    assert(visible[2].source_x == 33U);
    assert(visible[3].viewport_x == 132U);
    assert(visible[3].source_x == 33U);

    // A sprite near viewport column zero is cropped, never wrapped to the
    // far-right ray. In particular its leftmost source column emits nothing.
    const auto cropped =
        rloft::Type7ColumnTraversalModel::visible_columns(
            plan, 0, 500U,
            std::vector<rloft::ViewSweepRecord>(2U));
    assert(cropped.size() == 2U);
    assert(cropped[0].source_x == 32U);
    assert(cropped[0].viewport_x == 0U);
    assert(cropped[1].source_x == 32U);
    assert(cropped[1].viewport_x == 1U);

    // No output with no input ray slots, regardless of scene geometry.
    assert(rloft::Type7ColumnTraversalModel::visible_columns(
        plan, 128, 500U, {}).empty());

    // Extents below 8 exit before accessing the SINTAB code-pointer table.
    const auto tiny =
        rloft::Type7ColumnTraversalModel::build(
            shape(64U, 30, 33),
            rloft::RendererTables{},
            4010U,
            7U);
    assert(tiny.initial_scale == 0U);
    assert(tiny.columns.empty());

    // 36D34..36D59 special resource IDs alter 10450, not the selected
    // SINTAB pointer. For scale 64: phase=(64*2)+(128>>3)=144;
    // pointer=64-(64>>3)=56, the same physical block as ordinary IDs.
    const auto special =
        rloft::Type7ColumnTraversalModel::build(
            shape(64U, 30, 33),
            t,
            0x117U,
            512U);
    assert(special.renderer_phase_adjustment == 144U);
    assert(special.scale_pointer_index == 56U);
    assert(special.scale_block_index == 55U);
    assert(plan.renderer_phase_adjustment == 16U);

    // Verify both inclusive/exclusive edges of the original special range.
    const auto special_last =
        rloft::Type7ColumnTraversalModel::build(
            shape(64U, 30, 33),
            t,
            0x13AU,
            512U);
    const auto normal_next =
        rloft::Type7ColumnTraversalModel::build(
            shape(64U, 30, 33),
            t,
            0x13BU,
            512U);
    assert(special_last.renderer_phase_adjustment == 144U);
    assert(normal_next.renderer_phase_adjustment == 16U);
    assert(special_last.scale_pointer_index ==
           normal_next.scale_pointer_index);

    return 0;
}
