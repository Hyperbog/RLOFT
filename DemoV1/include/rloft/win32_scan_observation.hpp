#pragma once

#include <cstdint>
#include <optional>

namespace rloft {

// Win32 keyboard-message metadata, not a DOS interrupt or key translation.
// Physical scan bytes are only candidates for an eventual port-60 adapter.
enum class Win32ScanStatus : std::uint8_t {
    SingleUnextended,
    ExtendedSequenceUnresolved,
    RepeatTimingUnresolved,
    InvalidScan,
    InvalidTransition,
    NotKeyboardMessage,
};

struct Win32ScanObservation final {
    Win32ScanStatus status{Win32ScanStatus::NotKeyboardMessage};
    std::uint8_t oem_scan{};
    std::uint16_t repeat_count{};
    bool extended{};
    bool alt_context{};
    bool previous_down{};
    bool key_up{};
    bool system_key{};
    // Populated ONLY when one unextended Win32 message could carry
    // one original set-1 scan byte without guessing an E0/E1 prefix,
    // repetition cadence or scan-to-VK mapping.
    std::optional<std::uint8_t> candidate_port60{};
};

class Win32ScanObservationModel final {
public:
    // Message values are the Win32 SDK's WM_KEYDOWN, WM_KEYUP,
    // WM_SYSKEYDOWN, WM_SYSKEYUP, passed as integers for portable tests.
    // The low 32 bits of LPARAM carry the documented keyboard flags.
    [[nodiscard]] static Win32ScanObservation decode(
        std::uint32_t message, std::uint64_t lparam) noexcept;
};

} // namespace rloft
