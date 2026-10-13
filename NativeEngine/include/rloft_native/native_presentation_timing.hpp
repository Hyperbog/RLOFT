#pragma once
#include <cstdint>

namespace rloft_native {
// The original BIOS PIT tick is approximately 54.925 ms. CODE.1's
// authored 70-frame introduction was being advanced at 14 ms per frame
// (roughly one second for the complete sequence). Pace host presentation
// conservatively to one PIT-like tick per frame until the exact CODE.1
// live animation schedule is independently reconstructed. This is NOT
// a source-certified frame-for-frame DOS animation timing claim.
inline constexpr std::uint64_t kNativeLegacyAnimationFrameMs=55U;
inline constexpr std::uint64_t kNativeWin32PollIntervalMs=16U;

[[nodiscard]] inline constexpr bool native_presentation_frame_due(
    std::uint64_t now_ms,
    std::uint64_t frame_start_ms,
    std::uint64_t duration_ms) noexcept {
    // Monotonic clock epochs should not move backward. If an upstream
    // provider does, do not underflow and fast-forward the animation.
    return now_ms>=frame_start_ms &&
           now_ms-frame_start_ms>=duration_ms;
}
} // namespace rloft_native
