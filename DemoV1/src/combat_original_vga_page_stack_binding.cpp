#include "rloft/combat_original_vga_page_stack_binding.hpp"

#include <cstdint>
#include <utility>

namespace rloft {

CombatOriginalOverlayServices CombatOriginalVgaPageStackOverlayBinder::bind(
    CombatOriginalVgaPageStackState& page_stack,
    CombatOriginalVgaPageStackServices page_callbacks,
    CombatOriginalOverlayServices overlays) {
    overlays.push_page_2e590=[
        &page_stack,page_callbacks
    ](std::uint8_t requested_page) {
        CombatOriginalVgaPageStackModel::push_2e590(
            page_stack,requested_page,page_callbacks);
    };
    overlays.pop_page_2e5d4=[
        &page_stack,page_callbacks
    ]() {
        CombatOriginalVgaPageStackModel::pop_2e5d4(
            page_stack,page_callbacks);
    };
    return overlays;
}

} // namespace rloft
