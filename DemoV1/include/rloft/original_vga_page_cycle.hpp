#pragma once

#include <cstdint>
#include <functional>
#include <stdexcept>

namespace rloft {

// Original CODE.2 page globals in DGROUP; values are SOURCE state, not
// the native Win32 viewport's current framebuffer or live DOS RAM.
struct OriginalVgaPageCycleState final {
    std::uint32_t draw_page_c050{};
    std::uint8_t selected_page_ef4d{};
    std::uint32_t ground_base_fdbc{};
};

struct OriginalVgaPageCycleCallbacks final {
    // 1EA7E CALL 1A510. Receives the FULL pre-call DWORD[C050].
    // The original x86 toggles the live byte only AFTER this returns.
    std::function<void(std::uint32_t)> present_1a510;
    // 291B6 CALL 29FEC -> 2AB6C. Receives zero-extended low byte
    // of the incoming 291B0 stack argument, before EF4D is stored.
    std::function<void(std::uint32_t)> select_29fec;
};

class OriginalVgaPageCycleModel final {
public:
    // 1F1D6..1F1EB: MOV EAX,[C050]; SHL EAX,0Fh;
    // ADD EAX,0A0000h; MOV [FDBC],EAX. 32-bit wrap is intentional.
    // This is one bounded instruction span within 1F1C8, NOT a
    // simulation of its subsequent texture/overlay draw callbacks.
    static void calculate_fdbc_1f1d6(
        OriginalVgaPageCycleState& original) noexcept {
        original.ground_base_fdbc =
            0xA0000U + (original.draw_page_c050 << 15U);
    }

    // 1EA78..1EA8D: PUSH DWORD[C050], CALL 1A510,
    // XOR BYTE[C050],01h, ADD ESP,4, RET.
    // Do NOT use EF4D as the display argument. Do NOT toggle
    // the whole DWORD or calculate FDBC here (the binary does not).
    static void present_then_flip_1ea78(
        OriginalVgaPageCycleState& original,
        const OriginalVgaPageCycleCallbacks& callbacks) {
        if(!callbacks.present_1a510)
            throw std::invalid_argument(
                "CODE.2 1EA78 requires the original 1A510 callback");
        const auto pushed=original.draw_page_c050;
        callbacks.present_1a510(pushed);
        // A returning callback may have changed the live guest C050.
        // XOR takes effect AFTER callback, against the current low byte.
        original.draw_page_c050 ^= 1U;
    }

    // 291B0..291C7: MOVZX EAX,BYTE[ESP+4], PUSH EAX;
    // CALL 29FEC; ADD ESP,4; MOV AL,BYTE[ESP+4];
    // MOV BYTE[EF4D],AL. No direct read or write of C050.
    static void select_then_store_291b0(
        OriginalVgaPageCycleState& original,
        std::uint32_t argument,
        const OriginalVgaPageCycleCallbacks& callbacks) {
        if(!callbacks.select_29fec)
            throw std::invalid_argument(
                "CODE.2 291B0 requires the original 29FEC callback");
        const auto page=static_cast<std::uint8_t>(argument);
        callbacks.select_29fec(static_cast<std::uint32_t>(page));
        original.selected_page_ef4d=page;
    }
};

} // namespace rloft
