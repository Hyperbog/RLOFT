
// Semantic-audit repair: explicit reconstruction of 39A8A..39BBB.
// Immutable span SHA-256: a9287a29464272bd2ace173544a54f3d34989fc6032b0b6faaa07b2e310435b3
// DPMI AX=0503h resizes an existing linear-memory block. Success is CF-clear;
// returned BX:CX is the new linear base and SI:DI is the updated handle.
struct Runtime39a8aBoundary {
    Runtime141Boundary* cleanup=nullptr;
    bool (*dpmi_resize)(u32 bytes,u16 old_hi,u16 old_lo,
                        u32* new_base,u16* new_hi,u16* new_lo)=nullptr;
    void (*release_payload)(u32 payload)=nullptr; // encoded call 37C45h
};

[[nodiscard]] u32 function_39a8a(Machine& m,Runtime39a8aBoundary& b,
                                 u32 arena,u32 requested_payload) noexcept {
    if(b.cleanup)function_39a2c(m,*b.cleanup);

    u32 node=load_le32(dg(m,0x12624u));
    while(node!=0u){
        if(node>arena)return 0u;                         // unsigned JA
        if(node+0x2Cu!=arena || load_le32(dg(m,node+0x18u))!=1u){
            node=load_le32(dg(m,node+8u));
            continue;
        }

        // The encoded routine unlinks before validating backing ownership and
        // before attempting DPMI resize. Failure therefore does not relink it.
        function_399eb(m,node);
        const u32 owner=node-8u;
        if(load_le32(dg(m,owner+4u))!=0u)return 0u;       // DOS-backed: cannot resize here

        const u32 old_total=load_le32(dg(m,node))+0x0Cu;
        const u32 old_payload=load_le32(dg(m,arena))-4u;
        const u32 delta=requested_payload-old_payload;   // modulo-32-bit, as encoded
        const u32 resize_bytes=(old_total+delta+0x0FFFu)&0xFFFFF000u;

        const u16 old_hi=load_le16(dg(m,owner));
        const u16 old_lo=load_le16(dg(m,owner+2u));
        u32 base=0u; u16 new_hi=0u,new_lo=0u;
        if(!b.dpmi_resize || !b.dpmi_resize(resize_bytes,old_hi,old_lo,
                                            &base,&new_hi,&new_lo))return 0u;

        store_le16(dg(m,base),new_hi);
        store_le16(dg(m,base+2u),new_lo);
        store_le32(dg(m,base+4u),0u);
        const u32 new_node=base+8u;
        store_le32(dg(m,new_node),resize_bytes-0x0Cu);

        const u32 new_arena=function_39bbc(m,new_node);
        store_le32(dg(m,new_node+0x18u),1u);

        const u32 wanted=(requested_payload+1u)&0xFFFFFFFEu;
        const u32 remainder=load_le32(dg(m,new_arena))-wanted;
        const u32 result=new_arena;
        if(remainder>=0x0Cu){
            store_le32(dg(m,new_arena),wanted|1u);
            const u32 tail=new_arena+wanted;
            store_le32(dg(m,tail),remainder|1u);
            store_le32(dg(m,new_node+0x14u),0xFFFFFFFFu);
            store_le32(dg(m,new_node+0x18u),load_le32(dg(m,new_node+0x18u))+1u);
            if(b.release_payload)b.release_payload(tail+4u);
        }else{
            *dg(m,new_arena)=static_cast<u8>(*dg(m,new_arena)|1u);
        }
        return result;
    }
    return 0u;
}
static_assert(0x39BBCu-0x39A8Au==306u);
