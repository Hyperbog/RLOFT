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

CombatOriginalOverlayServices
CombatOriginalVgaPageStackOverlayBinder::bind_with_live_video(
    CombatOriginalVgaPageStackState& stack,
    CombatOriginalVgaCopyState& live_video,
    CombatOriginalVgaPageStackServices page_services,
    CombatOriginalOverlayServices overlay_services) {
    // Preserve independent F0D0/EF4D bookkeeping, but ensure
    // 2AB6C reads the SAME live F005 used by the 2A078 copy.
    // This read is invoked after any original 2F580 diagnostic
    // callback, immediately before the 291B0 projection.
    page_services.observed_stride_f005_before_291b0=[
        &live_video]() { return live_video.page_stride_f005; };

    const auto original_observer=page_services.selected_page_291b0;
    page_services.selected_page_291b0=[
        &stack,&live_video,original_observer
    ](std::uint8_t page) {
        // 291B0 first writes original EFE9/F001 through 2AB6C
        // and then original EF4D, before later callbacks.
        live_video.selected_page_efe9=stack.selected_page_efe9;
        live_video.linear_page_base_f001=
            stack.physical_page_base_f001;
        if(original_observer) original_observer(page);
    };
    return bind(stack,std::move(page_services),
                std::move(overlay_services));
}

} // namespace rloft
