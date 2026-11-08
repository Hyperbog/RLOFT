#pragma once

#include "rloft/combat_original_vga_backing.hpp"

namespace rloft {

// Attach the two actual VGA byte-moving routines beneath the
// existing 292BC/2932C clip wrappers, retaining live VGA page
// selectors and the caller-owned original DGROUP backing bytes.
class CombatOriginalVgaBackingBinder final {
public:
    [[nodiscard]] static CombatOriginalOverlayBackingServices bind(
        VgaPlanarMemory& original_vga,
        CombatOriginalDgroupCapture& original_dgroup,
        CombatOriginalVgaCopyState& live_video);
};

} // namespace rloft
