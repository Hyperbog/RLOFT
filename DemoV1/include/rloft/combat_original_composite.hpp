#pragma once

#include <cstdint>
#include <functional>

namespace rloft {

// Original CODE.2 logical 1A8C8..1A9F7; shared compositor,
// not specific to the Level 22 HUD. All globals are live DOS
// state. The injected services must preserve their own original
// effects and may mutate this state before later original reads.
struct CombatOriginalCompositeState final {
    std::uint16_t enabled_85fc{};
    std::int16_t overlay_reference_count_9e37{};
    std::int16_t overlay_left_9e31{};
    std::int16_t overlay_top_9e33{};
    std::uint8_t active_page_9e39{};
    std::uint16_t redraw_busy_a6ba{};
};

struct CombatOriginalCompositeRect final {
    std::uint32_t page{};
    std::uint32_t left{};
    std::uint32_t top{};
    std::uint32_t width{};
    std::uint32_t height{};
};

enum class CombatOriginalCompositeRoute : std::uint8_t {
    DirectWithoutOverlay,
    DirectOutsideOverlay,
    BufferedOverlayOverlap,
};

struct CombatOriginalCompositeServices final {
    // Original 2A078(dest_page, x1,y1,x2,y2,width,height).
    std::function<void(
        std::uint32_t,std::uint32_t,std::uint32_t,
        std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t)>
        copy_rect_2a078;

    std::function<void()> suspend_overlay_1a288;

    // Original 30CD4(dest,source,0x400). These are original DGROUP
    // DWORD addresses beginning at 9E3A, not host pointers.
    std::function<void(std::uint32_t,std::uint32_t,std::uint32_t)>
        move_overlay_bytes_30cd4;

    // Original 1AB20(overlay left, overlay top).
    std::function<void(std::int32_t,std::int32_t)>
        restore_overlay_1ab20;
};

class CombatOriginalCompositeModel final {
public:
    // Uses original inclusive, signed x86 rectangle comparisons
    // and ORIGINAL DWORD overflow for coordinate additions.
    [[nodiscard]] static CombatOriginalCompositeRoute inspect_1a8c8(
        const CombatOriginalCompositeState& state,
        const CombatOriginalCompositeRect& rect) noexcept;

    // Executes only the 1A8C8 wrapper: the four lower-level
    // DOS services remain injected and must not be approximated.
    // Invalid absent service reports unsupported via exception
    // BEFORE the wrapper changes any of its own state.
    [[nodiscard]] static CombatOriginalCompositeRoute run_1a8c8(
        CombatOriginalCompositeState& state,
        const CombatOriginalCompositeRect& rect,
        const CombatOriginalCompositeServices& services);
};

} // namespace rloft
