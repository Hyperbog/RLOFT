#include "rloft/combat_original_overlay_draw.hpp"

#include <cstdint>
#include <stdexcept>
#include <utility>

namespace rloft {

void CombatOriginalOverlayDrawModel::draw_29804(
    CombatOriginalOverlayDrawState& state,
    std::int32_t mode,
    std::int32_t row,
    std::int32_t column,
    std::uint32_t resource,
    const CombatOriginalOverlayDrawServices& callbacks) {
    if(!callbacks.draw_29414)
        throw std::invalid_argument("missing original 29414");
    // 29804 reads DWORD[EFD4] and multiplies by 24 with x86
    // low-DWORD signed IMUL semantics (modulo 2^32).
    const auto record=static_cast<std::uint32_t>(
        0xEF74U+state.nested_record_depth_efd4*0x18U);
    // 2981F..29824: INC EAX then store EFD4 BEFORE 29414.
    ++state.nested_record_depth_efd4;
    callbacks.draw_29414(mode,row,column,resource,record);
    // 2982E: DEC *LIVE* EFD4, not restore a pre-call snapshot.
    // Nested original draw services may have changed this DWORD.
    --state.nested_record_depth_efd4;
}

CombatOriginalOverlayServices CombatOriginalOverlayDrawModel::bind_1a288(
    CombatOriginalOverlayDrawState& state,
    CombatOriginalOverlayDrawServices draw,
    CombatOriginalOverlayServices overlays) {
    overlays.draw_29804=[&state,draw=std::move(draw)](
        std::int32_t mode,std::int32_t row,
        std::int32_t col,std::uint32_t resource) {
        CombatOriginalOverlayDrawModel::draw_29804(
            state,mode,row,col,resource,draw);
    };
    return overlays;
}

} // namespace rloft
