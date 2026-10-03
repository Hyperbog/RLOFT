    if(height-1>row) {
        const u8 v=*dg(m,cell+static_cast<u32>(width)+0x1760u);
        if(v<0x20u)low_mask+=4u;
        if(v==0x2Fu||v==0x2Eu)slash_mask+=4u;
    }
    const u8 kind=*dg(m,cell+0x1760u);
    if(kind==1u)return load_le32(dg(m,0x5FF0u+low_mask*4u));
    if(kind==2u) {
        if(low_mask==6u)return 0x29u;
        if(low_mask==9u)return 0x2Bu;
        return low_mask;
    }
    if(kind==3u) {
        if(slash_mask==1u)return 0x21u;
        if(slash_mask==2u)return 0x23u;
        if(slash_mask==4u)return 0x22u;
        if(slash_mask==8u)return 0x20u;
        if((slash_mask&9u)!=0u)return 0x20u;
        if((slash_mask&6u)!=0u)return 0x22u;
        return low_mask;
    }
    if(kind==4u) {
        if(low_mask==6u)return 0x28u;
        if(low_mask==9u)return 0x2Au;
        return low_mask;
    }
    if(kind==5u) {
        if((slash_mask&6u)==6u||(slash_mask&9u)==9u)return 0x33u;
        return load_le32(dg(m,0x5FF0u+low_mask*4u));
    }
    return low_mask;
}
static_assert(0x00B6Cu-0x009B0u==444u);

// 00B6C..00BF9: derive a tile graphic code, temporarily install its two
// palette bytes at 764D/7651, draw it through 2975C, then restore B1h/B5h.
void function_00b6c(Machine& m,Application03Boundary& b,s32 col,s32 row,s32 draw0,s32 draw1) noexcept {
    const s32 width=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12756u))));
    const u32 cell=static_cast<u32>(col)+static_cast<u32>(width)*static_cast<u32>(row);
    u32 code=*dg(m,cell+0x1760u);
    if(code==0xFFu)return;
    if(code<0x20u)code=function_009b0(m,cell,col,row);
    const u8 palette_index=static_cast<u8>(load_le16(dg(m,0x198E0u+cell*2u)))&0x3Fu;
    *dg(m,0x764Du)=*dg(m,0x25822u+static_cast<u32>(palette_index)*2u);
    *dg(m,0x7651u)=*dg(m,0x25823u+static_cast<u32>(palette_index)*2u);
    if(b.call_2975c)b.call_2975c(code,static_cast<u32>(draw0),static_cast<u32>(draw1),load_le32(dg(m,0x258DCu))+0x34u,0u);
    *dg(m,0x764Du)=0xB1u;
    *dg(m,0x7651u)=0xB5u;
}
static_assert(0x00BFAu-0x00B6Cu==142u);

// 00BFC..00CAF: draw marker 34h, obtain two fixed-point deltas from 2DF2C,
// advance both accumulators eight times, then draw a line to the signed-word
// endpoints when the encoded lower/upper-bound tests pass.
void function_00bfc(Machine& m,Application03Boundary& b,Graphics2Boundary& g,s32 a0,s32 a1) noexcept {
    if(b.call_2975c)b.call_2975c(0x34u,static_cast<u32>(a0)-2u,static_cast<u32>(a1),load_le32(dg(m,0x258DCu))+0x34u,0u);
    u32 step0=0u,step4=0u;
    if(b.call_2df2c)b.call_2df2c(&step4,&step0,0x10000u,load_le32(dg(m,0xC658u)));
    u32 acc8=static_cast<u32>(static_cast<u16>(a1))<<16;
    u32 accc=static_cast<u32>(static_cast<u16>(a0))<<16;
    for(u32 i=0;i<8u;++i){accc+=step4;acc8+=step0;}
    const s32 end0=static_cast<s32>(static_cast<s16>(accc>>16));
    const s32 end1=static_cast<s32>(static_cast<s16>(acc8>>16));
    if(end0<0||end1<0||end0>0x18F)return;
    function_29cec(m,g,a0,a1,end0,end1,0xFFu);
}
static_assert(0x00CB0u-0x00BFCu==180u);

static s32 application03_div3333(u16 v) noexcept { return static_cast<s32>(v)/0x3333; }
static s32 application03_div2000(u16 v) noexcept { return static_cast<s32>(v)/0x2000; }
static s32 application03_add_wrap(s32 a,s32 b) noexcept { return static_cast<s32>(static_cast<u32>(a)+static_cast<u32>(b)); }

// 00CB0..010C2: render all record families occupying one map coordinate. The
// four search families retain distinct sprite codes and record layouts; every
// 2975C call reloads 258DC, matching the binary across opaque draw calls.
void function_00cb0(Machine& m,Application03Boundary& b,Graphics2Boundary& g,s32 key6,s32 key2,s32 base_coord,s32 add_coord) noexcept {
    const s32 width=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12756u))));
    const u32 cell=static_cast<u32>(key6)+static_cast<u32>(width)*static_cast<u32>(key2);
    const s32 base_minus1=static_cast<s32>(static_cast<u32>(base_coord)-1u);
    if(*dg(m,cell+0x1760u)==0xFFu)return;
    auto draw=[&](u32 sprite,s32 p0,s32 p1) noexcept {
        if(b.call_2975c)b.call_2975c(sprite,static_cast<u32>(p0),static_cast<u32>(p1),load_le32(dg(m,0x258DCu))+0x34u,0u);
    };
    s32 i=function_00700(m,key6,key2,0);
    while(i!=-1) {
        const u32 off=static_cast<u32>(i)*0x16u;
        u32 sprite=0x38u;
        s32 p0=0,p1=0;
        if(*dg(m,0x5FCBu)!=0u) {
            const u32 type=load_le16(dg(m,0x22246u+off));
            if(type>=0x3Cu&&type<=0x43u)sprite=0x35u;
            else if(type==0xADu)sprite=0x31u;
            p0=application03_add_wrap(application03_div3333(load_le16(dg(m,0x22238u+off))),add_coord);
            p1=application03_add_wrap(application03_div2000(load_le16(dg(m,0x2223Cu+off))),base_minus1);
        } else {
            const u32 p=load_le32(dg(m,0x6048u))+off;
            const u32 alt_type=load_le16(dg(m,p+0x0Eu));
            if(alt_type>=0x3Cu&&alt_type<=0x43u)sprite=0x35u;
            else if(load_le16(dg(m,0x22246u+off))==0xADu)sprite=0x31u;
            p0=application03_add_wrap(application03_div3333(load_le16(dg(m,p))),add_coord);
            p1=application03_add_wrap(application03_div2000(load_le16(dg(m,p+4u))),base_minus1);
        }
        draw(sprite,p1,p0);
        i=function_00700(m,key6,key2,i+1);
    }
    if(*dg(m,0x5FCBu)!=0u) {
        i=function_00794(m,key6,key2,0);
        while(i!=-1) {
            const u32 off=static_cast<u32>(i)*0x16u;
            const s32 p0=application03_add_wrap(application03_div3333(load_le16(dg(m,0x14D80u+off))),add_coord);
            const s32 p1=application03_add_wrap(application03_div2000(load_le16(dg(m,0x14D84u+off))),base_minus1);
            draw(0x32u,p1,p0);
            i=function_00794(m,key6,key2,i+1);
        }
        i=function_007e4(m,key6,key2,0);
        while(i!=-1) {
            const u32 off=static_cast<u32>(i)*0x16u;
            const s32 p0=application03_add_wrap(application03_div3333(load_le16(dg(m,0x22238u+off))),add_coord);
            const s32 p1=application03_add_wrap(application03_div2000(load_le16(dg(m,0x2223Cu+off))),base_minus1);
            draw(0x30u,p1,p0);
            i=function_007e4(m,key6,key2,i+1);
        }
    }
    i=function_00934(m,b,key6,key2,0);
    while(i!=-1) {
        const u32 off=static_cast<u32>(i)*0x16u;
        const s32 p0=application03_add_wrap(application03_div3333(load_le16(dg(m,0x14D80u+off))),add_coord);
        const s32 p1=application03_add_wrap(application03_div2000(load_le16(dg(m,0x14D84u+off))),base_minus1);
        draw(0x37u,p1,p0);
        i=function_00934(m,b,key6,key2,i+1);
    }
    if(*dg(m,0x5FCBu)!=0u) {
        i=function_0083c(m,b,key6,key2,0);
        while(i!=-1) {
            const u32 off=static_cast<u32>(i)*0x16u;
            const s32 p0=application03_add_wrap(application03_div3333(load_le16(dg(m,0x14D80u+off))),add_coord);
            const s32 p1=application03_add_wrap(application03_div2000(load_le16(dg(m,0x14D84u+off))),base_minus1);
            draw(0x36u,p1,p0);
            i=function_0083c(m,b,key6,key2,i+1);
        }
        if(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))))==key6 &&
           static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))))==key2) {
            const s32 p0=application03_add_wrap(application03_div3333(load_le16(dg(m,0x255D4u))),add_coord);
            const s32 p1=application03_add_wrap(application03_div2000(load_le16(dg(m,0x255D0u))),base_coord);
            function_00bfc(m,b,g,p1,p0);
        }
    }
}
static_assert(0x010C3u-0x00CB0u==1043u);

// 010C4..011A4: fixed frame-decoration draw list. 258DC is deliberately
// reloaded before each opaque 2975C call.
void function_010c4(Machine& m,Application03Boundary& b) noexcept {
    auto draw=[&](u32 a0,u32 a1,u32 a2,u32 offset) noexcept {
        if(b.call_2975c)b.call_2975c(a0,a1,a2,load_le32(dg(m,0x258DCu))+offset,0u);
    };
    draw(0u,0u,0x67u,0x3Cu);
    draw(1u,0x170u,0x67u,0x3Cu);
    draw(0u,0xA4u,0u,0x3Du);
    draw(1u,0xA4u,0xE4u,0x3Du);
    draw(0u,8u,0x75u,0x36u);
    draw(1u,0x17Au,0x75u,0x36u);
    draw(2u,0xC0u,4u,0x36u);
    draw(3u,0xC0u,0xE9u,0x36u);
}
static_assert(0x011A5u-0x010C4u==225u);
