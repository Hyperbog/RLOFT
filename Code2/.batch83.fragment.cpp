// Batch 83: 3263C..32951 VGA/callback renderer with explicit self-modifying-code effects.
struct Application83Boundary {
    Application82Boundary* app82=nullptr;
    void (*out16)(u16,u16)=nullptr;
    u8 (*code_load8)(u32)=nullptr;
    u16 (*code_load16)(u32)=nullptr;
    void (*code_store8)(u32,u8)=nullptr;
    u32 (*call_337c8)(u32,u32)=nullptr;
    u32 (*call_338c8)(u32,u32,u32,u32)=nullptr;
};
static void app83_mask(Application83Boundary& b,u8 mask) noexcept {
    if(b.out16)b.out16(0x3C4u,static_cast<u16>(2u|(static_cast<u16>(mask)<<8)));
}
static s32 app83_mul_idiv128(u32 a,s8 signed_byte) noexcept {
    u64 bits=static_cast<u64>(a)*static_cast<u64>(static_cast<u32>(static_cast<s32>(signed_byte)));
    bits+=0x40u;
    const bool neg=(bits>>63)!=0u;
    const u64 mag=neg?(~bits+1u):bits;
    const u64 q=mag/0x80u;
    return neg?static_cast<s32>(0u-static_cast<u32>(q)):static_cast<s32>(static_cast<u32>(q));
}
static u16 app83_code16(Application83Boundary& b,u32 a) noexcept { return b.code_load16?b.code_load16(a):0u; }
static u8 app83_code8(Application83Boundary& b,u32 a) noexcept { return b.code_load8?b.code_load8(a):0u; }
static void app83_code_store8(Application83Boundary& b,u32 a,u8 v) noexcept { if(b.code_store8)b.code_store8(a,v); }

void function_3263c(Machine& m,Application83Boundary& b) noexcept {
    const u32 scene=load_le32(dg(m,0xC700u));
    if(scene==0x93u)return;

    const u32 phase=load_le32(dg(m,0xC6F8u));
    u32 screen_addr=((0x800u*static_cast<u32>(*dg(m,0xEF4Du))+0xA000u)<<4);
    u32 span=(load_le32(dg(m,0x15A18u+phase*0x1Cu))&0xFFF8u)>>1;
    const u32 shade_table=load_le32(dg(m,0x258C4u));
    u32 shade=load_le16(dg(m,shade_table));

    if(static_cast<s32>(scene)<0x242 || static_cast<s32>(scene)>0x24D){
        shade=load_le16(dg(m,0xFCFAu));
        if(static_cast<s32>(span)<=static_cast<s32>(load_le32(dg(m,0xC730u)))){
            u16 ax=load_le16(dg(m,shade_table+span*2u));
            const u8 sub=*dg(m,0xFCF2u);
            ax=static_cast<u16>(ax-static_cast<u16>(static_cast<u16>(sub)|(static_cast<u16>(sub)<<8)));
            u8 lo=static_cast<u8>(ax),hi=static_cast<u8>(ax>>8);
            if(static_cast<s8>(hi)<0)hi=0u;
            if(static_cast<s8>(lo)<0)lo=0u;
            shade=static_cast<u32>(lo)|(static_cast<u32>(hi)<<8);
        }
        if((phase&1u)!=0u)shade=((shade&0xFFu)<<8)|((shade>>8)&0xFFu);
    }
    if(shade>0x808u)shade=0x808u;

    span&=0xFFFCu;
    u32 callback=load_le32(dg(m,0x18FE0u+span));
    screen_addr+=phase>>2;
    screen_addr+=load_le32(dg(m,0xC6E0u));
    const u32 mask_index=load_le32(dg(m,0xC6F4u))+((phase&3u)<<3);

    if(load_le32(dg(m,0xFCD0u))!=0u){
        const u32 half=span>>1;
        const u32 top=(span>>3)+0x80u;
        if(static_cast<s32>(top-half)<=0)return;
        s32 row_shift=app83_mul_idiv128(span,static_cast<s8>(*dg(m,0x25621u)));
        screen_addr-=static_cast<u32>(row_shift)*0x50u;
        const s32 remaining=static_cast<s32>(top-half-static_cast<u32>(row_shift));
        if(remaining<0)row_shift=static_cast<s32>(static_cast<u32>(row_shift)+static_cast<u32>(remaining));

        callback-=0x204u;
        const s32 patch_index=static_cast<s32>(static_cast<s8>(*dg(m,0x25621u)))+1;
        const u16 patch_offset=app83_code16(b,callback+static_cast<u32>(patch_index*2));
        const u32 patch_addr=callback+patch_offset;
        const u8 saved=app83_code8(b,patch_addr);
        app83_code_store8(b,patch_addr,0xC3u);
        const u32 target=callback+(b.call_337c8?b.call_337c8(callback,static_cast<u32>(row_shift)):0u);

        const u8 m0=*dg(m,mask_index+0xC668u);
        app83_mask(b,m0);
        if(b.app82)(void)function_32620(m,*b.app82,target,screen_addr,shade);
        ++screen_addr;
        const u8 m1=*dg(m,mask_index+0xC688u);
        if(m1!=0u){app83_mask(b,m1);if(b.app82)(void)function_32620(m,*b.app82,target,screen_addr,shade);}
        ++screen_addr;
        const u8 m2=*dg(m,mask_index+0xC6A8u);
        if(m2!=0u){app83_mask(b,m2);if(b.app82)(void)function_32620(m,*b.app82,target,screen_addr,shade);}
        app83_code_store8(b,patch_addr,saved);
        return;
    }

    const u8 m0=*dg(m,mask_index+0xC668u);
    app83_mask(b,m0);
    if(static_cast<s32>(span)>static_cast<s32>(load_le32(dg(m,0xC730u)))){
        if(b.call_338c8)(void)b.call_338c8(shade,span,load_le32(dg(m,0x255E4u)),screen_addr);
        ++screen_addr;
        const u8 m1=*dg(m,mask_index+0xC688u);
        if(m1!=0u){app83_mask(b,m1);if(b.call_338c8)(void)b.call_338c8(shade,span,load_le32(dg(m,0x255E4u)),screen_addr);}
        ++screen_addr;
        const u8 m2=*dg(m,mask_index+0xC6A8u);
        if(m2!=0u){app83_mask(b,m2);if(b.call_338c8)(void)b.call_338c8(shade,span,load_le32(dg(m,0x255E4u)),screen_addr);}
    }else{
        if(b.app82)(void)function_32620(m,*b.app82,callback,screen_addr,shade);
        ++screen_addr;
        const u8 m1=*dg(m,mask_index+0xC688u);
        if(m1!=0u){app83_mask(b,m1);if(b.app82)(void)function_32620(m,*b.app82,callback,screen_addr,shade);}
        ++screen_addr;
        const u8 m2=*dg(m,mask_index+0xC6A8u);
        if(m2!=0u){app83_mask(b,m2);if(b.app82)(void)function_32620(m,*b.app82,callback,screen_addr,shade);}
    }
}
static_assert(0x32952u-0x3263Cu==790u);
// 32952..32953 is XCHG EBX,EBX alignment with no incoming direct control-flow target.
