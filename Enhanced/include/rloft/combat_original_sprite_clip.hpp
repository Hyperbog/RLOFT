#pragma once

#include <cstdint>
#include <functional>
#include <optional>

#include "rloft/combat_original_dgroup_move.hpp"

namespace rloft {

// Original CODE.2 29414 viewport clipping. These are LIVE DWORD
// boundaries, not assumptions about Level 22 or 640x480 windows.
struct CombatOriginalSpriteClipGlobals final {
    std::uint32_t viewport_first_row_ef55{};
    std::uint32_t viewport_last_row_ef59{};
    std::uint32_t viewport_first_column_ef5d{};
    std::uint32_t viewport_last_column_ef61{};
    std::uint16_t output_row_ef69{};
    std::uint16_t output_column_ef6b{};
    std::uint16_t output_rows_ef6d{};
    std::uint16_t output_columns_ef6f{};
};

struct CombatOriginalSpriteClipServices final {
    // Original 2E758 is called BEFORE 29414 tests resource==0.
    std::function<void()> refresh_2e758;
    // Reads at exact 29414 source instruction sites. Underlying
    // DOS sprite header pointer may change during callbacks.
    std::function<std::optional<std::uint16_t>(std::uint32_t)>
        read_sprite_word;
    std::function<std::optional<std::uint8_t>(std::uint32_t)>
        read_sprite_byte;
    // Original full and cropped source rasterizer entry points.
    std::function<void(std::uint32_t,std::uint32_t,std::uint32_t,
                       std::uint32_t)> full_sprite_2a380;
    std::function<void(std::uint32_t,std::uint32_t,std::uint32_t,
                       std::uint32_t,std::uint32_t)> clipped_sprite_2a6b0;
};

class CombatOriginalSpriteClipModel final {
public:
    // Original 29414(mode,row,column,resource,record_pointer).
    // Scratch is SIX adjacent little-endian DWORDs (24 bytes)
    // in original captured DGROUP at the 29804-provided pointer.
    // Returns original EAX 0, 1 or 2. Missing required authentic
    // memory or raster collaborators are rejected explicitly.
    [[nodiscard]] static std::uint32_t draw_29414(
        CombatOriginalSpriteClipGlobals& live_globals,
        CombatOriginalDgroupCapture& original_dgroup,
        std::uint32_t mode,
        std::uint32_t row,
        std::uint32_t column,
        std::uint32_t original_resource_pointer,
        std::uint32_t original_scratch_pointer,
        const CombatOriginalSpriteClipServices& original_services);
};

} // namespace rloft
