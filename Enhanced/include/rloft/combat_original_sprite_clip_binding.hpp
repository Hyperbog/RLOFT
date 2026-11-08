#pragma once

#include "rloft/combat_original_overlay_draw.hpp"
#include "rloft/combat_original_sprite_clip.hpp"

namespace rloft {

// Replace the last opaque 29414 callback in the 29804 EFD4
// scratch-record stack with the source-backed six-DWORD sprite
// clipping implementation. All live captures outlive callbacks.
class CombatOriginalSpriteClipBinder final {
public:
    [[nodiscard]] static CombatOriginalOverlayDrawServices bind_29804(
        CombatOriginalSpriteClipGlobals& live_clip_globals,
        CombatOriginalDgroupCapture& original_dgroup,
        CombatOriginalSpriteClipServices original_raster_services,
        CombatOriginalOverlayDrawServices existing);
};

} // namespace rloft
