#pragma once
#include <cstddef>
#include <cstdint>
#include "rloft/surface_fill.hpp"
#include "rloft/vga_planar_memory.hpp"

namespace rloft {

// Isolated CODE.2 1F1D6/33DE0/33E28 linear-address replay.
// ES's DOS selector-to-physical mapping is NOT established. Never
// substitute this for live drawing until that external proof exists.
enum class OriginalGroundFillStrip : std::uint8_t { Upper33E28, Lower33DE0 };

struct OriginalGroundPlanarFillReport final {
    std::uint32_t source_fdbc{};
    std::uint32_t first_linear_address{};
    std::uint32_t last_row_first_linear_address{};
    std::uint32_t last_linear_address{};
    std::size_t planar_byte_addresses_written{};
    std::size_t indexed_lanes_written{};
    std::uint8_t sequencer_plane_mask{};
};

class OriginalGroundPlanarFillReplay final {
public:
    static constexpr std::size_t kRows=0x80U;
    static constexpr std::uint32_t kBytesPerRow=0x40U;
    static constexpr std::uint32_t kStride=0x50U;
    [[nodiscard]] static OriginalGroundPlanarFillReport replay_flat_es(
        VgaPlanarMemory& diagnostic_video,
        const SurfaceFillTable& original_fe94,
        OriginalGroundFillStrip strip,
        std::uint8_t base_color,
        unsigned original_c050_page);
};

} // namespace rloft
