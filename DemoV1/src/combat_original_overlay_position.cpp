#include "rloft/combat_original_overlay_position.hpp"

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace rloft {
namespace {
std::int32_t widen(std::int16_t value) noexcept {
    return static_cast<std::int32_t>(value);
}
} // namespace

CombatOriginalOverlayPositionResult
CombatOriginalOverlayPositionModel::run_1a3b8(
    CombatOriginalOverlayPositionState& s,
    const CombatOriginalOverlayPositionServices& calls) {
    bool changed=false; // original EBX starts at zero
    if(s.disabled_d680!=0U) {
        // 1A4CA..1A4DC: disable both WORD modes and call 1F820.
        // It can mutate the live A6B4/A6B6/A6B8 before return.
        if(!calls.disabled_side_effect_1f820)
            throw std::invalid_argument("missing source 1F820");
        s.clamp_mode_a6be=0U;
        s.constrain_mode_a6bc=0U;
        calls.disabled_side_effect_1f820();
    } else {
        if(s.clamp_mode_a6be!=0U) {
            if(!calls.pointer_move_33c90)
                throw std::invalid_argument("missing source 33C90");
            // 1A3D7..1A428: the first clamp STORES the signed
            // WORD globals before calling the mouse service.
            s.row_a6b4=static_cast<std::int16_t>(
                std::clamp(widen(s.row_a6b4),0x124,0x187));
            s.column_a6b6=static_cast<std::int16_t>(
                std::clamp(widen(s.column_a6b6),0x7e,0xc3));
            changed=true; // 1A43A sets EBX=1 before CALL
            calls.pointer_move_33c90(widen(s.row_a6b4),
                                      widen(s.column_a6b6));
        }
        // 1A447 rereads this WORD *after* first 33C90 callback.
        if(s.constrain_mode_a6bc!=0U) {
            const auto target_row=static_cast<std::int16_t>(
                std::clamp(widen(s.row_a6b4),4,0x103));
            const auto target_col=static_cast<std::int16_t>(
                std::clamp(widen(s.column_a6b6),0x11,0x11f));
            if(s.row_a6b4!=target_row ||
               s.column_a6b6!=target_col) {
                if(!calls.pointer_move_33c90)
                    throw std::invalid_argument("missing source 33C90");
                calls.pointer_move_33c90(
                    widen(target_row),widen(target_col));
                // 1A4B5/1A4BC WRITE targets AFTER callback returns,
                // overriding any changes made by 33C90 there.
                s.row_a6b4=target_row;
                s.column_a6b6=target_col;
            }
            changed=true; // 1A4C3 sets EBX=1 even without MOVe
        }
    }
    // 1A4E1..1A508 writes 16-bit results into the three supplied
    // pointers, after all nested original callback side effects.
    return {changed,s.row_a6b4,s.column_a6b6,s.third_word_a6b8};
}

CombatOriginalOverlayServices
CombatOriginalOverlayPositionModel::bind_1a288(
    CombatOriginalOverlayPositionState& mouse,
    CombatOriginalOverlayPositionServices calls,
    CombatOriginalOverlayServices overlays) {
    overlays.position_1a3b8=[&mouse,calls=std::move(calls)](
        CombatOriginalOverlayGlobals& overlay) {
        const auto projection=run_1a3b8(mouse,calls);
        overlay.composite.overlay_row_9e31=projection.output_row;
        overlay.composite.overlay_column_9e33=projection.output_column;
        // Position result A6B8 is the supplied original 9E35 word.
        // No additional meaning/geometry is invented for it.
    };
    return overlays;
}

} // namespace rloft
