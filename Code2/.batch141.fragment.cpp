
// Batch 141: verified CODE.2 39A2C..39A89 and 39BBC..39C2F.
// 39A8A..39BBB is deliberately not owned by this batch.
struct Runtime141Boundary {
    void (*release_dpmi_block)(u32 handle)=nullptr;
    void (*release_dos_block)(u16 selector)=nullptr;
};

void function_39a2c(Machine& m,Runtime141Boundary& b) noexcept {
    u32 node=load_le32(dg(m,0x12624u));
    while(node!=0u){
        const u32 descriptor=load_le32(dg(m,node+0x24u));
        if(load_le32(dg(m,descriptor))+0x2Cu!=node){node=load_le32(dg(m,node+8u));continue;}
        const u32 next=load_le32(dg(m,node+8u));
        function_399eb(m,node);
        const u32 owner=node-8u;
        const u32 dos_selector=load_le32(dg(m,owner+4u));
        if(dos_selector==0u){
            const u32 handle=load_le32(dg(m,owner));
            if(b.release_dpmi_block)b.release_dpmi_block(handle);
        }else if(b.release_dos_block)b.release_dos_block(static_cast<u16>(dos_selector));
        node=next;
    }
}
static_assert(0x39A8Au-0x39A2Cu==94u);

[[nodiscard]] u32 function_39bbc(Machine& m,u32 node) noexcept {
    u32 cur=load_le32(dg(m,0x12624u));
    u32 prev=0u;
    while(cur!=0u && node>=cur){prev=cur;cur=load_le32(dg(m,cur+8u));}
    store_le32(dg(m,node+4u),prev);
    store_le32(dg(m,node+8u),cur);
    if(prev!=0u)store_le32(dg(m,prev+8u),node);else store_le32(dg(m,0x12624u),node);
    if(cur!=0u)store_le32(dg(m,cur+4u),node);

    const u32 sentinel=node+0x20u;
    const u32 arena=node+0x2Cu;
    const u32 total=load_le32(dg(m,node));
    store_le32(dg(m,node+0x20u),0u);
    store_le32(dg(m,node+0x10u),0u);
    store_le32(dg(m,node+0x18u),0u);
    store_le32(dg(m,node+0x1Cu),0u);
    store_le32(dg(m,node+0x24u),sentinel);
    store_le32(dg(m,node+0x28u),sentinel);
    store_le32(dg(m,node+0x0Cu),sentinel);
    const u32 payload=total-0x2Cu;
    store_le32(dg(m,arena),payload);
    store_le32(dg(m,arena+payload),0xFFFFFFFFu);
    return arena;
}
static_assert(0x39C30u-0x39BBCu==116u);
static_assert((0x39A8Au-0x39A2Cu)+(0x39C30u-0x39BBCu)==210u);
