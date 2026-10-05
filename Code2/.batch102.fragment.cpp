// Batch 102: 33DE0..33E6A planar VGA fill pair.
struct Runtime102Boundary {
    // 33DE0/33E28 emit one 16-bit OUT DX,AX to the VGA sequencer.
    void (*out16)(u16,u16)=nullptr;
    // Preserve ES-qualified REP STOSB semantics; caller advances EDI by count exactly as the instruction does.
    void (*rep_stosb_es)(u32,u8,u32)=nullptr;
};

void function_33de0(Machine& m,Runtime102Boundary& b,u32 value) noexcept {
    if(b.out16)b.out16(0x03C4u,0x0F02u);
    u32 edi=load_le32(dg(m,0xFDBCu))+0x50F8u;
    u32 ebx=0x80u;
    do {
        const u8 al=static_cast<u8>(static_cast<u8>(value)-*dg(m,0xFE94u+ebx));
        if(b.rep_stosb_es)b.rep_stosb_es(edi,al,0x40u);
        edi+=0x40u;
        edi-=0x90u;
        --ebx;
    } while(ebx!=0u);
}

void function_33e28(Machine& m,Runtime102Boundary& b,u32 value) noexcept {
    if(b.out16)b.out16(0x03C4u,0x0F02u);
    u32 edi=load_le32(dg(m,0xFDBCu))+0x148u;
    u32 ebx=0x80u;
    do {
        const u8 al=static_cast<u8>(static_cast<u8>(value)-*dg(m,0xFE94u+ebx));
        if(b.rep_stosb_es)b.rep_stosb_es(edi,al,0x40u);
        edi+=0x40u;
        edi+=0x10u;
        --ebx;
    } while(ebx!=0u);
}

static_assert(0x33E26u-0x33DE0u==70u);
// 33E26..33E27 is unreferenced XCHG EBX,EBX alignment.
static_assert(0x33E6Bu-0x33E28u==67u);
// 33E6B is an unreferenced NOP; 33E6C has independent callers.
