#pragma once

#include <cstddef>
#include <cstdint>

#include "rloft/renderer_tables.hpp"
#include "rloft/type7_column_traversal.hpp"

namespace rloft {

// CODE.2 36DB8..36E94 prepares the generated SINTAB code for a type-7 draw.
// 'patch_from' is the first segment temporarily turned into RET (mode 0).
// 'patch_before' is the prefix changed to short JMP (nonzero mode).
// The host port describes those mutations rather than writing executable RAM.
struct Type7RasterizerSetup final {
    bool drawable{};
    bool nonzero_mode{};
    std::int32_t source_center{};
    std::uint32_t render_phase_after_mode{};
    std::uint32_t mode_scaled_offset{};
    std::size_t patch_from{128U};
    std::size_t patch_before{};
};

class Type7RasterizerSetupModel final {
public:
    [[nodiscard]] static Type7RasterizerSetup build(
        const Type7ColumnTraversalPlan& traversal,
        const RendererTables& tables,
        std::uint32_t render_mode);

    // Exact 36B9C/36BD8 search semantics: 16-bit AX accumulation,
    // signed 32-bit comparison, strict greater-than cutoff.
    [[nodiscard]] static std::size_t search_backward(
        const RendererScaleBlock& block,
        std::int32_t threshold) noexcept;
    [[nodiscard]] static std::size_t search_forward(
        const RendererScaleBlock& block,
        std::int32_t threshold) noexcept;
};

} // namespace rloft
