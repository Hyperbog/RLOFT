#include "rloft/original_dos_keyboard_irq.hpp"

#include <cstddef>
#include <cstdint>

namespace rloft {

void OriginalDosKeyboardIrqModel::release_30748(
    OriginalDosKeyboardIrqState& s, std::uint8_t scan) {
    // 30748 uses the first matching entry. Its 30CD4 call is
    // overlap-safe, shifting subsequent bytes left exactly once.
    for(std::size_t i=0; i<s.count_f588; ++i) {
        if(s.held_2b6c4[i]!=scan) continue;
        for(std::size_t j=i+1U;j<s.count_f588;++j)
            s.held_2b6c4[j-1U]=s.held_2b6c4[j];
        s.count_f588=static_cast<std::uint8_t>(s.count_f588-1U);
        break;
    }
    // 30798..307C1 publishes the LAST held scan into both globals,
    // or sets F572 to zero if none remain; F571 mirrors F572.
    s.input.pending_f572=s.count_f588==0U
        ? 0U : s.held_2b6c4[s.count_f588-1U];
    s.last_f571=s.input.pending_f572;
}

void OriginalDosKeyboardIrqModel::press_307f0(
    OriginalDosKeyboardIrqState& s, std::uint8_t scan) {
    // Duplicate scan refresh removes its first old occurrence first.
    // The F588 counter is BYTE-sized; its overflow is original wrap,
    // not a fixed-capacity ten-key or FIFO queue.
    for(std::size_t i=0; i<s.count_f588; ++i) {
        if(s.held_2b6c4[i]==scan) {
            release_30748(s,scan);
            break;
        }
    }
    s.held_2b6c4[s.count_f588]=scan;
    s.count_f588=static_cast<std::uint8_t>(s.count_f588+1U);
    // 307C8 is an actual CS dispatch table indexed by scan-0x47.
    switch(scan) {
        case 0x47: s.direction_258fc=1U; s.step_258fe=0xFFFFU; break;
        case 0x48: s.direction_258fc=3U; s.step_258fe=0xFFFFU; break;
        case 0x49: s.direction_258fc=1U; s.step_258fe=1U; break;
        case 0x4B: s.direction_258fc=2U; s.step_258fe=0xFFFFU; break;
        case 0x4D: s.direction_258fc=2U; s.step_258fe=1U; break;
        case 0x50: s.direction_258fc=0U; s.step_258fe=1U; break;
        default: break;
    }
    s.input.pending_f572=scan;
    s.last_f571=scan;
}

bool OriginalDosKeyboardIrqModel::held_308bc(
    const OriginalDosKeyboardIrqState& s, std::uint8_t scan) {
    for(std::size_t i=0; i<s.count_f588; ++i)
        if(s.held_2b6c4[i]==scan) return true;
    return false;
}

std::uint32_t OriginalDosKeyboardIrqModel::modifiers_30abc(
    const OriginalDosKeyboardIrqState& s, std::uint32_t mask) {
    return static_cast<std::uint32_t>(s.modifiers_f573)&mask;
}

std::uint8_t OriginalDosKeyboardIrqModel::consume_space_30ac8(
    OriginalDosKeyboardIrqState& s) {
    const auto result=s.space_f574;
    s.space_f574=0U;
    return result;
}

bool OriginalDosKeyboardIrqModel::dispatch_30950(
    OriginalDosKeyboardIrqState& s, std::uint8_t raw_port60) {
    const bool is_break=raw_port60>0x80U; // source uses JLE 80h
    const auto scan=is_break ? static_cast<std::uint8_t>(raw_port60&0x7FU)
                             : raw_port60;
    const auto modifier_bit=[&]() -> std::uint8_t {
        switch(scan) {
            case 0x2A:return 1U;  // left shift
            case 0x36:return 2U;  // right shift
            case 0x38:return 4U;  // alt
            case 0x1D:return 8U;  // control
            default:return 0U;
        }
    }();
    if(modifier_bit) {
        if(is_break)
            s.modifiers_f573=static_cast<std::uint8_t>(
                s.modifiers_f573 & ~modifier_bit);
        else
            s.modifiers_f573=static_cast<std::uint8_t>(
                s.modifiers_f573 | modifier_bit);
    } else if(is_break) {
        if(scan>=0x47U && scan<=0x51U)
            release_30748(s,scan);
        else
            s.input.pending_f572=s.last_f571;
    } else {
        if(scan>=0x47U && scan<=0x51U) {
            press_307f0(s,scan);
            s.last_f571=scan;
        }
        s.input.pending_f572=scan;
    }
    // 30A1F onward: ALT+Q, F9, P and Space special routes,
    // including their ordering relative to F572 publication.
    if(s.input.pending_f572==0x10U && (s.modifiers_f573&4U)) {
        s.alt_q_f0b1=static_cast<std::uint8_t>(s.alt_q_f0b1|1U);
        s.input.pending_f572=0U;
        return false;
    }
    if(s.input.pending_f572==0x43U) {
        s.input.pending_f572=0U;
        return true;
    }
    if(s.input.pending_f572==0x19U) {
        s.break_flag_1457c=1U;
        return true;
    }
    if(s.input.pending_f572==0x39U) s.space_f574=1U;
    return false;
}

} // namespace rloft
