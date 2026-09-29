#include "games/ravenloft/game/OriginalGameplayClock19C80.h"

namespace ravenloft::game {

void OriginalGameplayClock19C80::irq30708(){
    // 0x30717..0x30722: the old value is loaded but unused by the opening
    // gameplay path, then DS:F58C is incremented before 0x19C80 executes.
    ++timerF58C_;

    // 0x19CB3..0x19CC2: A7E4 advances only while A7E8 is nonzero.
    if(enabledA7E8_!=0)++gameplayA7E4_;

    // 0x30727 chains the previous IRQ0/BIOS handler. With the original
    // divisor 65536 this is one standard BIOS tick per game timer pulse.
    ++biosTick046C_;
}

void OriginalGameplayClock19C80::push19CCC(std::uint32_t enabledValue){
    // 0x19CCC..0x19CF3: only the low byte of the previous A7E8 value is
    // stored, while the new argument is written as a full DWORD.
    if(stackDepthA7EC_>=kEnableStackCapacity)return;
    enableStack140A0_[stackDepthA7EC_] = static_cast<std::uint8_t>(enabledA7E8_);
    ++stackDepthA7EC_;
    enabledA7E8_ = enabledValue;
}

void OriginalGameplayClock19C80::pop19CF4(){
    // 0x19CF4..0x19D15: empty pop changes neither A7E8 nor depth.
    if(stackDepthA7EC_==0)return;
    --stackDepthA7EC_;
    enabledA7E8_ = enableStack140A0_[stackDepthA7EC_];
}

} // namespace ravenloft::game
