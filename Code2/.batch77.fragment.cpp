// Batch 77: 31F94..320F7 phase-change view record/shading updater.
void function_31f94(Machine& m) noexcept {
    u32 packed=load_le32(dg(m,0xCF4Cu));
    u16 phase=static_cast<u16>(packed>>16);
    if(phase==0xFFFFu)phase=0x17u;
    phase=static_cast<u16>(phase%0x18u);
    store_le32(dg(m,0xCF4Cu),(static_cast<u32>(phase)<<16)|(packed&0xFFFFu));
    if(static_cast<u32>(phase)==load_le32(dg(m,0xF9CCu)))return;

    const u8 view=*dg(m,0x258A8u);
    if(view!=0xFFu){
        const u32 rec=load_le32(dg(m,load_le32(dg(m,0x2B764u))+static_cast<u32>(view)*4u));
        const s32 shade=static_cast<s16>(load_le16(dg(m,0xF924u+static_cast<u32>(phase)*2u)));
        if(shade<10){
            store_le16(dg(m,rec),0x18u);
            store_le32(dg(m,0xF910u),0xFFFFFFFFu);
        } else if(load_le32(dg(m,0xF910u))==0xFFFFFFFFu){
            function_30cc4(m,function_30700(m));
            const u32 selection=load_le32(dg(m,0xF914u));
            store_le32(dg(m,0xF910u),selection);
            store_le16(dg(m,rec),static_cast<u16>(selection));
            const s32 next=app74_wrap_add(static_cast<s32>(selection),1);
            store_le32(dg(m,0xF914u),static_cast<u32>(next%8));
        }
        store_le16(dg(m,rec+2u),load_le16(dg(m,0xF99Au+static_cast<u32>(phase)*2u)));
        store_le16(dg(m,rec+6u),load_le16(dg(m,0xF96Au+static_cast<u32>(phase)*2u)));
        store_le32(dg(m,0xF920u),static_cast<u32>(shade));
        const s32 fade=app74_wrap_add(static_cast<s32>(function_31e74(m)),static_cast<s32>(load_le32(dg(m,0xF91Cu))));
        function_31a14(m,static_cast<s32>(load_le32(dg(m,0xF920u))),fade);
    }
    store_le32(dg(m,0xF9CCu),static_cast<u32>(phase));
}
static_assert(0x320F8u-0x31F94u==356u);
// 320F8 begins the next independent byte-output callback; no separator bytes.
