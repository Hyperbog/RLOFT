#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/micro_rasterizer_layout.hpp"
#include "rloft/renderer_tables.hpp"
#include "rloft/surface_prepass.hpp"

namespace rloft {

struct MicroRasterizerSpan final {
    std::size_t source_index{};
    std::uint16_t step_value{};

    std::uint32_t cumulative_step_begin{};
    std::uint32_t cumulative_step_end{};

    std::uint16_t patch_offset{};
    std::uint16_t next_patch_offset{};
    std::size_t code_begin{};
    std::size_t code_end{};
};

struct MicroRasterizerPlan final {
    std::size_t scale_block_index{};
    std::size_t first_segment{};
    std::size_t stop_segment{};
    std::uint32_t cumulative_step_total{};
    std::vector<MicroRasterizerSpan> spans;
};

class MicroRasterizerPlanModel final {
public:
    // Full CODE.2 generated-rasterizer control span for a normal surface column.\n    // Segment zero is the entry point and boundary 128 is the natural RET.\n    // scale_step words remain evidence-bounded control weights here; this API\n    // does not claim they are literal output-pixel repeat counts.
    [[nodiscard]] static MicroRasterizerPlan full(
        const RendererTables& tables,
        std::uint32_t extent);

    // The FCD0 prepass enters at plan.entry_segment_index and temporarily\n    // patches plan.stop_segment_index with RET, so only [entry, stop) runs.\n    // cumulative_step_* preserves the exact 16-bit table values without\n    // assigning an unproved screen-row meaning.
    [[nodiscard]] static MicroRasterizerPlan prepass(
        const RendererTables& tables,
        const SurfacePrepassPlan& prepass);

    [[nodiscard]] static MicroRasterizerPlan range(
        const RendererScaleBlock& block,
        std::size_t block_index,
        std::size_t first_segment,
        std::size_t stop_segment);
};

}  // namespace rloft
