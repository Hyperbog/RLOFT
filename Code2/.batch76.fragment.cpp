// Batch 76: 31E74..31F90 active-party shade helper and paired clamp/setters.
[[nodiscard]] u32 function_31e74(Machine& m) noexcept {
    u32 result=0u;
    for(s32 slot=0;slot<4;++slot){
        const u32 off=static_cast<u32>(slot)*0x248u;
        const u32 flags=load_le16(dg(m,0x218F0u+off));
        if(static_cast<s32>(flags)>=0x4000)continue;
        if((load_le16(dg(m,0x21A12u+off))&0x0402u)!=0u)result=8u;
    }
    return result;
}
static_assert(0x31EDCu-0x31E74u==104u);

void function_31edc(Machine& m,s32 value) noexcept {
    store_le32(dg(m,0xF920u),static_cast<u32>(value));
    if(static_cast<s32>(load_le32(dg(m,0xF920u)))>10)store_le32(dg(m,0xF920u),10u);
    if(static_cast<s32>(load_le32(dg(m,0xF920u)))<0)store_le32(dg(m,0xF920u),0u);
    const s32 fade=app74_wrap_add(static_cast<s32>(function_31e74(m)),static_cast<s32>(load_le32(dg(m,0xF91Cu))));
    function_31a14(m,static_cast<s32>(load_le32(dg(m,0xF920u))),fade);
}
static_assert(0x31F35u-0x31EDCu==89u);
// 31F35..31F37 is LEA alignment.

void function_31f38(Machine& m,s32 value) noexcept {
    store_le32(dg(m,0xF91Cu),static_cast<u32>(value));
    if(static_cast<s32>(load_le32(dg(m,0xF91Cu)))>8)store_le32(dg(m,0xF91Cu),8u);
    if(static_cast<s32>(load_le32(dg(m,0xF91Cu)))<0)store_le32(dg(m,0xF91Cu),0u);
    const s32 fade=app74_wrap_add(static_cast<s32>(function_31e74(m)),static_cast<s32>(load_le32(dg(m,0xF91Cu))));
    function_31a14(m,static_cast<s32>(load_le32(dg(m,0xF920u))),fade);
}
static_assert(0x31F91u-0x31F38u==89u);
// 31F91..31F93 is LEA alignment. 31F94 begins the next independent routine.
