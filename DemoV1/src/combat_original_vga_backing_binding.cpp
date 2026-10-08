#include "rloft/combat_original_vga_backing_binding.hpp"

#include <stdexcept>

namespace rloft {

CombatOriginalOverlayBackingServices CombatOriginalVgaBackingBinder::bind(
    VgaPlanarMemory& vga,
    CombatOriginalDgroupCapture& dgroup,
    CombatOriginalVgaCopyState& video) {
    CombatOriginalOverlayBackingServices out{};
    out.capture_2a114=[&vga,&dgroup,&video](
        const CombatOriginalOverlayBackingRequest& rect) {
        if(!CombatOriginalVgaBackingModel::capture_2a114(
                vga,dgroup,video,rect))
            throw std::logic_error(
                "uncaptured original DOS 2A114 VGA backing read");
    };
    out.restore_2a178=[&vga,&dgroup,&video](
        const CombatOriginalOverlayBackingRequest& rect) {
        if(!CombatOriginalVgaBackingModel::restore_2a178(
                vga,dgroup,video,rect))
            throw std::logic_error(
                "uncaptured original DOS 2A178 VGA backing write");
    };
    return out;
}

} // namespace rloft
