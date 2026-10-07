#include "rloft/type7_microcode_plan.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {

std::vector<Type7MicrocodeDraw>
Type7MicrocodePlanModel::build(
    const Type7SceneShape& shape,
    const Type7ColumnTraversalPlan& traversal,
    const std::vector<Type7VisibleColumn>& visible_columns,
    const RendererTables& tables) {
    std::vector<Type7MicrocodeDraw> out;
    if (visible_columns.empty()) {
        return out;
    }

    const auto& block =
        tables.scale_blocks.at(
            traversal.scale_block_index);
    for (const auto& visible : visible_columns) {
        if (visible.source_column_index >=
            shape.columns.size()) {
            throw std::out_of_range(
                "type-7 visible column leaves compiled shape");
        }
        const auto& column =
            shape.columns[visible.source_column_index];
        if (column.source_x < 0 ||
            static_cast<std::uint16_t>(
                column.source_x) != visible.source_x) {
            throw std::invalid_argument(
                "type-7 visible column refers to mismatched source x");
        }

        for (std::size_t descriptor_index = 0;
             descriptor_index < column.descriptors.size();
             ++descriptor_index) {
            const auto& descriptor =
                column.descriptors[descriptor_index];
            const auto entry =
                static_cast<std::uint16_t>(
                    descriptor.raster_entry_selector);
            const auto stop =
                static_cast<std::uint16_t>(
                    descriptor.raster_stop_selector);

            // The zero stop word terminates an on-disk list and is never a
            // descriptor. The 129 SINTAB patch words are selected by the
            // original signed word selector as a byte displacement.
            if (stop == 0U ||
                (entry & 1U) != 0U ||
                (stop & 1U) != 0U ||
                entry > stop ||
                stop > 0x0100U) {
                throw std::invalid_argument(
                    "invalid type-7 raster entry/stop selector");
            }

            const auto begin =
                block.code_patch_offset[entry / 2U];
            const auto end =
                block.code_patch_offset[stop / 2U];
            if (begin < 0x0204U ||
                end < begin ||
                static_cast<std::size_t>(
                    end - 0x0204U) >=
                    block.rasterizer_x86.size()) {
                throw std::runtime_error(
                    "type-7 descriptor references invalid SINTAB code patch");
            }

            out.push_back(
                Type7MicrocodeDraw{
                    visible.viewport_x,
                    visible.source_x,
                    visible.source_column_index,
                    descriptor_index,
                    static_cast<std::uint16_t>(entry / 2U),
                    static_cast<std::uint16_t>(stop / 2U),
                    descriptor.signed_adjustment,
                    entry,
                    stop,
                    begin,
                    end,
                    static_cast<std::size_t>(
                        begin - 0x0204U),
                    static_cast<std::size_t>(
                        end - 0x0204U),
                });
        }
    }
    return out;
}

}  // namespace rloft
