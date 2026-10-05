// Batch 116: 358D4..35A08 compact resource/file loader.
struct Runtime116Boundary {
    u32 (*call_37e77)(u32,u32)=nullptr;
    // Address-specific 35878 bridge for SS/ES stack-local destinations that are
    // intentionally not flattened into Machine::data/DGROUP.
    u32 (*call_35878_stack)(Machine&,u32,u32,u32,u8*,u32)=nullptr;
    s32 (*call_27c70_stack)(Machine&,const u8*,u32)=nullptr;
    u32 (*call_380f8)(u32)=nullptr;
};

static u32 app116_read(Machine& m,Runtime116Boundary& b,u32 source,u32 offset,
                       u32 flags,u8* destination,u32 length) noexcept {
    if(b.call_35878_stack)
        return b.call_35878_stack(m,source,offset,flags,destination,length);
    return (static_cast<u8>(flags)&1u)!=0u ? offset+length : 0u;
}

[[nodiscard]] u32 function_358d4(Machine& m,Runtime116Boundary& b,u32 source,u32 flags) noexcept {
    u8 local[0xCC]{};                                    // host backing for encoded stack locals
    store_le32(local+0xC8u,0x00002020u);                // MOV EAX,[35870] / local C8
    u32 total=0u;                                        // EDI

    u32 handle=source;
    if((static_cast<u8>(flags)&1u)==0u) {
        handle=b.call_37e77?b.call_37e77(source,0x200u):0u;
        if(handle==0xFFFFFFFFu)return 0u;
    }

    (void)app116_read(m,b,handle,0x3Cu,flags,local+0xC4u,4u);
    const u32 table_offset=load_le32(local+0xC4u);
    (void)app116_read(m,b,handle,table_offset,flags,local+0xC8u,2u);
    const s32 signature_cmp=b.call_27c70_stack?b.call_27c70_stack(m,local+0xC8u,0x1604u):0;
    if(signature_cmp!=0) {
        if(b.call_380f8)(void)b.call_380f8(handle);      // encoded even for flag-bit-0 direct mode
        return 0u;
    }

    (void)app116_read(m,b,handle,table_offset,flags,local,0xACu);
    u32 offset=table_offset+load_le32(local+0x40u);
    const u32 count=load_le32(local+0x44u);
    for(u32 index=0u; index<count; ++index) {
        offset=app116_read(m,b,handle,offset,flags,local+0xACu,0x18u);
        total+=load_le32(local+0xACu);
    }

    if((static_cast<u8>(flags)&1u)==0u && b.call_380f8)
        (void)b.call_380f8(handle);
    return total+0x0Fu;
}
static_assert(0x35A09u-0x358D4u==309u);
// 35A09..35A0B is LEA EAX,[EAX+0] alignment before independent 35A0C.
static_assert(0x35A0Cu-0x35A09u==3u);
