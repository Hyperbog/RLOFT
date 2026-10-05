// -----------------------------------------------------------------------------
// Batch 73: 314B0..318B0 perspective-row renderer. All addresses are CODE.2
// object-relative; CODE.2 bytes are controlling evidence.
// -----------------------------------------------------------------------------
struct Application73Boundary {
    Application57Boundary* app57=nullptr;
};

[[nodiscard]] static s32 app73_scaled_x(s32 delta) noexcept {
    const u32 shifted=static_cast<u32>(delta)<<7;
    const s32 numerator=static_cast<s32>(shifted+0x12u);
    return numerator/0x24;
}

// 314B0..318B0: draw one view's row list. A mode-dependent 32-sprite random
// prepass is followed by per-record wrapped horizontal projection. Record zero
// is always processed; later records stop at the first zero word0 sentinel.
void function_314b0(Machine& m,Application73Boundary& b,u32 view) noexcept {
    const s32 key_base=static_cast<s16>(load_le16(dg(m,0x2B750u+view*2u)));
    const u32 rows=load_le32(dg(m,load_le32(dg(m,0x2B764u))+view*4u));

    function_296c0(m,4,0x20,0x83,0x11F);
    function_30cc4(m,load_le32(dg(m,0xC658u)));

    const u32 mode=*dg(m,0xF918u);
    if(mode<=10u){
        s32 set_bias=0;
        if(mode==9u)set_bias=8;
        else if(mode==10u)set_bias=16;
        for(u32 i=0;i<0x20u;++i){
            const u32 random_set=((function_30ca2(m)*7u)>>15)+static_cast<u32>(key_base+set_bias);
            if(b.app57)(void)function_29838(*b.app57,0u,4u,i*8u+0x20u,random_set,8u);
        }
    }

    function_30cc4(m,function_30700(m));

    for(u32 i=0;;++i){
        const u32 rec=rows+i*8u;
        if(i!=0u && load_le16(dg(m,rec))==0u)break;

        s32 shade=static_cast<s32>(*dg(m,0xF918u));
        if(shade>8)shade=8;
        if(i==0u){
            shade=static_cast<s32>(*dg(m,0xF918u))/2;
            if(static_cast<s16>(load_le16(dg(m,rec)))!=0x18)shade=0;
        }

        s32 key=static_cast<s16>(load_le16(dg(m,rec));
        if(key>=8)key+=key_base;
        const s32 y=static_cast<s16>(load_le16(dg(m,rec+6u)));
        const s32 width=static_cast<s16>(load_le16(dg(m,rec+4u)));
        const s32 world_x=static_cast<s16>(load_le16(dg(m,rec+2u)));
        const s32 camera=static_cast<s32>(load_le32(dg(m,0xC658u)));

        auto screen_for=[&](s32 wrap) noexcept {
            const s32 delta=static_cast<s32>(static_cast<u32>(world_x-camera)+static_cast<u32>(wrap));
            return static_cast<s32>(0xA0-app73_scaled_x(delta)-width/2);
        };
        auto visible=[&](s32 x) noexcept { return x<0x120 && x+width>=0x20; };

        s32 x=screen_for(0);
        if(!visible(x)){
            x=screen_for(0x168);
            if(!visible(x))x=screen_for(-0x168);
        }
        if(b.app57)(void)function_2986c(m,*b.app57,0u,static_cast<u32>(y),static_cast<u32>(x),static_cast<u32>(key),8u,static_cast<u32>(static_cast<u8>(shade)));
    }

    function_296c0(m,0,0,0x18F,0x13F);
}
static_assert(0x318B1u-0x314B0u==1025u);
// 318B1..318B3 is LEA alignment with no incoming direct control-flow target.
