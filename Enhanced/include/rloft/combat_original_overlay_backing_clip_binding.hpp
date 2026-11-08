#pragma once

#include "rloft/combat_original_overlay.hpp"
#include "rloft/combat_original_overlay_backing_clip.hpp"

namespace rloft {

// 1A288/1AB20 already provide genuine 292BC/2932C arguments.
// Bind their next original clip layer against live EF3F/EF41
// while retaining all other overlay sprite/page collaborators.
class CombatOriginalOverlayBackingClipBinder final {
public:
    [[nodiscard]] static CombatOriginalOverlayServices bind(
        CombatOriginalOverlayClipLimits& live_clip_globals,
        CombatOriginalOverlayBackingServices real_backing_services,
        CombatOriginalOverlayServices remaining_overlay_services);
};

} // namespace rloft
