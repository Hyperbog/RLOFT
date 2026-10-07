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
    std::uint16_t repeat_count{};

    std::uint32_t output_begin{};
    std::uint32_t output_end{};

    std::uint16_t patch_offset{};
    std::uint16_t next_patch_offset{};
    std::size_t code_begin{};
    std::size_t code_end{};
};

struct MicroRasterizerPlan final {
    std::size_t scale_block_index{};
    std::size_t first_segment{};
    std::size_t stop_segment{};
    std::uint32_t output_count{};
    std::vector<MicroRasterizerSpan> spans;
};

class MicroRasterizerPlanModel final {
public:
    // Full CODE.2 generated-rasterizer call for a normal surface column.
    // Segment zero is the entry point and boundary 128 is the natural RET.
    [[nodiscard]] static MicroRasterizerPlan full(
        const RendererTables& tables,
        std::uint32_t extent);

    // The FCD0 prepass enters at plan.entry_segment_index and temporarily
    // patches plan.stop_segment_index with RET, so only [entry, stop) runs.
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
