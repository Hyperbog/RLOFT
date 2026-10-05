// Batch 81: 324CC..3261B palette blanking, triple rotation, and resource-backed palette load.
struct Application81Boundary {
    Application56Boundary* app56=nullptr;
    Application70Boundary* app70=nullptr;
    Application72Boundary* app72=nullptr;
};

// 324CC..324F7: publish an all-zero 256-entry palette and set the presentation flag.
void function_324cc(Machine& m,Application81Boundary& b) noexcept {
    function_32404(m,0xF9D0u,0u,0u,0u,0u,0x100u);
    if(b.app56)function_29234(*b.app56,0xF9D0u);
    function_2f83c(m);
}
static_assert(0x324F8u-0x324CCu==44u);

// 324F8..32585: remove one RGB triple from a span and append it at the span end.
// The binary uses forward REP MOVS from selected+3 back to selected after first
// saving the selected triple on its four-byte local stack slot.
void function_324f8(Machine& m,u32 palette,s32 first,s32 relative,s32 span) noexcept {
    if(span<=1)return;
    const u32 selected=palette+static_cast<u32>(first)*3u+static_cast<u32>(relative)*3u;
    const u8 saved[3]={*dg(m,selected),*dg(m,selected+1u),*dg(m,selected+2u)};
    const u32 move_bytes=static_cast<u32>(span)*3u-3u;
    for(u32 i=0;i<move_bytes;++i)*dg(m,selected+i)=*dg(m,selected+3u+i);
    for(u32 i=0;i<3u;++i)*dg(m,selected+move_bytes+i)=saved[i];
}
static_assert(0x32586u-0x324F8u==142u);
// 32586..32587 is MOV EAX,EAX alignment with no incoming direct control-flow target.

// 32588..325AC: open resource mode 0200h; report 15ACh if the handle is -1.
[[nodiscard]] u32 function_32588(Machine& m,Application81Boundary& b,u32 resource) noexcept {
    const u32 handle=b.app72?function_312a0(*b.app72,resource):0xFFFFFFFFu;
    if(handle==0xFFFFFFFFu && b.app70)function_2f580(m,*b.app70,0x15ACu);
    return handle;
}
static_assert(0x325ADu-0x32588u==37u);
// 325AD..325AF is LEA alignment with no incoming direct control-flow target.

// 325B0..3261B: seek to row ((resource_row*3)<<8)+1, read count RGB triples
// into palette+destination_first*3, apply the active palette rescaler, then close.
[[nodiscard]] u32 function_325b0(Machine& m,Application81Boundary& b,u32 palette,u32 destination_first,u32 resource_row,u32 resource,u32 count) noexcept {
    const u32 handle=function_32588(m,b,resource);
    const u32 encoded_offset=((resource_row*3u)<<8)+1u;
    if(b.app72)(void)function_3131c(*b.app72,encoded_offset,1u,handle);
    const u32 byte_count=count*3u;
    const u32 destination=palette+destination_first*3u;
    if(b.app72)(void)function_312ec(*b.app72,destination,byte_count,handle);
    function_32150(m,palette);
    return b.app72?function_312dc(*b.app72,handle):0u;
}
static_assert(0x3261Cu-0x325B0u==108u);
// 3261C..3261F is four zero padding bytes with no incoming direct control-flow target.
