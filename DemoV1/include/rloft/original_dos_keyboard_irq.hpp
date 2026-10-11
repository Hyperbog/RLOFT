#pragma once

#include "rloft/original_dos_feedback_runtime.hpp"

#include <array>
#include <cstdint>

namespace rloft {

// Runtime observations for original CODE.2 30748/307F0/308BC and
// the keyboard interrupt dispatch at 30950. This is original DOS
// scan-byte state, not Windows virtual-key or character input.
struct OriginalDosKeyboardIrqState final {
    OriginalDosInputState input{};  // F572 and original F590 translations
    std::array<std::uint8_t,256> held_2b6c4{};
    std::uint8_t count_f588{};
    std::uint8_t last_f571{};
    std::uint8_t modifiers_f573{};
    std::uint8_t space_f574{};
    std::uint8_t alt_q_f0b1{};
    std::uint16_t direction_258fc{};
    std::uint16_t step_258fe{};
    std::uint16_t break_flag_1457c{};
};

class OriginalDosKeyboardIrqModel final {
public:
    static void release_30748(OriginalDosKeyboardIrqState& state,
                              std::uint8_t scan);
    static void press_307f0(OriginalDosKeyboardIrqState& state,
                            std::uint8_t scan);
    [[nodiscard]] static bool held_308bc(
        const OriginalDosKeyboardIrqState& state, std::uint8_t scan);

    // CODE.2 30ABC masks the current F573 modifier byte by a DWORD mask.
    [[nodiscard]] static std::uint32_t modifiers_30abc(
        const OriginalDosKeyboardIrqState& state, std::uint32_t mask);

    // CODE.2 30AC8 returns unsigned BYTE F574 and clears that byte.
    [[nodiscard]] static std::uint8_t consume_space_30ac8(
        OriginalDosKeyboardIrqState& state);

    // Models source 30950..30AB9 AFTER the DOS ISR has read raw port
    // 0x60 and saved EBX. No Windows scan conversion or PIC/EOI effects.
    // Returns whether the original source takes a special early IRET
    // after the interrupt acknowledge (F572==43h or 19h).
    [[nodiscard]] static bool dispatch_30950(
        OriginalDosKeyboardIrqState& state, std::uint8_t raw_port60);
};

} // namespace rloft
