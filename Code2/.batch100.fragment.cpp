// Batch 100: 33B44..33C8F mouse/runtime initialization with DOS-memory probe fallback.
struct Runtime100MouseResetResult {
    u32 eax=0u;
    u32 ebx=0u;
};
struct Runtime100Boundary {
    Runtime100MouseResetResult (*int33_reset)()=nullptr;
    u32 (*call_30d80)(u32,u32,u32)=nullptr;
    u32 (*call_33aa0)(u32)=nullptr;
    u32 (*call_30de8)(u32)=nullptr;
    void (*call_311e8)(u32)=nullptr;
    void (*call_27ecc)()=nullptr;
    void (*call_285d4)()=nullptr;
    void (*call_362be)(u32)=nullptr;
    void (*call_30c38)()=nullptr;
    void (*call_352be)(u32)=nullptr;
};

u32 function_33b44(Machine& m,Runtime100Boundary& b) noexcept {
    const Runtime100MouseResetResult reset=b.int33_reset?b.int33_reset():Runtime100MouseResetResult{};
    store_le16(dg(m,0x2B7A2u),static_cast<u16>(reset.eax));
    store_le16(dg(m,0x2B7A6u),0u);
    store_le16(dg(m,0x2B7A4u),static_cast<u16>(reset.ebx));

    if(load_le16(dg(m,0x2B7A2u))!=0u){
        const u32 linear=b.call_30d80?b.call_30d80(0x40u,0xFD14u,0xFD18u):0u;
        store_le32(dg(m,0xFD10u),linear);
        if(linear==0u)return 0x2B7A2u;

        const u32 frame=0x2B770u;
        for(u32 i=0u;i<0x32u;++i)*dg(m,frame+i)=0u;
        store_le32(dg(m,frame+0x1Cu),0x000053C1u);
        const u32 segment=load_le32(dg(m,0xFD14u))>>16;
        store_le32(dg(m,frame+0x14u),0u);
        store_le16(dg(m,frame+0x22u),static_cast<u16>(segment));
        if(b.call_33aa0)(void)b.call_33aa0(frame);

        if(load_le32(dg(m,frame+0x1Cu))==1u){
            const u16 probe=static_cast<u16>(*dg(m,load_le32(dg(m,0xFD10u))));
            store_le16(dg(m,0x2B7A6u),probe);
        }
        if(load_le16(dg(m,0x2B7A6u))==0u){
            const u32 selector=static_cast<u32>(load_le16(dg(m,0xFD18u)));
            if(b.call_30de8)(void)b.call_30de8(selector);
        }
        return 0x2B7A2u;
    }

    if(b.call_311e8)b.call_311e8(0x15C8u);
    const u32 device=load_le32(dg(m,0xF050u));
    const u32 cleanup_mask=load_le32(dg(m,0xEE78u))|load_le32(dg(m,0xEE40u));
    const s16 device_kind=static_cast<s16>(load_le16(dg(m,device+0x08u)));
    if(device_kind!=0x71){
        if(load_le16(dg(m,device+0x14u))!=0u && b.call_27ecc)b.call_27ecc();
        if(b.call_285d4)b.call_285d4();
        if(cleanup_mask!=0u && b.call_362be)b.call_362be(0u);
    }
    if(b.call_30c38)b.call_30c38();
    if(b.call_352be)b.call_352be(1u);
    return 0x2B7A2u;
}
static_assert(0x33C90u-0x33B44u==332u);
