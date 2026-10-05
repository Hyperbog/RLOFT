
// Batch 128: optimized fill helpers, selector snapshot, and DOS memory wrappers.
struct Runtime128Boundary {
    u16 cs=0,ds=0,es=0,ss=0,fs=0,gs=0;
    u32 (*dos_alloc)(u16,u16*)=nullptr;
    u32 (*dos_free)(u16)=nullptr;
    u32 (*dos_resize)(u16,u16,u16*)=nullptr;
};

// 379D0..37A00: byte-count fill entry. The binary aligns EAX bytewise while
// rotating EDX, delegates aligned dword groups to 37A07, then emits 0..3 tail bytes.
void function_379d0(u8* dst,u32 count,u32 pattern) noexcept {
    u32 i=0u;
    while(count!=0u && ((reinterpret_cast<std::uintptr_t>(dst+i)&3u)!=0u)){
        dst[i++]=static_cast<u8>(pattern); pattern=(pattern>>8)|(pattern<<24); --count;
    }
    const u32 dwords=count>>2;
    for(u32 n=0;n<dwords;++n)store_le32(dst+i+4u*n,pattern);
    i+=dwords*4u; count&=3u;
    if(count>0u)dst[i]=static_cast<u8>(pattern);
    if(count>1u)dst[i+1]=static_cast<u8>(pattern>>8);
    if(count>2u)dst[i+2]=static_cast<u8>(pattern>>16);
}
static_assert(0x37A01u-0x379D0u==49u);

// 37A07..37A72: aligned dword-count fill helper; unrolled stores do not alter value.
void function_37a07(u8* dst,u32 dword_count,u32 value) noexcept {
    for(u32 i=0;i<dword_count;++i)store_le32(dst+4u*i,value);
}
static_assert(0x37A73u-0x37A07u==108u);

// 37A73..37A9C: snapshot ES,CS,SS,DS,FS,GS in the encoded six-word order.
void function_37a73(Runtime128Boundary& b,u16* out) noexcept {
    out[0]=b.es; out[1]=b.cs; out[2]=b.ss; out[3]=b.ds; out[4]=b.fs; out[5]=b.gs;
}
static_assert(0x37A9Du-0x37A73u==42u);

// 37A9E..37AC1: DOS AH=48h allocation. On success *result receives AX and EAX=0;
// on failure DOS BX (largest available block) is retained in *result.
[[nodiscard]] u32 function_37a9e(Runtime128Boundary& b,u16 paragraphs,u16& result) noexcept {
    return b.dos_alloc?b.dos_alloc(paragraphs,&result):0u;
}
static_assert(0x37AC2u-0x37A9Eu==36u);

// 37AC2..37AD3: DOS AH=49h free wrapper.
[[nodiscard]] u32 function_37ac2(Runtime128Boundary& b,u16 segment) noexcept {
    return b.dos_free?b.dos_free(segment):0u;
}
static_assert(0x37AD4u-0x37AC2u==18u);

// 37AD4..37AFB: DOS AH=4Ah resize. On failure the returned BX maximum is stored.
[[nodiscard]] u32 function_37ad4(Runtime128Boundary& b,u16 paragraphs,u16 segment,u16& maximum) noexcept {
    return b.dos_resize?b.dos_resize(segment,paragraphs,&maximum):0u;
}
static_assert(0x37AFCu-0x37AD4u==40u);
