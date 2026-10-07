#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/renderer_tables.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/type7_column_traversal.hpp"

namespace rloft {

// One type-7 descriptor as handed to CODE.2 369D8 for one depth-visible
// destination column. Offsets refer to the selected physical SINTAB block,
// not to an executable native function pointer in the portable process.
struct Type7MicrocodeDraw final {
    std::size_t viewport_x{};
    std::uint16_t source_x{};
    std::size_t source_column_index{};
    std::size_t descriptor_index{};

    std::uint16_t source_row_begin{};
    std::uint16_t source_row_end{};
    std::int16_t signed_source_adjustment{};
    std::uint16_t entry_selector{};
    std::uint16_t stop_selector{};

    std::uint16_t entry_patch_offset{};
    std::uint16_t stop_patch_offset{};
    std::size_t entry_code_offset{};
    std::size_t stop_code_offset{};
};

class Type7MicrocodePlanModel final {
public:
    // CODE.2 369D8 resolves both selectors through the scale block's
    // code_patch_offset[] prefix. This stage records the patch + indirect
    // call operands and native source ranges; it intentionally does not
    // execute or emulate the generated SINTAB x86 instructions.
    [[nodiscard]] static std::vector<Type7MicrocodeDraw> build(
        const Type7SceneShape& shape,
        const Type7ColumnTraversalPlan& traversal,
        const std::vector<Type7VisibleColumn>& visible_columns,
        const RendererTables& tables);
};

}  // namespace rloft
