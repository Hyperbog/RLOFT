#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_dos_keyboard_irq.hpp"

#include <cassert>
#include <cstdint>
#include <initializer_list>

int main(){
    using rloft::OriginalDosKeyboardIrqState;
    using M=rloft::OriginalDosKeyboardIrqModel;
    OriginalDosKeyboardIrqState s{};
    assert(!M::held_308bc(s,0x48U));
    M::press_307f0(s,0x47U);
    assert(s.count_f588==1U && s.input.pending_f572==0x47U);
    assert(s.direction_258fc==1U && s.step_258fe==0xFFFFU);
    M::press_307f0(s,0x48U);
    assert(s.count_f588==2U && M::held_308bc(s,0x47U));
    assert(s.direction_258fc==3U && s.step_258fe==0xFFFFU);
    M::press_307f0(s,0x47U); // duplicate moves to end
    assert(s.count_f588==2U && s.held_2b6c4[0]==0x48U);
    assert(s.held_2b6c4[1]==0x47U);
    M::release_30748(s,0x47U);
    assert(s.count_f588==1U && s.input.pending_f572==0x48U);
    assert(s.last_f571==0x48U);
    M::release_30748(s,0x33U); // absent: last still published
    assert(s.count_f588==1U && s.input.pending_f572==0x48U);
    M::release_30748(s,0x48U);
    assert(s.count_f588==0U && s.input.pending_f572==0U);
    assert(!M::held_308bc(s,0x48U));

    // Source-specific navigation mapping table includes holes 4A/4C/4E/4F/50.
    for(auto sc:{0x47U,0x48U,0x49U,0x4BU,0x4DU,0x51U}) {
        s.direction_258fc=100U;
        M::press_307f0(s,static_cast<std::uint8_t>(sc));
        assert(s.direction_258fc!=100U);
    }
    s.direction_258fc=100U;
    M::press_307f0(s,0x50U);
    assert(s.direction_258fc==100U);

    // Raw IRQ source treats 0x80 as a make event (JG 80h, not JGE).
    s={};
    assert(!M::dispatch_30950(s,0x80U));
    assert(s.input.pending_f572==0x80U);
    assert(!M::dispatch_30950(s,0x2AU));
    assert(s.modifiers_f573==1U && s.input.pending_f572==0x80U);
    assert(!M::dispatch_30950(s,0xAAU));
    assert(s.modifiers_f573==0U && s.input.pending_f572==0x80U);

    assert(!M::dispatch_30950(s,0x38U)); // alt make
    assert(s.modifiers_f573==4U);
    assert(!M::dispatch_30950(s,0x10U)); // Alt+Q special
    assert((s.alt_q_f0b1&1U)==1U && s.input.pending_f572==0U);
    assert(!M::dispatch_30950(s,0xB8U)); // alt break
    assert(s.modifiers_f573==0U);

    assert(M::dispatch_30950(s,0x43U)); // early IRET
    assert(s.input.pending_f572==0U);
    assert(M::dispatch_30950(s,0x19U)); // early IRET
    assert(s.break_flag_1457c==1U && s.input.pending_f572==0x19U);
    assert(!M::dispatch_30950(s,0x39U));
    assert(s.space_f574==1U && s.input.pending_f572==0x39U);

    s={};
    assert(!M::dispatch_30950(s,0x48U));
    assert(s.count_f588==1U && s.last_f571==0x48U);
    assert(!M::dispatch_30950(s,0x49U));
    assert(s.count_f588==2U && s.last_f571==0x49U);
    assert(!M::dispatch_30950(s,0xC9U));
    assert(s.count_f588==1U && s.input.pending_f572==0x48U);
    assert(!M::dispatch_30950(s,0xC8U));
    assert(s.count_f588==0U && s.input.pending_f572==0U);
    // Normal key release does not zero current; restores last nav.
    assert(!M::dispatch_30950(s,0x48U));
    assert(!M::dispatch_30950(s,0x1EU));
    assert(s.input.pending_f572==0x1EU);
    assert(!M::dispatch_30950(s,0x9EU));
    assert(s.input.pending_f572==0x48U);
}
