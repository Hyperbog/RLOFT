#pragma once

#include "rloft/combat_original_composite.hpp"
#include "rloft/combat_original_dgroup_move.hpp"
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

    // Adds original 30CD4 backing-byte copy to the real 2A078
    // callback. 1A288/1AB20 overlay draw services remain required
    // for the buffered branch, and are NOT simulated here.
    [[nodiscard]] static CombatOriginalCompositeServices
    bind_copy_and_backing(
        VgaPlanarMemory& original_vga_planes,
        CombatOriginalVgaCopyState& original_video_state,
        CombatOriginalDgroupCapture& original_dgroup);
};

} // namespace rloft
