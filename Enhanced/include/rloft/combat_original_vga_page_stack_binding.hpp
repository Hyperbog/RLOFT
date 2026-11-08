#pragma once

#include "rloft/combat_original_overlay.hpp"
#include "rloft/combat_original_vga_page_stack.hpp"
#include "rloft/combat_original_vga_copy.hpp"

namespace rloft {

// Attach true 2E590/2E5D4 state to 1A288/1AB20 callbacks,
// preserving all other source-backed overlay collaborators.
// F0D0/EF4D/2B644 are separate from 9E37/9E39 overlay state.
class CombatOriginalVgaPageStackOverlayBinder final {
public:
    [[nodiscard]] static CombatOriginalOverlayServices bind(
        CombatOriginalVgaPageStackState& page_stack_state,
        CombatOriginalVgaPageStackServices page_services,
        CombatOriginalOverlayServices remaining_overlay_services);

    // Prefer for fully connected VGA copy flows: original
    // 291B0 must update the SAME EFE9/F001 registers consumed
    // by the 2A078 source/destination page calculation.
    // F005 is observed from live_video at each 291B0 call.
    [[nodiscard]] static CombatOriginalOverlayServices
    bind_with_live_video(
        CombatOriginalVgaPageStackState& page_stack_state,
        CombatOriginalVgaCopyState& live_video,
        CombatOriginalVgaPageStackServices page_services,
        CombatOriginalOverlayServices remaining_overlay_services);
};

} // namespace rloft
