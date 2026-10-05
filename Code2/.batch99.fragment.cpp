// Batch 99: 33AA0..33B41 DPMI real-mode INT 33h simulation and probe.
struct Runtime99Boundary {
    u16 ds_selector=0u;
    // Models the already translated 33A7C adapter while preserving its raw frame layout.
    u32 (*call_33a7c)(u32,u32*,u32*,u16*)=nullptr;
};

u32 function_33aa0(Runtime99Boundary& b,u32 real_mode_frame) noexcept {
    u32 regs[7]={};       // 1Ch-byte input/output register block at local +00h.
    u16 segments[6]={};   // 0Ch-byte segment-register block at local +1Ch.
    regs[0]=0x00000300u;
    regs[1]=0x00000033u;
    regs[2]=0u;
    regs[5]=real_mode_frame; // Encoded dword at local +14h.
    segments[0]=b.ds_selector; // Encoded word at segment block +00h.
    if(b.call_33a7c)return b.call_33a7c(0x31u,regs,regs,segments);
    return regs[0];
}

u32 function_33af4(Machine& m,Runtime99Boundary& b) noexcept {
    const u32 frame=0x2B770u;
    for(u32 i=0u;i<0x32u;++i)*dg(m,frame+i)=0u;
    store_le32(dg(m,frame+0x1Cu),0x000053C1u);
    const u32 segment=load_le32(dg(m,0xFD14u))>>16;
    store_le32(dg(m,frame+0x14u),0u);
    store_le16(dg(m,frame+0x22u),static_cast<u16>(segment));
    (void)function_33aa0(b,frame);
    return load_le32(dg(m,frame+0x1Cu))==1u?1u:0u;
}

static_assert(0x33AF3u-0x33AA0u==83u);
// 33AF3 is an unreferenced NOP alignment byte.
static_assert(0x33B42u-0x33AF4u==78u);
// 33B42..33B43 is unreferenced MOV EAX,EAX alignment; 33B44 has independent callers.
