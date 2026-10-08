#pragma once
#include <cstdint>
#include <functional>
#include <optional>
#include "rloft/combat_original_clipped_raster.hpp"
#include "rloft/combat_original_sprite_clip.hpp"
namespace rloft {
class CombatOriginalClippedRasterBinder final {
public:
    // Provide original 29414 -> 2A6B0 service, retaining existing
    // refresh, sprite-header reader, and full 2A380 callbacks.
    [[nodiscard]] static CombatOriginalSpriteClipServices bind_29414(
        VgaPlanarMemory& vga,
        const CombatOriginalSpriteVideoState& live_video,
        const CombatOriginalDgroupCapture& live_dgroup,
        CombatOriginalClippedRasterState& original_machine,
        std::function<std::optional<CombatOriginalSpriteMemory>(std::uint32_t)>
            capture_original_sprite,
        CombatOriginalSpriteClipServices existing);
};
} // namespace rloft
