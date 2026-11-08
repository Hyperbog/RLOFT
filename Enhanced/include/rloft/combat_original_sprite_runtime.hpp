#pragma once

#include <cstdint>
#include <functional>

namespace rloft {

// Original CODE.2 2939C (normal), 293EC (shaded) and 2E758 (refresh).
// Pointer F0C8 is a mutable original callback GLOBAL. The primitive
// can change F0C8 before the 2E758 tail call observes it.
struct CombatOriginalSpriteRefreshState final {
    std::uint32_t refresh_callback_pointer_f0c8{};
};

struct CombatOriginalSpriteServices final {
    std::function<void(std::int32_t,std::uint32_t,
                       std::uint32_t,std::uint32_t)>
        raster_2a380;
    std::function<void(std::int32_t,std::uint32_t,
                       std::uint32_t,std::uint32_t,std::uint32_t)>
        raster_shaded_2a584;
    std::function<void()> invoke_live_f0c8;
};

class CombatOriginalSpriteRuntimeModel final {
public:
    static void draw_normal_2939c(
        CombatOriginalSpriteRefreshState& state,
        std::int32_t original_frame,
        std::uint32_t original_row,
        std::uint32_t original_column,
        std::uint32_t original_resource_pointer,
        const CombatOriginalSpriteServices& services);

    static void draw_shaded_293ec(
        CombatOriginalSpriteRefreshState& state,
        std::int32_t original_frame,
        std::uint32_t original_row,
        std::uint32_t original_column,
        std::uint32_t original_resource_pointer,
        std::uint32_t original_shade,
        const CombatOriginalSpriteServices& services);
};

} // namespace rloft
