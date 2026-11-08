#pragma once

#include <cstdint>
#include <functional>
#include "rloft/combat_original_composite.hpp"

namespace rloft {

// CODE.2 1A288 and 1AB20 share the SAME DGROUP state as 1A8C8.
// 1A8C8 itself changes 9E37 and 9E39 around both callbacks.
struct CombatOriginalOverlayGlobals final {
    CombatOriginalCompositeState& composite;
    std::int16_t selected_shape_85fe{-1};
    std::uint16_t overlay_mode_a6c0{};
    std::int16_t special_mode_a6bc{};
};

// Original DOS graphics, page, overlay-position and cursor services.
// None of these callbacks may be silently replaced with host pixels.
struct CombatOriginalOverlayServices final {
    std::function<void(std::uint8_t)> push_page_2e590;
    // 1A3B8 accepts pointers to DGROUP 9E31/9E33/9E35 and can
    // change the live position before the backing copy occurs.
    std::function<void(CombatOriginalOverlayGlobals&)> position_1a3b8;
    std::function<void(std::uint32_t,std::int32_t,std::int32_t,
                       std::uint32_t,std::uint32_t)> capture_292bc;
    std::function<std::int32_t(std::int32_t,std::int32_t)> probe_2ae30;
    // 29804(mode,row,column,resource_DGROUP_address)
    std::function<void(std::int32_t,std::int32_t,std::int32_t,
                       std::uint32_t)> draw_29804;
    std::function<void()> pop_page_2e5d4;
    std::function<void(std::uint32_t,std::int32_t,std::int32_t,
                       std::uint32_t,std::uint32_t)> restore_2932c;
};

class CombatOriginalOverlayModel final {
public:
    static void suspend_1a288(
        CombatOriginalOverlayGlobals&,
        const CombatOriginalOverlayServices&);
    static void restore_1ab20(
        CombatOriginalOverlayGlobals&,
        std::int32_t row, std::int32_t column,
        const CombatOriginalOverlayServices&);

    // Preserve existing genuine VGA 2A078 and DGROUP 30CD4 services.
    // The bound state and all collaborator resources must outlive
    // the original 1A8C8 compositor callback.
    [[nodiscard]] static CombatOriginalCompositeServices bind_1a8c8(
        CombatOriginalOverlayGlobals&,
        CombatOriginalOverlayServices,
        CombatOriginalCompositeServices);
};

} // namespace rloft
