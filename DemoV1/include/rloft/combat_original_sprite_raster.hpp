#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include "rloft/vga_planar_memory.hpp"

namespace rloft {

// A bounded view of real 32-bit DOS memory, NOT a modern Sprite/Raster.
// The original 2A380 resource pointer may reference a header in the
// middle of a font bank: signed-negative glyph frames are permitted.
struct CombatOriginalSpriteMemory final {
    std::uint32_t captured_base_address{};
    std::uint32_t resource_header_address{};
    std::vector<std::uint8_t> bytes;
};

struct CombatOriginalSpriteVideoState final {
    std::uint32_t row{};
    std::uint32_t column{};
    std::uint32_t first_row_shift_efed{};
    std::uint32_t second_row_shift_eff1{};
    std::uint32_t page_base_f001{};
    std::uint32_t row_pitch_eff9{};
};

struct CombatOriginalSpriteRasterResult final {
    bool used_transparent_branch{};
    std::uint8_t sentinel{};
    std::uint8_t final_sequencer_plane_mask{};
    std::uint32_t source_bytes_consumed{};
    std::uint32_t vga_bytes_written{};
    std::uint32_t transparent_bytes_skipped{};
};

// CODE.2 2A380..2A477: source-backed opaque/transparent four-plane
// sprite copy into actual VgaPlanarMemory. This is the NORMAL path;
// the separate shaded/self-patching 2A584 path is not implied.
class CombatOriginalSpriteRasterModel final {
public:
    // No fallback graphics and no invented DOS memory reads. If header,
// source span or VGA aperture is incomplete, returns nullopt without
    // modifying any planes. Successful writes preserve source order.
    [[nodiscard]] static std::optional<CombatOriginalSpriteRasterResult>
    draw_2a380(
        VgaPlanarMemory& memory,
        const CombatOriginalSpriteMemory& source,
        std::uint32_t original_frame_index_bits,
        const CombatOriginalSpriteVideoState& video);
};

} // namespace rloft
