#pragma once

#include "rloft/combat_original_composite.hpp"
#include "rloft/combat_original_vga_copy.hpp"

namespace rloft {

// Supplies original 1A8C8's 2A078 callback using the verified VGA
// write-mode-1 copy and the LIVE original EFE9/F001/F005/EFF9
// video state. The state and VGA memory must outlive the callback.
class CombatOriginalVgaCopyBinder final {
public:
    [[nodiscard]] static CombatOriginalCompositeServices bind_direct_copy(
        VgaPlanarMemory& original_vga_planes,
        CombatOriginalVgaCopyState& original_video_state);
};

} // namespace rloft
