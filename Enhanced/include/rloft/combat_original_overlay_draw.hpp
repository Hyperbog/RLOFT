#pragma once

#include <cstdint>
#include <functional>

#include "rloft/combat_original_overlay.hpp"

namespace rloft {

// CODE.2 29804 is NOT a direct sprite blit: it manages an
// EFD4-indexed original 24-byte drawing record at EF74,
// then invokes 29414 with the original four call arguments.
struct CombatOriginalOverlayDrawState final {
    std::uint32_t nested_record_depth_efd4{};
};

struct CombatOriginalOverlayDrawServices final {
    std::function<void(std::int32_t,std::int32_t,std::int32_t,
                       std::uint32_t,std::uint32_t)> draw_29414;
};

class CombatOriginalOverlayDrawModel final {
public:
    static void draw_29804(
        CombatOriginalOverlayDrawState& live_state,
        std::int32_t mode,
        std::int32_t row,
        std::int32_t column,
        std::uint32_t resource_dgroup_pointer,
        const CombatOriginalOverlayDrawServices& original_services);

    [[nodiscard]] static CombatOriginalOverlayServices bind_1a288(
        CombatOriginalOverlayDrawState& live_draw_state,
        CombatOriginalOverlayDrawServices original_services,
        CombatOriginalOverlayServices remaining_overlay);
};

} // namespace rloft
