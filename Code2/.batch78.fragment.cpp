// Batch 78: 320F8..32141 formatter byte-emitter and sprintf-style wrapper.
struct Application78Boundary {
    u32 (*linear_load32)(u32)=nullptr;
    void (*linear_store32)(u32,u32)=nullptr;
    void (*linear_store8)(u32,u8)=nullptr;
    u32 (*call_385b0)(u32,u32,const u32*,u32)=nullptr;
};

// 320F8 callback: state+0 is the output cursor and state+10h is emitted count.
[[nodiscard]] u32 function_320f8(Application78Boundary& b,u32 state,u32 ch) noexcept {
    const u32 cursor=b.linear_load32?b.linear_load32(state):0u;
    if(b.linear_store32)b.linear_store32(state,cursor+1u);
    if(b.linear_store8)b.linear_store8(cursor,static_cast<u8>(ch));
    const u32 count=b.linear_load32?b.linear_load32(state+0x10u):0u;
    if(b.linear_store32)b.linear_store32(state+0x10u,count+1u);
    return state;
}
static_assert(0x3210Fu-0x320F8u==23u);

// 3210F wrapper: the reconstruction surface supplies the encoded varargs cursor
// explicitly; 385B0 receives destination, format, that cursor, and callback 320F8.
[[nodiscard]] u32 function_3210f(Machine& m,Application78Boundary& b,u32 dst,u32 format,const u32* varargs) noexcept {
    const u32 count=b.call_385b0?b.call_385b0(dst,format,varargs,0x320F8u):0u;
    *dg(m,dst+count)=0u;
    return count;
}
static_assert(0x32142u-0x3210Fu==51u);
// 32142..3214F is fourteen zero padding bytes with no incoming direct control-flow target.
