#pragma once

#include <cstdint>
#include <functional>

namespace rloft {

// Original CODE.2 292BC / 2932C bounds are SIGNED WORDs
// EF3F and EF41, not hardcoded 400x320 Windows viewport dimensions.
struct CombatOriginalOverlayClipLimits final {
    std::int16_t last_row_ef3f{};
    std::int16_t last_column_ef41{};
};

struct CombatOriginalOverlayBackingRequest final {
    std::uint32_t backing_pointer{};
    std::uint32_t row_bits{};
    std::uint32_t column_bits{};
    std::uint32_t row_count_bits{};
    std::uint32_t column_count_bits{};
};

struct CombatOriginalOverlayClippedCall final {
    bool backend_called{};
    CombatOriginalOverlayBackingRequest args{};
};

struct CombatOriginalOverlayBackingServices final {
    // 292BC forwards five DWORD args to 2A114, whereas
    // 2932C forwards them to 2A178. Their VGA read/write
    // backend effects are separate and not invented here.
    std::function<void(const CombatOriginalOverlayBackingRequest&)>
        capture_2a114;
    std::function<void(const CombatOriginalOverlayBackingRequest&)>
        restore_2a178;
};

class CombatOriginalOverlayClipModel final {
public:
    [[nodiscard]] static CombatOriginalOverlayClippedCall
    inspect_292bc(
        const CombatOriginalOverlayClipLimits& original_bounds,
        const CombatOriginalOverlayBackingRequest& original_args) noexcept;

    [[nodiscard]] static CombatOriginalOverlayClippedCall
    inspect_2932c(
        const CombatOriginalOverlayClipLimits& original_bounds,
        const CombatOriginalOverlayBackingRequest& original_args) noexcept;

    [[nodiscard]] static CombatOriginalOverlayClippedCall
    capture_292bc(
        const CombatOriginalOverlayClipLimits& original_bounds,
        const CombatOriginalOverlayBackingRequest& original_args,
        const CombatOriginalOverlayBackingServices& services);

    [[nodiscard]] static CombatOriginalOverlayClippedCall
    restore_2932c(
        const CombatOriginalOverlayClipLimits& original_bounds,
        const CombatOriginalOverlayBackingRequest& original_args,
        const CombatOriginalOverlayBackingServices& services);
};

} // namespace rloft
