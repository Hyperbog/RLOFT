
// Batch 125: runtime allocation/read helpers at 374E0..375FC.
struct Runtime125Boundary {
    u32 (*alloc)(u32)=nullptr;
    void (*free_block)(u32)=nullptr;
    void (*read_block)(u32,u32,u32)=nullptr; // destination, count, stream/context
    void (*call_2f960)(u32)=nullptr;
    void (*call_2f96c)()=nullptr;
    void (*call_2fb04)(u32,u32,u32,u32,u32)=nullptr;
    u8 (*read8)(u32)=nullptr;
    void (*write8)(u32,u8)=nullptr;
};
static inline u8 runtime125_read8(Runtime125Boundary& b,u32 p) noexcept { return b.read8?b.read8(p):0u; }
static inline void runtime125_write8(Runtime125Boundary& b,u32 p,u8 v) noexcept { if(b.write8)b.write8(p,v); }
static inline u16 runtime125_read16(Runtime125Boundary& b,u32 p) noexcept { return static_cast<u16>(runtime125_read8(b,p)|static_cast<u16>(runtime125_read8(b,p+1u))<<8); }
static inline u32 runtime125_read32(Runtime125Boundary& b,u32 p) noexcept { return static_cast<u32>(runtime125_read16(b,p))|(static_cast<u32>(runtime125_read16(b,p+2u))<<16); }
static inline void runtime125_write16(Runtime125Boundary& b,u32 p,u16 v) noexcept { runtime125_write8(b,p,static_cast<u8>(v));runtime125_write8(b,p+1u,static_cast<u8>(v>>8)); }

[[nodiscard]] u32 function_374e0(Runtime125Boundary& b,u32 descriptor,u32 stream) noexcept {
    const u32 src=runtime125_read32(b,descriptor+5u);
    const u32 count=runtime125_read32(b,src+0x16u);
    const u32 dst=b.alloc?b.alloc(count+0x1Bu):0u;
    if(dst==0u)return 0u;
    for(u32 i=0;i<0x1Cu;++i)runtime125_write8(b,dst+i,runtime125_read8(b,src+i));
    if(b.read_block)b.read_block(dst+0x1Cu,count-1u,stream);
    return dst;
}
static_assert(0x3757Cu-0x374E0u==156u);

void function_3757c(Runtime125Boundary& b,u32 block) noexcept { if(b.free_block)b.free_block(block); }
static_assert(0x37589u-0x3757Cu==13u);

[[nodiscard]] u32 function_37590(Runtime125Boundary& b,u32 stream,u32 setup_arg) noexcept {
    u16 count=0u;
    if(b.read_block){
        // The original reads exactly two bytes into a stack word before allocation.
        const u32 scratch=0xFFFFFFFFu;
        b.read_block(scratch,2u,stream);
        count=runtime125_read16(b,scratch);
    }
    const u32 dst=b.alloc?b.alloc(static_cast<u32>(count)+2u):0u;
    if(dst==0u)return 0u;
    runtime125_write16(b,dst,count);
    if(b.call_2f960)b.call_2f960(setup_arg);
    if(b.call_2f96c)b.call_2f96c();
    if(b.call_2fb04)b.call_2fb04(0u,0u,dst+2u,count,0u);
    return dst;
}
static_assert(0x375EFu-0x37590u==95u);

void function_375f0(Runtime125Boundary& b,u32 block) noexcept { if(b.free_block)b.free_block(block); }
static_assert(0x375FDu-0x375F0u==13u);
