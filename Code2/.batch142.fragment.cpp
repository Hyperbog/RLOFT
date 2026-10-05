
// Batch 142: verified CODE.2 39C30..39D4C.
struct Runtime142Boundary {
    // Returns the inclusive DS selector limit, matching LSL.
    u32 (*ds_limit)()=nullptr;
    // DPMI 0501h allocation: returns linear base and stores handle words.
    u32 (*dpmi_alloc)(u32 bytes,u16* handle_hi,u16* handle_lo)=nullptr;
    // DOS AH=48h allocation: returns signed carry-folded selector/segment result.
    s32 (*dos_alloc_paragraphs)(u16 paragraphs)=nullptr;
    void (*dos_free_selector)(u16 selector)=nullptr;
    // DPMI 0006h: selector -> linear base.
    u32 (*selector_base)(u16 selector)=nullptr;
};

[[nodiscard]] u32 function_39c30(Machine& m,Runtime142Boundary& b) noexcept {
    const u32 head=load_le32(dg(m,0x12624u));
    if(head==0u)return 0u;
    const u32 arena=load_le32(dg(m,head+0x24u));
    const u32 end=arena+load_le32(dg(m,arena))+4u;
    const u8 mode=dg(m,0x12662u)[0];
    if(mode>=2u && mode<=8u && dg(m,0x12664u)[0]==0u && b.ds_limit)
        store_le32(dg(m,0x12634u),b.ds_limit()+1u);
    return end==load_le32(dg(m,0x12634u)) ? load_le32(dg(m,arena)) : 0u;
}
static_assert(0x39C7Bu-0x39C30u==75u);

[[nodiscard]] u32 function_39c7b(Machine& m,Runtime142Boundary& b,u32 bytes) noexcept {
    function_39a2c(m,b);
    if(b.dpmi_alloc){
        u16 hi=0u,lo=0u;
        const u32 base=b.dpmi_alloc(bytes,&hi,&lo);
        if(base!=0u){
            store_le16(dg(m,base),hi); store_le16(dg(m,base+2u),lo);
            store_le32(dg(m,base+4u),0u);
            store_le32(dg(m,base+8u),bytes-8u);
            return base+8u;
        }
    }
    if((load_le16(dg(m,0x2BD0Au))&0xFFF0u)!=0u)store_le32(dg(m,0x2BD08u),0x000FFFFFu);
    if(bytes>0x10000u || !b.dos_alloc_paragraphs)return 0u;
    const u16 probe_paras=static_cast<u16>((load_le32(dg(m,0x2BD08u))>>4u)|1u);
    const s32 probe=b.dos_alloc_paragraphs(probe_paras);
    if(probe<0)return 0u;
    const s32 selector=b.dos_alloc_paragraphs(static_cast<u16>(bytes>>4u));
    if(b.dos_free_selector)b.dos_free_selector(static_cast<u16>(probe));
    if(selector<0 || !b.selector_base)return 0u;
    const u32 base=b.selector_base(static_cast<u16>(selector))+8u;
    store_le32(dg(m,base-4u),static_cast<u32>(selector));
    store_le32(dg(m,base),bytes-8u);
    return base;
}
static_assert(0x39D4Du-0x39C7Bu==210u);
static_assert((0x39C7Bu-0x39C30u)+(0x39D4Du-0x39C7Bu)==285u);
