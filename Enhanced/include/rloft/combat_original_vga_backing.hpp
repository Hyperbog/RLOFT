#pragma once

#include <cstdint>
#include <optional>

#include "rloft/combat_original_dgroup_move.hpp"
#include "rloft/combat_original_overlay_backing_clip.hpp"
#include "rloft/combat_original_vga_copy.hpp"
#include "rloft/vga_planar_memory.hpp"

namespace rloft {

// CODE.2 2A114 and 2A178 use the exact same 2AB38
// address projection as the other original VGA rasterizers.
// The captured DGROUP is a raw DOS offset-addressed byte image.
struct CombatOriginalVgaBackingResult final {
    std::uint32_t initial_linear_vga_address{};
    std::uint8_t initial_column_plane{};
    std::uint32_t physical_bytes_per_row{};
    std::uint32_t backing_bytes_transferred{};
};

class CombatOriginalVgaBackingModel final {
public:
    // CODE.2 2A114: for each of four read-map plane selections,
    // copy rows*rounded_bytes from planar VGA to consecutive
    // captured DGROUP bytes. Leaves GC read-map index 04h at 3.
    [[nodiscard]] static std::optional<CombatOriginalVgaBackingResult>
    capture_2a114(
        VgaPlanarMemory& original_vga,
        CombatOriginalDgroupCapture& original_dgroup,
        const CombatOriginalVgaCopyState& live_video,
        const CombatOriginalOverlayBackingRequest& request);

    // CODE.2 2A178: inverse of the capture layout, with source
    // bytes in four consecutive DGROUP plane blocks; sequencer
    // masks 1,2,4,8 and final OUT 03C4h,AX=FF02h (mask 0x0F).
    [[nodiscard]] static std::optional<CombatOriginalVgaBackingResult>
    restore_2a178(
        VgaPlanarMemory& original_vga,
        const CombatOriginalDgroupCapture& original_dgroup,
        const CombatOriginalVgaCopyState& live_video,
        const CombatOriginalOverlayBackingRequest& request);
};

} // namespace rloft
