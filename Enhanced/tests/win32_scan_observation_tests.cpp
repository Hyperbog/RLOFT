#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/win32_scan_observation.hpp"

#include <cassert>
#include <cstdint>

namespace {
std::uint64_t param(unsigned scan, unsigned repeats=1U,
                    bool ext=false, bool alt=false,
                    bool previous=false, bool up=false) {
    return static_cast<std::uint64_t>(repeats)|
        (static_cast<std::uint64_t>(scan)<<16U)|
        (static_cast<std::uint64_t>(ext)<<24U)|
        (static_cast<std::uint64_t>(alt)<<29U)|
        (static_cast<std::uint64_t>(previous)<<30U)|
        (static_cast<std::uint64_t>(up)<<31U);
}
}
int main() {
    using M=rloft::Win32ScanObservationModel;
    using S=rloft::Win32ScanStatus;
    auto event=M::decode(0x100U,param(0x48U));
    assert(event.status==S::SingleUnextended &&
           event.candidate_port60==0x48U &&
           !event.key_up && !event.extended);
    event=M::decode(0x101U,param(0x48U,1U,false,false,true,true));
    assert(event.status==S::SingleUnextended &&
           event.candidate_port60==0xC8U && event.key_up);
    // Extended navigation clusters and right ALT/CTRL cannot lose E0.
    event=M::decode(0x100U,param(0x48U,1U,true));
    assert(event.status==S::ExtendedSequenceUnresolved &&
           !event.candidate_port60 && event.oem_scan==0x48U);
    event=M::decode(0x105U,param(0x38U,1U,true,true,true,true));
    assert(event.status==S::ExtendedSequenceUnresolved &&
           event.system_key && event.alt_context && event.key_up);
    // Ordinary system ALT is distinguishable from the extended right ALT.
    event=M::decode(0x104U,param(0x38U,1U,false,true));
    assert(event.status==S::SingleUnextended &&
           event.candidate_port60==0x38U && event.system_key);
    // Both aggregated repeats and bit-30 autorepeat need timing evidence.
    event=M::decode(0x100U,param(0x1EU,3U));
    assert(event.status==S::RepeatTimingUnresolved &&
           !event.candidate_port60 && event.repeat_count==3U);
    event=M::decode(0x100U,param(0x1EU,1U,false,false,true));
    assert(event.status==S::RepeatTimingUnresolved &&
           !event.candidate_port60);
    event=M::decode(0x100U,param(0x00U));
    assert(event.status==S::InvalidScan && !event.candidate_port60);
    event=M::decode(0x100U,param(0x80U));
    assert(event.status==S::InvalidScan && !event.candidate_port60);
    event=M::decode(0x101U,param(0x1EU,1U,false,false,false,true));
    assert(event.status==S::InvalidTransition && !event.candidate_port60);
    event=M::decode(0x100U,param(0x1EU,1U,false,false,false,true));
    assert(event.status==S::InvalidTransition && !event.candidate_port60);
    event=M::decode(0x102U,param(0x1EU));  // WM_CHAR is never a raw scan.
    assert(event.status==S::NotKeyboardMessage && !event.candidate_port60);
    // A 64-bit sign-extended LPARAM must decode only its low 32 bits.
    event=M::decode(0x101U,param(0x1EU,1U,false,false,true,true)|
                             0xFFFFFFFF00000000ULL);
    assert(event.status==S::SingleUnextended &&
           event.candidate_port60==0x9EU);
    // Decoder only observes metadata: no DOS input state is mutated.
}
