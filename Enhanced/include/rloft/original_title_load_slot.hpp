#pragma once

#include <cstdint>

namespace rloft {

// CODE.2 SHA-256 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314
// Logical 07BB9..07C18 in the supplied 558,689-byte CODE.2,
// physical 0x3AF24+logical. Original selector 07B94..07D42.
// This is a SOURCE-VERIFIED hit-test only. It neither opens SAVE.DAT,
// restores WORK/LEVnnn.DAT, nor changes the normal menu implementation.
enum class OriginalTitleLoadHitKind : std::uint8_t {
    Outside = 0,
    SelectedSlot = 1,
    RightClickExit = 2, // CODE.2 07BDA returns status 2 before XY tests.
};

struct OriginalTitleLoadHit {
    OriginalTitleLoadHitKind kind{OriginalTitleLoadHitKind::Outside};
    std::uint8_t slot{0}; // 1..10 ONLY when kind==SelectedSlot.
};

// 07BB9 reads 9E35 as WORD but clears DH, masks DL with 2 and returns
// status 2 immediately on right click. 07BC5 and 07BCF sign-extend the
// mouse X and Y WORDs, respectively, into 32-bit coordinates.
//
// 07BE2/07BE7: 21 <= x < 379.
// 07BEF/07BF4: 20 <= y < 92.
// 07BF9..07C18: (x-21)/37, signed division, remainder <=25.
// All in-range x values have non-negative dividend, so the DOS signed
// quotient and C++ integer quotient coincide. The final column ends
// at x=378, not 379; do not create a symmetric invented 26px hitbox.
[[nodiscard]] constexpr OriginalTitleLoadHit
original_title_load_hit(std::int16_t mouse_x, std::int16_t mouse_y,
                        std::uint16_t mouse_buttons) noexcept {
    if ((mouse_buttons & 0x0002U) != 0U) {
        return {OriginalTitleLoadHitKind::RightClickExit, 0};
    }
    const auto x=static_cast<std::int32_t>(mouse_x);
    const auto y=static_cast<std::int32_t>(mouse_y);
    if (x < 21 || x >= 379 || y < 20 || y >= 92) {
        return {};
    }
    const auto offset=x-21;
    if (offset % 37 > 25) {
        return {};
    }
    const auto slot=(offset/37)+1;
    return {OriginalTitleLoadHitKind::SelectedSlot,
            static_cast<std::uint8_t>(slot)};
}

} // namespace rloft
