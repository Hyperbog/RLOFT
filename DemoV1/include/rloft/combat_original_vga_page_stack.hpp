#pragma once

#include <cstdint>
#include <functional>
#include <vector>

namespace rloft {

// Captured original CODE.2 DGROUP global state, NOT an assumed
// Windows framebuffer page stack. F0D0/EF4D/2B644 are byte-
// addressed and need not match the independent EFE9/F001 page
// selection used by 2AB6C.
struct CombatOriginalVgaPageStackState final {
    std::uint8_t depth_f0d0{};
    std::uint8_t active_page_ef4d{};
    std::uint32_t selected_page_efe9{};
    std::uint32_t physical_page_base_f001{};
    std::uint32_t page_stride_f005{};
    // Explicit captured bytes at DGROUP:2B644. Original access
    // indexes using zero-extended BYTE F0D0, up to 0xFF.
    std::vector<std::uint8_t> saved_pages_2b644;
};

struct CombatOriginalVgaPageStackServices final {
    // Original 2F580 diagnostic callback at overflow depth 0x0A
    // or underflow depth 0. An actual DOS handler may terminate;
    // if this callback returns, original code CONTINUES.
    std::function<void(std::uint32_t)> diagnostic_2f580;
    // Observer invoked after the original 291B0/29FEC/2AB6C
    // state mutation, never as a substitute for that mutation.
    std::function<void(std::uint8_t)> selected_page_291b0;
    // Optional live 2AB6C DWORD[F005] read. An original diagnostic
    // may change F005 before 291B0 is invoked; callers may provide
    // the authoritative video-state provider instead of an old copy.
    std::function<std::uint32_t()> observed_stride_f005_before_291b0;
};

class CombatOriginalVgaPageStackModel final {
public:
    // CODE.2 2E590: conditional overflow diagnostic, store
    // EF4D to 2B644[BYTE F0D0], increment depth MOD 256,
    // then select caller BYTE page through original 291B0.
    static void push_2e590(
        CombatOriginalVgaPageStackState& original,
        std::uint32_t original_page_argument,
        const CombatOriginalVgaPageStackServices& services);

    // CODE.2 2E5D4: conditional underflow diagnostic,
    // decrement depth MOD 256, reload saved BYTE page,
    // and select it through the same original 291B0.
    static void pop_2e5d4(
        CombatOriginalVgaPageStackState& original,
        const CombatOriginalVgaPageStackServices& services);
};

} // namespace rloft
