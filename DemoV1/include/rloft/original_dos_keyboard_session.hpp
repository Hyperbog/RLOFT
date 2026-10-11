#pragma once

#include "rloft/original_dos_keyboard_irq.hpp"

#include <cstdint>
#include <functional>
#include <vector>

namespace rloft {

// Composition seam for original CODE.2 keyboard globals and services.
// Accepts raw, ALREADY-DECODED port-60 bytes; never interprets a Win32
// virtual key, WM_KEYDOWN lParam, E0/E1 prefix, or host keyboard layout.
// This is not wired into normal game input until the DOS IRQ bridge is proven.
class OriginalDosKeyboardSession final {
public:
    [[nodiscard]] static OriginalDosKeyboardSession from_installed_code2(
        const std::vector<std::uint8_t>& code2_file);

    // Original 30950 after the keyboard-controller I/O prelude.
    [[nodiscard]] bool receive_port60(std::uint8_t raw_scan);

    [[nodiscard]] bool held(std::uint8_t scan) const;
    [[nodiscard]] std::uint32_t modifiers(std::uint32_t mask) const;
    [[nodiscard]] std::uint8_t consume_space();

    // Original 30AD8, 30B40, 30AEC each have DISTINCT 2F868
    // invocation and F572 consumption rules. The supplied service
    // may synchronously call receive_port60 while drawing a frame.
    [[nodiscard]] bool input_ready(const std::function<void()>& draw_2f868);
    [[nodiscard]] std::uint32_t peek_key(
        const std::function<void()>& draw_2f868);
    [[nodiscard]] std::uint32_t consume_key(
        const std::function<void()>& draw_2f868);

    [[nodiscard]] const OriginalDosKeyboardIrqState& state() const noexcept;

private:
    explicit OriginalDosKeyboardSession(OriginalDosInputState initial);
    OriginalDosKeyboardIrqState state_{};
};

} // namespace rloft
