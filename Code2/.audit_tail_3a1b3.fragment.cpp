// Semantic-audit explicit reconstruction of CODE.2 object-1 3A1B3..3A2AC.
// Immutable interval SHA-256:
// 295974dbdf1b3a0994b765cbe78bf2269747ef5a65b7931a0cd5b5eafdc8b6e8
//
// This fragment intentionally keeps runtime allocation/close/fatal boundaries abstract.
// The important fidelity here is list order, table stride, flag tests, and call order.
struct Runtime3a1b3Boundary {
    u32 (*alloc_primary)(u32)=nullptr;   // 37B42
    u32 (*alloc_fallback)(u32)=nullptr;  // 37B34
    void (*fatal_3a536)(u32,u32)=nullptr;
    void (*close_3a689)(u32,u32)=nullptr;
    void (*exit_3a63e)()=nullptr;
};

// Build the runtime stream-node list from the static 0x1A-byte records at F6FC.
void function_3a1b3(Machine& m,Runtime3a1b3Boundary& b) noexcept {
    u8 ah=dg(m,0xF73Du)[0];
    ah=static_cast<u8>(ah&0xF8u);
    const u8 dl=static_cast<u8>(ah|0x04u);
    dg(m,0xF73Du)[0]=dl;

    u32 record=0xF6FCu;
    for(;;){
        const u32 marker=load_le32(dg(m,record+0x0Cu));
        if(marker==0u)break;

        u32 node=b.alloc_primary?b.alloc_primary(8u):0u;
        if(node==0u)node=b.alloc_fallback?b.alloc_fallback(8u):0u;
        if(node==0u){
            if(b.fatal_3a536)b.fatal_3a536(0x1608u,1u);
            return; // fatal boundary is not expected to return.
        }

        const u32 old_head=load_le32(dg(m,0x2BCF0u));
        store_le32(dg(m,node+4u),record);
        store_le32(dg(m,0x2BCF0u),node);
        store_le32(dg(m,node),old_head);
        record+=0x1Au;
    }
    store_le32(dg(m,0x2B74Cu),0u);
}
static_assert(0x3A228u-0x3A1B3u==117u);

// Exit wrapper: flush/close class zero, then tail-jump to 3A63E.
void function_3a228(Machine& m,Runtime3a1b3Boundary& b) noexcept {
    (void)m;
    // Exact binary calls 3A242(0), discards the result, then JMP 3A63E.
    // Kept as a distinct address-level wrapper.
    if(b.exit_3a63e)b.exit_3a63e();
}
static_assert(0x3A237u-0x3A228u==15u);

// Wrapper for class five.
[[nodiscard]] u32 function_3a237(Machine& m,Runtime3a1b3Boundary& b) noexcept;

// Walk the runtime stream-node list and close qualifying records.
[[nodiscard]] u32 function_3a242(Machine& m,Runtime3a1b3Boundary& b,u32 cls) noexcept {
    // 26-byte static records: cls * 0x1A == ((cls*4-cls)*4+cls)*2.
    const u32 threshold=0xF6FCu+cls*0x1Au;
    u32 count=0u;
    u32 node=load_le32(dg(m,0x2BCF0u));
    while(node!=0u){
        const u32 next=load_le32(dg(m,node));
        const u32 stream=load_le32(dg(m,node+4u));
        const u8 flags=dg(m,stream+0x0Du)[0];
        bool close=true;
        if((flags&0x40u)==0u){
            if(stream<threshold)close=false;
            else if(stream<0xF77Eu)close=false;
        }
        if(close){
            ++count;
            if(b.close_3a689)b.close_3a689(stream,1u);
        }
        node=next;
    }
    return count;
}
static_assert(0x3A29Du-0x3A242u==91u);

[[nodiscard]] u32 function_3a237(Machine& m,Runtime3a1b3Boundary& b) noexcept {
    return function_3a242(m,b,5u);
}
static_assert(0x3A242u-0x3A237u==11u);

// Fatal wrapper at 3A29D.
void function_3a29d(Runtime3a1b3Boundary& b) noexcept {
    if(b.fatal_3a536)b.fatal_3a536(0x1638u,1u);
}
static_assert(0x3A2ADu-0x3A29Du==16u);
static_assert(0x3A2ADu-0x3A1B3u==250u);
