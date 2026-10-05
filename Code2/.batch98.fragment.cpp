// Batch 98: 33A7C..33A9E generic interrupt-frame adapter.
struct Runtime98Boundary {
    // 39148 owns selector/register-frame mechanics; keep it opaque and address-specific.
    void (*call_39148)(u32,u32,u32,u32)=nullptr;
    // 33A97 explicitly reloads EAX from dword [output_regs] after 39148 returns.
    u32 (*read_output_eax)(u32)=nullptr;
};

u32 function_33a7c(Runtime98Boundary& b,u32 interrupt_number,u32 input_regs,
                   u32 output_regs,u32 segment_regs) noexcept {
    if(b.call_39148)b.call_39148(interrupt_number,input_regs,output_regs,segment_regs);
    return b.read_output_eax?b.read_output_eax(output_regs):0u;
}
static_assert(0x33A9Fu-0x33A7Cu==35u);
// 33A9F is an unreferenced zero alignment byte; 33AA0 is the next independent entry.
