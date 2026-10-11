#include "rloft/win32_scan_observation.hpp"

#include <cstdint>

namespace rloft {

Win32ScanObservation Win32ScanObservationModel::decode(
    std::uint32_t message, std::uint64_t lparam) noexcept {
    constexpr std::uint32_t kDown=0x0100U;
    constexpr std::uint32_t kUp=0x0101U;
    constexpr std::uint32_t kSystemDown=0x0104U;
    constexpr std::uint32_t kSystemUp=0x0105U;
    Win32ScanObservation result{};
    if(message!=kDown && message!=kUp &&
       message!=kSystemDown && message!=kSystemUp) return result;
    const auto flags=static_cast<std::uint32_t>(lparam);
    result.key_up=(message==kUp || message==kSystemUp);
    result.system_key=(message==kSystemDown || message==kSystemUp);
    result.oem_scan=static_cast<std::uint8_t>((flags>>16U)&0xFFU);
    result.repeat_count=static_cast<std::uint16_t>(flags&0xFFFFU);
    result.extended=(flags&(1U<<24U))!=0U;
    result.alt_context=(flags&(1U<<29U))!=0U;
    result.previous_down=(flags&(1U<<30U))!=0U;
    const bool transition_up=(flags&(1U<<31U))!=0U;
    if(transition_up!=result.key_up ||
       (result.key_up && !result.previous_down)) {
        result.status=Win32ScanStatus::InvalidTransition;
        return result;
    }
    if(result.oem_scan==0U || result.oem_scan>0x7FU ||
       result.repeat_count==0U) {
        result.status=Win32ScanStatus::InvalidScan;
        return result;
    }
    // Windows has already collapsed the sequence into one keyboard
    // message. Never invent the missing DOS E0/E1 prefix or its timing.
    if(result.extended) {
        result.status=Win32ScanStatus::ExtendedSequenceUnresolved;
        return result;
    }
    // A repeated WM_KEYDOWN is not evidence of HOW MANY port-60 IRQs
    // fired, nor WHEN. Preserve metadata but do not synthesize IRQs.
    if(result.repeat_count!=1U ||
       (!result.key_up && result.previous_down)) {
        result.status=Win32ScanStatus::RepeatTimingUnresolved;
        return result;
    }
    result.status=Win32ScanStatus::SingleUnextended;
    result.candidate_port60=static_cast<std::uint8_t>(
        result.key_up ? (result.oem_scan|0x80U) : result.oem_scan);
    return result;
}

} // namespace rloft
