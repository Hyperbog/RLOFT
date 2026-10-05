// Batch 124: 3736D..3740D and 37433..374D1 radix conversion helpers.
struct Runtime124Boundary { void (*write8)(u32,u8)=nullptr; };
static inline void runtime124_write8(Runtime124Boundary& b,u32 p,u8 v) noexcept { if(b.write8)b.write8(p,v); }

[[nodiscard]] u32 function_3736d(Machine& m,Runtime124Boundary& b,u32 value,u32 dst,u32 radix) noexcept {
    (void)m; u8 tmp[40]{}; u32 n=0u;
    do { const u32 q=value/radix,r=value%radix; value=q; tmp[++n]=static_cast<u8>(r<10u?('0'+r):('a'+r-10u)); } while(value!=0u);
    u32 out=dst; do { const u8 ch=tmp[n--]; runtime124_write8(b,out++,ch); if(ch==0u)break; } while(true);
    return dst;
}
[[nodiscard]] u32 function_373bf(Machine& m,Runtime124Boundary& b,s32 value,u32 dst,u32 radix) noexcept {
    u32 out=dst,bits=static_cast<u32>(value); if(radix==10u&&value<0){bits=0u-bits;runtime124_write8(b,out++,static_cast<u8>('-'));}
    (void)function_3736d(m,b,bits,out,radix); return dst;
}
[[nodiscard]] u32 function_373f4(Machine& m,Runtime124Boundary& b,s32 value,u32 dst,u32 radix) noexcept { return function_373bf(m,b,value,dst,radix); }

[[nodiscard]] u32 function_37433(Machine& m,Runtime124Boundary& b,u32 value,u32 dst,u32 radix) noexcept {
    (void)m; u8 tmp[40]{}; u32 n=0u;
    do { const u32 q=value/radix,r=value%radix; value=q; tmp[++n]=static_cast<u8>(r<10u?('0'+r):('a'+r-10u)); } while(value!=0u);
    u32 out=dst; do { const u8 ch=tmp[n--]; runtime124_write8(b,out++,ch); if(ch==0u)break; } while(true);
    return dst;
}
[[nodiscard]] u32 function_37483(Machine& m,Runtime124Boundary& b,s32 value,u32 dst,u32 radix) noexcept {
    u32 out=dst,bits=static_cast<u32>(value); if(radix==10u&&value<0){bits=0u-bits;runtime124_write8(b,out++,static_cast<u8>('-'));}
    (void)function_37433(m,b,bits,out,radix); return dst;
}
[[nodiscard]] u32 function_374b8(Machine& m,Runtime124Boundary& b,s32 value,u32 dst,u32 radix) noexcept { return function_37483(m,b,value,dst,radix); }

static_assert(0x3740Eu-0x3736Du==161u);
static_assert(0x374D2u-0x37433u==159u);
