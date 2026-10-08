#pragma once
#include <cstdint>
#include <optional>
#include "rloft/combat_original_sprite_raster.hpp"
#include "rloft/combat_original_sprite_machine.hpp"
#include "rloft/combat_original_dgroup_move.hpp"

namespace rloft {
// The ten original mutable DWORD globals written by CODE.2 2A6B0.
// Original machine code also patches its own CMP immediate at 2A816.
struct CombatOriginalClippedRasterState final {
    std::uint32_t top_skip_bytes_f00d{};
    std::uint32_t bottom_skip_bytes_f011{};
    std::uint32_t visible_rows_f015{};
    std::uint32_t left_skip_bytes_f019{};
    std::uint32_t right_skip_bytes_f01d{};
    std::uint32_t visible_plane_bytes_f021{};
    std::uint32_t mod_four_f025{};
    std::uint32_t right_flag_f029{};
    std::uint32_t initial_skipped_planes_f02d{};
    std::uint32_t remaining_plane_passes_f031{};
    std::uint32_t live_row_counter_f009{};
    std::uint8_t code_comparison_byte_2a816{};
};
struct CombatOriginalClippedRasterResult final {
    std::uint32_t bytes_compared{};
    std::uint32_t bytes_written{};
    std::uint32_t transparent_skipped{};
    std::uint32_t plane_passes{};
    std::uint32_t frame_source_bytes_advanced{};
    std::uint8_t final_raw_sequencer_mask{};
    bool transparent_resource{};
};
class CombatOriginalClippedRasterModel final {
public:
    // Original 2A6B0(frame,row,column,scratch,resource), with caller-owned
    // 24-byte six-DWORD record at its original DGROUP offset. Source
    // resource header, frame bytes and VGA aperture must be captured.
    [[nodiscard]] static std::optional<CombatOriginalClippedRasterResult>
    draw_2a6b0(
        VgaPlanarMemory& vga,
        const CombatOriginalSpriteMemory& sprite,
        const CombatOriginalDgroupCapture& dgroup,
        std::uint32_t frame_bits,
        std::uint32_t row,
        std::uint32_t column,
        std::uint32_t scratch_pointer,
        const CombatOriginalSpriteVideoState& video,
        CombatOriginalClippedRasterState& original_state,
        CombatOriginalSpriteMachineState* mutable_code_image=nullptr);
};
} // namespace rloft
