#include "rloft/combat_original_composite.hpp"

#include <cstdint>
#include <stdexcept>
#include <utility>

namespace rloft {
namespace {
// Signed x86 CMP/JL/JG, with no implementation-defined narrowing.
std::int64_t signed_dword(std::uint32_t bits) noexcept {
    return bits<=0x7FFFFFFFU
        ? static_cast<std::int64_t>(bits)
        : static_cast<std::int64_t>(bits)-0x100000000LL;
}

std::int16_t word_dec(std::int16_t in) noexcept {
    const auto bits=static_cast<std::uint16_t>(in);
    const auto val=static_cast<std::uint16_t>(bits-1U);
    return val<=0x7FFFU ? static_cast<std::int16_t>(val)
                        : static_cast<std::int16_t>(
                            static_cast<std::int32_t>(val)-0x10000);
}
std::int16_t word_inc(std::int16_t in) noexcept {
    const auto bits=static_cast<std::uint16_t>(in);
    const auto val=static_cast<std::uint16_t>(bits+1U);
    return val<=0x7FFFU ? static_cast<std::int16_t>(val)
                        : static_cast<std::int16_t>(
                            static_cast<std::int32_t>(val)-0x10000);
}
} // namespace

CombatOriginalCompositeRoute CombatOriginalCompositeModel::inspect_1a8c8(
    const CombatOriginalCompositeState& state,
    const CombatOriginalCompositeRect& rect) noexcept {
    // 1A8D7..1A8F5: overlay must be enabled and its signed
    // WORD reference count must be at least 1.
    if(state.enabled_85fc==0U ||
       state.overlay_reference_count_9e37<1)
        return CombatOriginalCompositeRoute::DirectWithoutOverlay;

    // 1A8FC..1A92C: OVERLAP compares signed DWORD coordinates,
    // inclusively, against a 32x16 overlay. x+width and y+height
    // use 32-bit wrap, and original signed conditional branches.
    const auto overlay_right=static_cast<std::uint32_t>(
        static_cast<std::int32_t>(state.overlay_row_9e31)+0x20);
    const auto overlay_bottom=static_cast<std::uint32_t>(
        static_cast<std::int32_t>(state.overlay_column_9e33)+0x10);
    const auto overlay_row=static_cast<std::int64_t>(
        state.overlay_row_9e31);
    const auto overlay_column=static_cast<std::int64_t>(
        state.overlay_column_9e33);
    if(signed_dword(overlay_right)<signed_dword(rect.row) ||
       overlay_row>signed_dword(rect.row+rect.rows) ||
       signed_dword(overlay_bottom)<signed_dword(rect.column) ||
       overlay_column>signed_dword(rect.column+rect.columns))
        return CombatOriginalCompositeRoute::DirectOutsideOverlay;

    return CombatOriginalCompositeRoute::BufferedOverlayOverlap;
}

CombatOriginalCompositeRoute CombatOriginalCompositeModel::run_1a8c8(
    CombatOriginalCompositeState& state,
    const CombatOriginalCompositeRect& rect,
    const CombatOriginalCompositeServices& service) {
    const auto route=inspect_1a8c8(state,rect);
    if(!service.copy_rect_2a078)
        throw std::invalid_argument("original 1A8C8 requires 2A078");
    if(route==CombatOriginalCompositeRoute::BufferedOverlayOverlap &&
       (!service.suspend_overlay_1a288 ||
        !service.move_overlay_bytes_30cd4 ||
        !service.restore_overlay_1ab20))
        throw std::invalid_argument(
            "original 1A8C8 overlap needs 1A288,30CD4,1AB20");

    // 1A8A3..1A8B2: original 2A078 uses the page argument with
    // its LOW BIT flipped, but keeps its remaining 31 bits.
    const auto copy=[&](){
        service.copy_rect_2a078(
            rect.page^1U,rect.row,rect.column,
            rect.row,rect.column,rect.rows,rect.columns);
    };

    // 1A9D5 path never touches A6BA, even if it was nonzero
    // on entry. Don't perform synthetic enter/exit state writes.
    if(route==CombatOriginalCompositeRoute::DirectWithoutOverlay) {
        copy();
        return route;
    }

    // 1A8FF: when the overlay gate succeeds, A6BA becomes one
    // on BOTH direct-outside and buffered-overlap branches.
    state.redraw_busy_a6ba=1U;
    if(route==CombatOriginalCompositeRoute::DirectOutsideOverlay) {
        copy();
    } else {
        // 1A94B..1A965: saved BYTE 9E39, decrement signed WORD
        // 9E37, set active BYTE 9E39 from low argument byte,
        // then invoke 1A288. The latter can mutate live globals.
        const auto old_page=state.active_page_9e39;
        const auto saved_row=state.overlay_row_9e31;
        const auto saved_column=state.overlay_column_9e33;
        state.overlay_reference_count_9e37=word_dec(
            state.overlay_reference_count_9e37);
        state.active_page_9e39=static_cast<std::uint8_t>(rect.page);
        service.suspend_overlay_1a288();

        // 1A96A..1A9A8: copy rectangle, then memmove 0x400
        // bytes from 9E3A + page*400 to 9E3A + (page^1)*400.
        // Both address calculations wrap as original DWORD SHL.
        copy();
        const auto src=static_cast<std::uint32_t>(
            0x9E3AU+(rect.page<<10U));
        const auto dst=static_cast<std::uint32_t>(
            0x9E3AU+((rect.page^1U)<<10U));
        service.move_overlay_bytes_30cd4(dst,src,0x400U);

        // 1A9AB..1A9BF: call 1AB20 with the saved overlay
        // POSITION from entry, then increment live WORD 9E37
        // and restore the original mode BYTE.
        service.restore_overlay_1ab20(saved_row,saved_column);
        state.overlay_reference_count_9e37=word_inc(
            state.overlay_reference_count_9e37);
        state.active_page_9e39=old_page;
    }

    // 1A9C4: normal return clears A6BA on both gated paths.
    state.redraw_busy_a6ba=0U;
    return route;
}

std::function<void(
    std::uint32_t,std::uint32_t,std::uint32_t,
    std::uint32_t,std::uint32_t)>
CombatOriginalCompositeModel::bind_irq_display_1a8c8(
    CombatOriginalCompositeState& original_state,
    CombatOriginalCompositeServices original_services) {
    return [&original_state,
            services=std::move(original_services)](
        std::uint32_t page,std::uint32_t left,std::uint32_t top,
        std::uint32_t width,std::uint32_t height) {
        const CombatOriginalCompositeRect rect{
            page,left,top,width,height};
        (void)CombatOriginalCompositeModel::run_1a8c8(
            original_state,rect,services);
    };
}

} // namespace rloft
