// -----------------------------------------------------------------------------
// Batch 09: 06008..06CAC settings-panel drawing/interaction, save serializer,
// and tile/record flag probe. CODE.2 bytes are controlling evidence.
// -----------------------------------------------------------------------------

struct Application09Boundary {
    void (*call_291b0)(u32)=nullptr;
    u32 (*call_30ca2)()=nullptr;
    void (*call_2975c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_2e374)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_3060c)(u32,u32,u32,u32)=nullptr;
    void (*call_305a8)(u32,u32,u32,u32)=nullptr;
    u32 (*call_30070)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_05eec)()=nullptr;
    void (*call_1a510)(u32)=nullptr;
    void (*call_1a9f8)()=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    void (*call_2a380)(u32,u32,u32,u32)=nullptr;
    void (*call_1a8c8)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_325b0)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_29234)(u32)=nullptr;
    void (*call_28098)(u32)=nullptr;
    void (*call_290a8)(u32)=nullptr;
    void (*call_28e58)(u32,u32,u32)=nullptr;
    void (*call_20994)()=nullptr;
    void (*call_30040)(u32)=nullptr;
    void (*call_31380)(u32)=nullptr;
    u32 (*call_312c8)(u32)=nullptr;
    void (*call_312dc)(u32)=nullptr;
    u32 (*call_312b4)(u32)=nullptr;
    u32 (*call_31304)(u32,u32,u32)=nullptr;
    u32 (*call_31304_local)(const u8*,u32,u32)=nullptr;
    u32 (*call_30700)()=nullptr;
    u32 (*call_1e8c8)(s32,s32)=nullptr;
};

static void app09_copy_cstr(Machine& m,u32 dst,u32 src) noexcept {
    for(;;){const u8 v=*dg(m,src++);*dg(m,dst++)=v;if(v==0u)break;}
}
static void app09_append_cstr(Machine& m,u32 dst,u32 src) noexcept {
    while(*dg(m,dst)!=0u)++dst;
    app09_copy_cstr(m,dst,src);
}

void function_06008(Machine& m,Application09Boundary& b) noexcept {
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    const u32 image=load_le32(dg(m,0x258DCu))+0x35u;
    for(u32 y=0u;y<0x190u;y+=0x10u) {
        for(u32 x=0u;x<0x140u;x+=8u) {
            const u32 r=b.call_30ca2?b.call_30ca2():0u;
            if(b.call_2975c)b.call_2975c((r*4u)>>15,y,x,image,0u);
        }
    }
    for(u32 x=8u;x<0x138u;x+=8u) {
        u32 r=b.call_30ca2?b.call_30ca2():0u;
        if(b.call_2975c)b.call_2975c(((r*2u)>>15)+9u,0u,x,image,0u);
        r=b.call_30ca2?b.call_30ca2():0u;
        if(b.call_2975c)b.call_2975c(((r*2u)>>15)+5u,0x180u,x,image,0u);
    }
    for(u32 y=0x10u;y<0x180u;y+=0x10u) {
        u32 r=b.call_30ca2?b.call_30ca2():0u;
        if(b.call_2975c)b.call_2975c(((r*2u)>>15)+0x0Cu,y,0u,image,0u);
        r=b.call_30ca2?b.call_30ca2():0u;
        if(b.call_2975c)b.call_2975c(((r*2u)>>15)+0x0Eu,y,0x138u,image,0u);
    }
    if(b.call_2975c){
        b.call_2975c(8u,0u,0u,image,0u);
        b.call_2975c(0x0Bu,0u,0x138u,image,0u);
        b.call_2975c(4u,0x180u,0u,image,0u);
        b.call_2975c(7u,0x180u,0x138u,image,0u);
    }
    if(b.call_2e374){
        b.call_2e374(0x0Fu,0x78u,0x18u,0x89u,0x3Du);
        b.call_2e374(0xA5u,0x78u,0x18u,0xAFu,0x3Du);
        b.call_2e374(0xFFu,0x78u,0x18u,0xBBu,0x3Du);
    }
    if(b.call_3060c){
        b.call_3060c(0x17u,0x7Cu,load_le32(dg(m,0x60E4u)),1u);
        b.call_3060c(0xADu,0x7Cu,load_le32(dg(m,0x60E4u)),1u);
        b.call_3060c(0x107u,0x7Cu,load_le32(dg(m,0x60E4u)),1u);
    }
    if(b.call_305a8){
        b.call_305a8(0x17u,0xFCu,load_le32(dg(m,0x60E8u)),1u);
        b.call_305a8(0xADu,0x122u,load_le32(dg(m,0x60E8u)),1u);
        b.call_305a8(0x107u,0x12Du,load_le32(dg(m,0x60E8u)),1u);
    }
    u32 y=0x0Fu;
    for(u32 off=4u;off<=0x28u;off+=4u,y+=0x1Eu) {
        const u32 h=b.call_30070?b.call_30070(y,0x78u,0x19u,0xC8u,0xFFu):0u;
        store_le32(dg(m,0x12B2Cu+off),h);
    }
    if(b.call_05eec)b.call_05eec();
    if(b.call_1a510)b.call_1a510(load_le32(dg(m,0xC050u)));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    if(b.call_1a9f8)b.call_1a9f8();
}
static_assert(0x062ABu-0x06008u==675u);

void function_062ac(Machine& m,Application09Boundary& b) noexcept {
    for(;;) {
        if(!b.call_1a1f4||b.call_1a1f4()==0u)return;
        const s32 delta=static_cast<s16>(load_le16(dg(m,0x9E33u)))-0x90;
        s32 index=delta/10;
        if(index<0)index=0;
        if(index>9)index=9;
        if(static_cast<u32>(index)==*dg(m,0xC025u))continue;
        *dg(m,0xC025u)=static_cast<u8>(index);
        if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
        if(b.call_2a380)b.call_2a380(0u,0x0Fu,0x78u,load_le32(dg(m,0x12B30u)));
        if(b.call_2975c)b.call_2975c(1u,0x0Fu,static_cast<u32>(index*10+0x78),load_le32(dg(m,0x258DCu))+0x40u,0u);
        if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0x0Fu,0x78u,0x19u,0xC8u);
    }
}
static_assert(0x06352u-0x062ACu==166u);

void function_06354(Machine& m,Application09Boundary& b) noexcept {
    for(;;) {
        if(!b.call_1a1f4||b.call_1a1f4()==0u)return;
        const s32 delta=static_cast<s16>(load_le16(dg(m,0x9E33u)))-0x90;
        s32 index=delta/10;
        if(index<0)index=0;
        if(index>0x0E)index=0x0E;
        if(static_cast<u32>(index)==*dg(m,0xC02Eu))continue;
        *dg(m,0xC02Eu)=static_cast<u8>(index);
        if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
        if(b.call_2a380)b.call_2a380(0u,0xFFu,0x78u,load_le32(dg(m,0x12B50u)));
        if(b.call_2975c)b.call_2975c(1u,0xFFu,static_cast<u32>(index*10+0x78),load_le32(dg(m,0x258DCu))+0x40u,0u);
        if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0xFFu,0x78u,0x19u,0xC8u);
        const u32 p=load_le32(dg(m,0xF050u));
        *dg(m,p+0x6Cu)=*dg(m,0xC02Eu);
        if(b.call_325b0)b.call_325b0(0xC054u,0u,load_le32(dg(m,0xC034u)),0x390u,0x100u);
        if(b.call_29234)b.call_29234(0xC054u);
    }
}
static_assert(0x06441u-0x06354u==237u);

void function_06444(Machine& m,Application09Boundary& b) noexcept {
    for(;;) {
        if(!b.call_1a1f4||b.call_1a1f4()==0u)break;
        s32 index=static_cast<s16>(load_le16(dg(m,0x9E33u)))-0x90;
        if(index<0)index=0;
        if(index>0x80)index=0x80;
        if(static_cast<u32>(index)==*dg(m,0xC02Au))continue;
        *dg(m,0xC02Au)=static_cast<u8>(index);
        if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
        if(b.call_2a380)b.call_2a380(0u,0xA5u,0x78u,load_le32(dg(m,0x12B44u)));
        if(b.call_2975c)b.call_2975c(1u,0xA5u,static_cast<u32>(index+0x78),load_le32(dg(m,0x258DCu))+0x40u,0u);
        if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0xA5u,0x78u,0x19u,0xC8u);
    }
    const s32 factor=0x80-static_cast<s16>(load_le16(dg(m,0x258B2u)));
    const s32 numer=factor*static_cast<s32>(*dg(m,0xC02Au))+0x40;
    const s32 quotient=numer/0x80;
    *dg(m,0xC02Du)=static_cast<u8>(0x80-quotient);
}
static_assert(0x06520u-0x06444u==220u);

void function_06548(Machine& m,Application09Boundary& b) noexcept {
    function_06008(m,b);
    for(;;) {
        while(b.call_1a1f4&&b.call_1a1f4()!=0u){}
        while(b.call_1a1f4&&b.call_1a1f4()==0u){}
        const u16 buttons=load_le16(dg(m,0x9E35u));
        if((buttons&2u)!=0u)break;
        const s32 y=static_cast<s16>(load_le16(dg(m,0x9E33u)));
        const s32 x=static_cast<s16>(load_le16(dg(m,0x9E31u)));
        if(y<0x78)continue;
        const s32 adjusted=x-0x0F;
        const s32 rem=adjusted%0x1E;
        const s32 action=adjusted/0x1E;
        if(rem>=0x19)continue;
        if(action>=0&&action<=9) {
            switch(action) {
                case 0: function_062ac(m,b);break;
                case 1: if(y<0xA8)*dg(m,0xC026u)^=1u;break;
                case 2: if(y<0xA8)*dg(m,0xC027u)^=1u;break;
                case 3: if(y<0xA8)*dg(m,0xC028u)^=1u;break;
                case 4: if(y<0xA8)*dg(m,0xC029u)^=1u;break;
                case 5: function_06444(m,b);break;
                case 6:
                    if(y<0xA8){*dg(m,0xC02Bu)^=1u;if(b.call_28098)b.call_28098(*dg(m,0xC02Bu));}
                    break;
                case 7:
                    if(y<0xA8){
                        *dg(m,0xC02Cu)^=1u;
                        if(b.call_290a8)b.call_290a8(*dg(m,0xC02Cu));
                        if(*dg(m,0xC02Cu)!=0u&&b.call_28e58)b.call_28e58(*dg(m,0x258B4u),1u,0x9C4u);
                    }
                    break;
                case 8: function_06354(m,b);break;
                case 9: if(y<0xA8&&b.call_20994)b.call_20994();break;
            }
        }
        if(b.call_05eec)b.call_05eec();
        if(b.call_1a510)b.call_1a510(load_le32(dg(m,0xC050u)));
        if(b.call_1a9f8)b.call_1a9f8();
        *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    }
    for(u32 off=0u;off<0x24u;off+=4u)if(b.call_30040)b.call_30040(load_le32(dg(m,0x12B30u+off)));
}
static_assert(0x066E6u-0x06548u==414u);

[[nodiscard]] u32 function_066e8(Machine& m,Application09Boundary& b,u32 slot) noexcept {
    app09_copy_cstr(m,0x12990u,0xC020u);
    app09_append_cstr(m,0x12990u,0x398u);
    app09_append_cstr(m,0x12990u,0x39Cu);
    if(b.call_31380)b.call_31380(0x12990u);
    const u32 probe=b.call_312c8?b.call_312c8(0x12990u):0u;
    if(b.call_312dc)b.call_312dc(probe);
    const u32 h=b.call_312b4?b.call_312b4(0x12990u):0xFFFFFFFFu;
    if(h==0xFFFFFFFFu)return 1u;
    if(slot==0xFFFFFFFFu) {
        for(u32 i=0;i<5u;++i)*dg(m,0x12990u+i)=*dg(m,0x3A8u+i);
    } else {
        app09_copy_cstr(m,0x12990u,0x6461u+slot*0x32u);
    }
    auto write_guest=[&](u32 ptr,u32 n)->bool{return b.call_31304&&b.call_31304(ptr,n,h)==n;};
    auto fail=[&]()->u32{if(b.call_312dc)b.call_312dc(h);return 1u;};
    const struct Spec{u32 ptr,n;} first[]={
        {0x12990u,0x32u},{0x22200u,0x20u},{0xC6C8u,4u},{0x255D0u,4u},{0x255D4u,4u},
        {0xC658u,4u},{0xF03Cu,2u},{0xF03Eu,2u},{0xFD08u,4u},{0x6DAEu,2u}
    };
    for(const auto& q:first)if(!write_guest(q.ptr,q.n))return fail();
    const u32 runtime=b.call_30700?b.call_30700():0u;
    if(!b.call_31304_local||b.call_31304_local(reinterpret_cast<const u8*>(&runtime),4u,h)!=4u)return fail();
    const Spec second[]={
        {0xCF4Cu,4u},{0x8600u,2u},{0x8602u,1u},{0x218E0u,0x920u},{0xC038u,4u}
    };
    for(const auto& q:second)if(!write_guest(q.ptr,q.n))return fail();
    const u32 records=load_le32(dg(m,0xC038u))*0x16u;
    if(!write_guest(0x248ECu,records))return fail();
    if(!write_guest(0xD8A8u,8u))return fail();
    if(!write_guest(0xA7E4u,4u))return fail();
    for(u32 i=0;i<0x3Eu;++i) {
        const u32 d=0x7330u+i*10u;
        const u32 n=static_cast<u32>(*dg(m,d))*2u;
        if(!write_guest(load_le32(dg(m,d+5u)),n))return fail();
        if(!write_guest(d+9u,1u))return fail();
    }
    const Spec tail[]={
        {0xF920u,4u},{0xF91Cu,4u},{0x67ECu,0x0Fu},{0xA808u,0x16D0u},{0x6FA0u,0x10u}
    };
    for(const auto& q:tail)if(!write_guest(q.ptr,q.n))return fail();
    if(b.call_312dc)b.call_312dc(h);
    return 0u;
}
static_assert(0x06C67u-0x066E8u==1407u);

void function_06c68(Machine& m,Application09Boundary& b) noexcept {
    const s32 x=static_cast<s16>(load_le16(dg(m,0x255D2u)));
    const s32 y=static_cast<s16>(load_le16(dg(m,0x255D6u)));
    const u32 p=b.call_1e8c8?b.call_1e8c8(x,y):0u;
    store_le32(dg(m,0xCF6Cu),0u);
    if(p!=0u&&*dg(m,p)==6u&&(*dg(m,p+1u)&4u)!=0u&&*dg(m,p+6u)!=0u)
        store_le32(dg(m,0xCF6Cu),1u);
}
static_assert(0x06CADu-0x06C68u==69u);
