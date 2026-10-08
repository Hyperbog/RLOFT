#pragma once

#include "rloft/combat_original_overlay.hpp"
#include "rloft/combat_original_vga_page_stack.hpp"

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
};

} // namespace rloft
