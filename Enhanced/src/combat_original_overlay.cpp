#include "rloft/combat_original_overlay.hpp"

#include <cstdint>
#include <stdexcept>
#include <utility>

namespace rloft {
namespace {
std::int16_t add_word(std::int16_t value, unsigned delta) noexcept {
    const auto bits=static_cast<std::uint16_t>(value);
    const auto next=static_cast<std::uint16_t>(bits+delta);
    const auto as_i32=next<=0x7FFFU ? static_cast<std::int32_t>(next)
        : static_cast<std::int32_t>(next)-0x10000;
    return static_cast<std::int16_t>(as_i32);
}
std::int32_t sx(std::int16_t x) noexcept {
    return static_cast<std::int32_t>(x);
}
std::uint32_t backing(std::uint8_t page) noexcept {
    return 0x9E3AU+(static_cast<std::uint32_t>(page)<<10U);
}
} // namespace

void CombatOriginalOverlayModel::suspend_1a288(
    CombatOriginalOverlayGlobals& g,
    const CombatOriginalOverlayServices& s) {
    auto& x=g.composite;
    // 1A288 -> 1AC0B: disabled overlay is a complete no-op.
    if(x.enabled_85fc==0U) return;
    // 1A296/1A29E jumps to shared 1AC04 for nonzero depth.
    if(x.overlay_reference_count_9e37!=0) {
        x.overlay_reference_count_9e37=add_word(
            x.overlay_reference_count_9e37,1U);
        return;
    }
    if(!s.push_page_2e590 || !s.position_1a3b8 ||
       !s.capture_292bc || !s.probe_2ae30 || !s.pop_page_2e5d4)
        throw std::invalid_argument(
            "missing original 1A288 first-activation service");

    s.push_page_2e590(x.active_page_9e39);
    s.position_1a3b8(g);
    // 292BC accepts the original 9E3A+(BYTE[9E39]<<10)
    // DGROUP buffer address and the LIVE signed row/column.
    s.capture_292bc(backing(x.active_page_9e39),
                    sx(x.overlay_row_9e31),sx(x.overlay_column_9e33),
                    0x20U,0x10U);
    const auto classification=s.probe_2ae30(
        sx(x.overlay_row_9e31),sx(x.overlay_column_9e33));

    // 1A30F tests A6C0 AFTER the 2AE30 callback, which may
    // have changed the original globals before this branch.
    if(g.overlay_mode_a6c0!=0U) {
        if(!s.draw_29804)
            throw std::invalid_argument(
                "missing original 29804 overlay drawing service");
        if(classification==2) {
            s.draw_29804(1,sx(x.overlay_row_9e31),
                         sx(x.overlay_column_9e33),0x880DU);
        } else if(classification==1) {
            s.draw_29804(2,sx(x.overlay_row_9e31),
                         sx(x.overlay_column_9e33),0x880DU);
        } else if(g.selected_shape_85fe!=-1) {
            s.draw_29804(0,sx(x.overlay_row_9e31),
                         sx(x.overlay_column_9e33),0x880DU);
            // Second 29804 re-reads live coords, not cached ones.
            s.draw_29804(0,sx(x.overlay_row_9e31),
                         sx(x.overlay_column_9e33),0x8603U);
        } else {
            const auto mode=g.special_mode_a6bc>=3
                ? sx(g.special_mode_a6bc) : 0;
            s.draw_29804(mode,sx(x.overlay_row_9e31),
                         sx(x.overlay_column_9e33),0x880DU);
        }
    }

    // All 1A288 first-activation branches join at 1ABFF
    // (2E5D4), then 1AC04 INC WORD[9E37].
    s.pop_page_2e5d4();
    x.overlay_reference_count_9e37=add_word(
        x.overlay_reference_count_9e37,1U);
}

void CombatOriginalOverlayModel::restore_1ab20(
    CombatOriginalOverlayGlobals& g,
    std::int32_t row,std::int32_t column,
    const CombatOriginalOverlayServices& s) {
    auto& x=g.composite;
    if(x.enabled_85fc==0U) return; // 1AB28 -> 1AAC9
    x.overlay_reference_count_9e37=add_word(
        x.overlay_reference_count_9e37,0xFFFFU); // DEC WORD
    if(x.overlay_reference_count_9e37!=0) return;
    if(!s.push_page_2e590 || !s.restore_2932c || !s.pop_page_2e5d4)
        throw std::invalid_argument(
            "missing original 1AB20 restore service");
    s.push_page_2e590(x.active_page_9e39);
    s.restore_2932c(backing(x.active_page_9e39),
                    row,column,0x20U,0x10U);
    // 1AB67 tail JMP: this routine does NOT increment 9E37.
    s.pop_page_2e5d4();
}

CombatOriginalCompositeServices CombatOriginalOverlayModel::bind_1a8c8(
    CombatOriginalOverlayGlobals& g,
    CombatOriginalOverlayServices s,
    CombatOriginalCompositeServices underlying) {
    underlying.suspend_overlay_1a288=[&g,s]() {
        CombatOriginalOverlayModel::suspend_1a288(g,s);
    };
    underlying.restore_overlay_1ab20=[&g,s](
        std::int32_t row,std::int32_t column) {
        CombatOriginalOverlayModel::restore_1ab20(g,row,column,s);
    };
    return underlying;
}

} // namespace rloft
