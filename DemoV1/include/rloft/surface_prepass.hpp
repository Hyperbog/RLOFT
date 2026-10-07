#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "rloft/exploration_state.hpp"
#include "rloft/renderer_tables.hpp"
#include "rloft/surface_shading.hpp"
#include "rloft/view_sweep_runs.hpp"

namespace rloft {

struct SurfacePrepassPlan final {
    std::size_t first_ray{};
    std::size_t ray_count{};
    std::uint32_t resource_id{};
    std::uint32_t extent{};
    std::uint32_t texture_column_index{};
    std::uint32_t texture_source_byte_offset{};
    std::uint16_t subtract_pair{};

    int screen_x{};
    int screen_base_row{};
    int row_shift{};

    std::size_t scale_block_index{};
    std::uint16_t entry_offset{};
    std::uint16_t stop_offset{};
};

class SurfacePrepassModel final {
public:
    // Portable setup half of CODE.2 3263C's FCD0=1 path.
    [[nodiscard]] static std::optional<SurfacePrepassPlan> build(
        const ViewSweepRun& run,
        const LevelState& level,
        const RendererTables& tables,
        const SurfaceShadeTable& shades);

private:
    [[nodiscard]] static int rounded_row_shift(
        std::uint32_t extent,
        std::int8_t vertical_offset) noexcept;

    [[nodiscard]] static std::uint16_t entry_offset(
        const RendererScaleBlock& block,
        int vertical_offset,
        std::uint32_t target);
};

}  // namespace rloft
