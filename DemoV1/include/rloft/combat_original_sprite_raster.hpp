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

// 2A584 stores the FULL 32-bit original lookup-table pointer into
// CODE.2 instruction operands at 2A60B and 2A672. The two MOV AL,
// [EBX+patched_pointer] sites use raw source BYTE as unsigned index.
// No invented brightness or RGB conversion is permitted.
struct CombatOriginalShaderMemory final {
    std::uint32_t captured_base_address{};
    std::uint32_t original_lookup_pointer{};
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
    bool used_shader_lookup{};
    std::uint32_t shader_pointer_self_patched{};
    std::uint8_t sentinel{};
    std::uint8_t final_sequencer_plane_mask{};
    std::uint32_t source_bytes_consumed{};
    std::uint32_t vga_bytes_written{};
    std::uint32_t transparent_bytes_skipped{};
};

// CODE.2 2A380..2A477 and 2A584..2A6B0: source-backed
// opaque/transparent four-plane sprite copies into VgaPlanarMemory.
// Shaded calls use the *original* 256-entry lookup table.
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

    // CODE.2 2A584..2A6B0: original shaded sprite uses a full
    // 256-byte table indexed by RAW source color, not SUB or tint.
    // Transparency compares its unmodified sentinel BEFORE lookup.
    // The result reports the source-code operand pointer to be
    // patched in real DOS code memory by a later execution layer.
    [[nodiscard]] static std::optional<CombatOriginalSpriteRasterResult>
    draw_2a584(
        VgaPlanarMemory& memory,
        const CombatOriginalSpriteMemory& source,
        std::uint32_t original_frame_index_bits,
        const CombatOriginalSpriteVideoState& video,
        const CombatOriginalShaderMemory& shader);
};

} // namespace rloft
