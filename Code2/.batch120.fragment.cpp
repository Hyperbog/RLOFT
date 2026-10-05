// Batch 120: first runtime island after the timer/event family, 369A7..369D7.
struct Runtime120Boundary {
    void (*call_register_guest)(u32,u32,u32,u32)=nullptr; // target, ECX, ESI, EDI
};

u32 function_369a7(u32 value) noexcept {
    if(static_cast<s32>(value)<0)value=0u-value;
    return value;
}
static_assert(0x369B2u-0x369A7u==11u);

// 369B2..369BF is a fourteen-byte zero island between two complete RET-bounded helpers.

void function_369c0(Runtime120Boundary& b,u32 target,u32 ecx,u32 esi,u32 edi) noexcept {
    // The ENTER frame makes these four values available at [EBP+8,+0C,+10,+14].
    // The binary then transfers the last three into ECX/ESI/EDI and performs CALL [EBP+8].
    if(b.call_register_guest)b.call_register_guest(target,ecx,esi,edi);
}
static_assert(0x369D8u-0x369C0u==24u);
