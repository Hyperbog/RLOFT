// -----------------------------------------------------------------------------
// Batch 29: 15E38..16A84 choice presentation/timing, party-panel rendering,
// cinematic mode execution, record conversion, and early resource-table loads.
// 16400 owns the backward blocks at 15F7E..16033; 163C4 falls through into it.
// Compiler separators and the bounded zero padding at 16806..1680F are listed
// as non-code/data in RESIDUAL_LEDGER.md.
// -----------------------------------------------------------------------------

struct Application29Boundary {
    Application28Boundary* app28=nullptr;
    void (*call_1a5b0)(u32,s32,u32)=nullptr;
    s32 (*call_1f3fc)(u32)=nullptr;
    void (*call_298f0)(u32,u32,u32,u32,u32,u32)=nullptr;
    void (*divide_error)()=nullptr;
    u32 (*call_312ec_local)(u8*,u32,u32)=nullptr;
};

void function_16400(Machine& m,Application29Boundary& b) noexcept;

void function_15e38(Machine& m,Application29Boundary& b,u32 choice) noexcept {
    if(!b.app28||!b.app28->app25)return;
    Application25Boundary& a25=*b.app28->app25;
    Application26Boundary* a26=b.app28->app26;

    const u32 random_value=b.app28->call_30ca2_noargs?b.app28->call_30ca2_noargs():0u;
    const s32 limit=static_cast<s32>((random_value*6u)>>15u)+4;
    s32 last=0;
    for(s32 i=0;i<limit;++i) {
        const s32 slot=i%4;
        const u32 actor=static_cast<u32>(slot)*0x248u;
        if((load_le16(dg(m,0x218F0u+actor))&0xD040u)==0u)last=i;
    }
    const s32 selected=last%4;
    if(a25.call_291b0)a25.call_291b0(*dg(m,0xC050u));
    for(s32 i=0;i<4;++i)
        if(i!=selected)store_le32(dg(m,0x7FBCu+static_cast<u32>(i)*4u),1u);

    function_16400(m,b);
    if(a25.call_30d30)a25.call_30d30(0x7FBCu,0u,0x10u);
    if(a25.call_2e510)a25.call_2e510(0xDCu,0u,load_le32(dg(m,0x258CCu))+2u,9u);
    const u32 actor=0x218E0u+static_cast<u32>(selected)*0x248u;
    if(a25.call_30508)a25.call_30508(0xE7u,0u,0x140u,actor,1u);
    if(a26&&a26->call_30354)a26->call_30354(0x3Au,1u);
    if(a26)(void)function_13fd4(m,*a26,0u,0xF6,0x1F,0x0A,0x10A,
                                0x12C20u+(choice<<8u),1u);
    store_le32(dg(m,0x7FFCu),static_cast<u32>(selected));
    if(b.call_1a5b0)b.call_1a5b0(load_le32(dg(m,0xC050u)),
                                  static_cast<s16>(load_le16(dg(m,0x8600u))),
                                  *dg(m,0x8602u));
    *dg(m,0xC050u)^=1u;
    if(a25.call_291b0)a25.call_291b0(*dg(m,0xC050u));
}
static_assert(0x15F7Eu-0x15E38u==326u);

// 15F7E..16033 is the backward continuation of function_16400.
static_assert(0x16034u-0x15F7Eu==182u);

void function_16034(Machine& m,Application29Boundary& b) noexcept {
    if(!b.app28||!b.app28->app25)return;
    Application25Boundary& a25=*b.app28->app25;
    for(u32 off=0u;off<0x3Cu;off+=0x0Cu) {
        const u32 target=load_le32(dg(m,0x14024u+off));
        if(target==0xFFFFFFFFu)continue;
        const u32 now=a25.call_30700?a25.call_30700():0u;
        if(now<=load_le32(dg(m,0x1402Cu+off)))continue;
        const u32 saved=load_le32(dg(m,0x7FECu));
        store_le32(dg(m,0x7FECu),target);
        while(function_15534(m,*b.app28)==0u){}
        store_le32(dg(m,0x7FECu),saved);
        const u32 next=a25.call_30700?a25.call_30700():0u;
        store_le32(dg(m,0x1402Cu+off),next+load_le32(dg(m,0x14028u+off)));
    }
}
static_assert(0x1608Au-0x16034u==86u);

void function_1608c(Machine& m,Application29Boundary& b) noexcept {
    if(!b.app28||!b.app28->app25||!b.app28->app26)return;
    Application25Boundary& a25=*b.app28->app25;
    Application26Boundary& a26=*b.app28->app26;
    function_141b8(m,a26);
    function_14218(m,a26,-1,1u);
    if(b.call_1a5b0)b.call_1a5b0(load_le32(dg(m,0xC050u)),
                                  static_cast<s16>(load_le16(dg(m,0x8600u))),
                                  *dg(m,0x8602u));
    *dg(m,0xC050u)^=1u;
    if(a25.call_291b0)a25.call_291b0(*dg(m,0xC050u));
    *dg(m,0x8102u)=1u;

    const s32 sentinel=-1;
    s32 selected=-1;
    for(;;) {
        if(!a25.call_1a1f4)break;
        while(a25.call_1a1f4()!=0u)function_16034(m,b);
        u32 event=0u;
        while((event=a25.call_1a1f4())==0u) {
            function_16034(m,b);
            const s32 y=static_cast<s16>(load_le16(dg(m,0x9E33u)));
            s32 x=static_cast<s16>(load_le16(dg(m,0x9E31u)));
            s32 hover=sentinel;
            // The encoded signed tests on 9E33 are tautological: a value below
            // 17h cannot simultaneously be greater than 129h.
            if(!(y<0x17&&y>0x129)&&x>=0xDD) {
                const s32 lower=0x12A-(4-static_cast<s32>(load_le32(dg(m,0x7FE8u))))*0x16;
                if(x<lower) {
                    const s32 q=(x-0xDD)/0x16;
                    if(q>=0&&q<=3&&load_le16(dg(m,0x7FD8u+static_cast<u32>(q)*4u))!=0xFFFFu)
                        hover=q;
                }
            }
            if(hover==selected)continue;
            function_14218(m,a26,hover,1u);
            if(a25.call_1a8c8)a25.call_1a8c8(load_le32(dg(m,0xC050u)),0xDCu,0u,0x50u,0x140u);
            selected=hover;
        }
        if((event&2u)!=0u)break;
        if(selected<0||selected>=static_cast<s32>(load_le32(dg(m,0x7FE8u))))continue;

        function_15e38(m,b,static_cast<u32>(selected));
        store_le32(dg(m,0x7FECu),load_le16(dg(m,0x7FDAu+static_cast<u32>(selected)*4u)));
        while(function_15534(m,*b.app28)==0u){}
        if(*dg(m,0x8101u)!=0u||*dg(m,0x8100u)!=0u)break;
        function_16400(m,b);
        function_14218(m,a26,sentinel,1u);
        if(b.call_1a5b0)b.call_1a5b0(load_le32(dg(m,0xC050u)),
                                      static_cast<s16>(load_le16(dg(m,0x8600u))),
                                      *dg(m,0x8602u));
        store_le32(dg(m,0xC050u),load_le32(dg(m,0xC050u))^1u);
        selected=sentinel;
        if(a25.call_291b0)a25.call_291b0(*dg(m,0xC050u));
    }
    if(a25.call_22d70)a25.call_22d70();
}
static_assert(0x1624Bu-0x1608Cu==447u);

void function_1624c(Machine& m,Application29Boundary& b) noexcept {
    if(!b.app28||!b.app28->app25)return;
    Application25Boundary& a25=*b.app28->app25;
    s32 index=0;
    u32 off=0u;
    for(;;) {
        const s32 count=static_cast<s16>(load_le16(dg(m,0x258A6u)));
        if(index>=count)return;
        if(load_le16(dg(m,0x22246u+off))==0x4Eu) {
            static constexpr u32 items[5]={0x52u,0x5Cu,0x5Fu,0x66u,0x69u};
            for(u32 item:items)if(a25.call_2195c)
                a25.call_2195c(0x80u,item,0u,load_le32(dg(m,0x2223Cu+off)),
                                load_le32(dg(m,0x22238u+off)),0u,0u);
            const u32 bytes=static_cast<u32>(count-index+1)*0x16u;
            if(a25.call_30cd4)a25.call_30cd4(0x22238u+off,
                                             0x22238u+static_cast<u32>(index+1)*0x16u,
                                             bytes);
            return;
        }
        ++index;
        off+=0x16u;
    }
}
static_assert(0x1635Cu-0x1624Cu==272u);

void function_1635c(Machine& m,Application29Boundary& b) noexcept {
    if(!b.app28)return;
    u32 result=0u;
    do { result=function_15534(m,*b.app28); } while(result!=2u&&result!=3u);
    if(result!=3u) {
        const u32 saved=load_le32(dg(m,0x7FECu));
        if(b.app28->app27)function_15320(m,*b.app28->app27);
        function_1608c(m,b);
        store_le32(dg(m,0x7FECu),saved);
        do { result=function_15534(m,*b.app28); } while(result!=3u);
    }
    if(load_le32(dg(m,0x7FD4u))==0x13u&&b.app28->app26&&
       b.app28->app26->call_1c1dc&&b.app28->app26->call_1c1dc(0x2Eu)==1u)
        function_1624c(m,b);
}
static_assert(0x163C4u-0x1635Cu==104u);

void function_163c4(Machine& m,Application29Boundary& b) noexcept {
    if(!b.app28||!b.app28->app25)return;
    Application25Boundary& a25=*b.app28->app25;
    if(a25.call_291b0)a25.call_291b0(*dg(m,0xC050u));
    if(a25.call_2e510) {
        a25.call_2e510(0u,0u,load_le32(dg(m,0x258CCu)),9u);
        a25.call_2e510(0xC8u,0u,load_le32(dg(m,0x258CCu))+1u,9u);
    }
    function_16400(m,b);
}
static_assert(0x16400u-0x163C4u==60u);

static s32 app29_sar1(s32 v) noexcept {
    return v/2-((v<0&&(v%2)!=0)?1:0);
}

void function_16400(Machine& m,Application29Boundary& b) noexcept {
    if(!b.app28||!b.app28->app25)return;
    Application25Boundary& a25=*b.app28->app25;
    if(a25.call_291b0)a25.call_291b0(*dg(m,0xC050u));
    s32 ybase=0x0D;
    for(u32 slot=0u;slot<4u;++slot,ybase+=0x50) {
        const u32 actor=slot*0x248u;
        const u16 state=load_le16(dg(m,0x218F0u+actor));
        if(state>=0x8000u)continue;
        s32 graphic=static_cast<s32>(load_le32(dg(m,0x258E4u)))+
                    static_cast<s8>(*dg(m,0x218F2u+actor))+
                    static_cast<s32>(*dg(m,0x218F3u+actor))*0x17;
        if((state&0x0100u)!=0u)graphic+=0x2E;
        s32 portrait=b.call_1f3fc?b.call_1f3fc(slot):0;
        if(load_le32(dg(m,0x7FBCu+slot*4u))!=0u)portrait=0;
        const s32 y=ybase+(slot>=2u?1:0);
        if(b.call_298f0)b.call_298f0(0u,0x136u,static_cast<u32>(y),
                                     static_cast<u32>(graphic),1u,
                                     0xCF70u+(static_cast<u32>(portrait)<<8u));
        if((state&0x4000u)!=0u)continue;
        if(a25.call_29b40)a25.call_29b40(0x136u,static_cast<u32>(y+0x30),0x50u,6u,0u);
        const s32 maxhp=static_cast<s16>(load_le16(dg(m,0x218FEu+actor)));
        const s32 half=app29_sar1(maxhp);
        const u32 wrapped=static_cast<u32>(function_0ef14(m,slot))*0x50u+static_cast<u32>(half);
        const s32 numerator=static_cast<s32>(wrapped);
        if(maxhp==0||(numerator==static_cast<s32>(0x80000000u)&&maxhp==-1)) {
            if(b.divide_error)b.divide_error();
            return;
        }
        const s32 amount=numerator/maxhp;
        const s32 bary=y+0x31;
        if(a25.call_29b40) {
            if(amount<=0x50) {
                const s32 x=0x136+(0x50-amount);
                const u32 color=amount>0x14?0x43u:0x57u;
                a25.call_29b40(static_cast<u32>(x),static_cast<u32>(bary),
                                static_cast<u32>(amount),4u,color);
            } else {
                a25.call_29b40(0x136u,static_cast<u32>(bary),0x50u,4u,0x1Bu);
            }
        }
    }
    if(a25.call_2975c)a25.call_2975c(0u,0x136u,0x8Eu,load_le32(dg(m,0x258CCu))+6u,9u);
    u32 y=0u;
    for(u32 slot=0u;slot<4u;++slot,y+=0x50u) {
        const u32 actor=slot*0x248u;
        if((load_le16(dg(m,0x218F0u+actor))&0x8000u)==0u)continue;
        if(a25.call_2e510)a25.call_2e510(0x134u,y+slot/2u+0x0Bu,
                                         load_le32(dg(m,0x258CCu))+4u,9u);
    }
}
static_assert(0x16524u-0x16400u==292u);

void function_16524(Machine& m,Application29Boundary& b,u32 mode) noexcept {
    if(!b.app28||!b.app28->app25||!b.app28->app26)return;
    Application25Boundary& a25=*b.app28->app25;
    Application26Boundary& a26=*b.app28->app26;
    s32 saved50=-1;
    s32 saved48=-1;
    u32 savedA0=0u;
    u32 saved98=0u;
    if(mode!=0x1Cu) {
        saved50=a26.call_29050?a26.call_29050():-1;
        saved48=static_cast<s32>(a26.call_29048?a26.call_29048():0u);
        savedA0=a26.call_290a0?a26.call_290a0():0u;
        saved98=a26.call_29098?a26.call_29098():0u;
        if(a25.call_28dcc)a25.call_28dcc(0x9C4u);
    }
    const u8 savedBF38=*dg(m,0xBF38u);
    *dg(m,0xBF38u)=0u;
    const u32 saved26=a26.call_1c1dc?a26.call_1c1dc(0x26u):0u;
    if(a25.call_19ccc)a25.call_19ccc(0u);
    *dg(m,0x8100u)=0u;
    store_le32(dg(m,0x7FF0u),0u);
    store_le32(dg(m,0x7FF8u),0u);
    *dg(m,0x8102u)=0u;
    store_le32(dg(m,0x7FD0u),0u);
    *dg(m,0x8101u)=0u;
    *dg(m,0x7FB8u)=0u;
    store_le16(dg(m,0xA6BAu),1u);
    const u8 savedD680=*dg(m,0xD680u);
    *dg(m,0xD680u)=2u;
    if(b.app28->call_296c0)b.app28->call_296c0(0u,0u,0x18Fu,0x13Fu);
    store_le16(dg(m,0xA6BAu),0u);
    store_le16(dg(m,0xA6BEu),0u);
    store_le16(dg(m,0xA6BCu),0u);

    for(u32 off=0u;off<0x3Cu;off+=0x0Cu)store_le32(dg(m,0x14024u+off),0xFFFFFFFFu);
    for(u32 i=0u;i<0x100u;++i)*dg(m,0x8000u+i)=static_cast<u8>(i);
    for(u32 off=0u;off<0x10u;off+=4u)store_le16(dg(m,0x7FD8u+off),0xFFFFu);
    *dg(m,0x801Bu)=0x45u;
    store_le32(dg(m,0x7FD4u),mode);
    function_13f70(m,a26,mode);
    if(a25.call_291b0)a25.call_291b0(*dg(m,0xC050u));
    if(a25.call_2e510) {
        a25.call_2e510(0u,0u,load_le32(dg(m,0x258CCu)),9u);
        a25.call_2e510(0xC8u,0u,load_le32(dg(m,0x258CCu))+1u,9u);
    }
    function_16400(m,b);
    function_1635c(m,b);

    if(mode==0x1Fu&&a26.call_1c1dc&&a26.call_1c1dc(0x3Fu)!=0u) {
        const s32 index=static_cast<s16>(load_le16(dg(m,0x25619u)));
        const u32 addr=static_cast<u32>(static_cast<s32>(0x19972)+index*0xE2);
        *dg(m,addr)=static_cast<u8>(*dg(m,addr)&0xC0u);
    }
    if(mode==0x14u&&saved26==0u&&a26.call_1c1dc&&a26.call_1c1dc(0x26u)==1u) {
        store_le16(dg(m,0x255D2u),0x3Cu); store_le16(dg(m,0x255D6u),4u);
        store_le16(dg(m,0x255D4u),0x8000u); store_le16(dg(m,0x255D0u),0x8000u);
        store_le32(dg(m,0xC658u),0xB4u);
        saved50=-1;
        if(a26.call_181b4)a26.call_181b4(6u);
    }
    if(mode==0x12u&&a26.call_1c1dc&&a26.call_1c1dc(0x2Fu)==0u)
        store_le32(dg(m,0xBF44u),0x16u);
    if(mode==0x20u&&a26.call_1c1dc&&a26.call_1c1dc(0x4Eu)==1u) {
        if(b.app28->app27&&b.app28->app27->call_1c160)
            b.app28->app27->call_1c160(0x4Eu,mode); // live EBX supplies the second argument.
        if(a26.call_1c1dc(0x4Fu)==1u)store_le32(dg(m,0xBF44u),0x4Bu);
    }
    if(mode!=0x1Cu&&saved50!=-1&&saved48!=-1) {
        if(a25.call_28dcc)a25.call_28dcc(0x9C4u);
        if(a25.call_28ef0)a25.call_28ef0();
        if(a25.call_28f58)a25.call_28f58(savedA0);
        if(a25.call_29008)a25.call_29008(saved98);
        if(a25.call_28e58)a25.call_28e58(saved50,saved48,0x9C4u);
    }
    if(mode==0x25u&&b.app28->app27&&function_14a60(m,*b.app28->app27,0x6C8u)!=-1)
        store_le32(dg(m,0xBF44u),0x69u);
    if(a25.call_1f8fc)a25.call_1f8fc();
    if(a25.call_22d70)a25.call_22d70();
    *dg(m,0xBF38u)=savedBF38;
    *dg(m,0xD680u)=savedD680;
    if(a25.call_19cf4)a25.call_19cf4();
}
static_assert(0x16806u-0x16524u==738u);

void function_16810(Machine& m,Application29Boundary& b) noexcept {
    if(!b.app28||!b.app28->app25||!b.app28->app26)return;
    Application25Boundary& a25=*b.app28->app25;
    Application26Boundary& a26=*b.app28->app26;
    const u32 handle=static_cast<u32>(a26.call_312a0?a26.call_312a0(0x6F8u):-1);
    if(a25.call_312ec)a25.call_312ec(0x258F4u,4u,handle);
    const u32 count=load_le32(dg(m,0x258F4u));
    if(count==0u)return; // The encoded zero-count path does not close the handle.
    const u32 bytes=count*0x45u;
    const u32 ptr=a26.call_3104c?a26.call_3104c(bytes):0u;
    store_le32(dg(m,0x22230u),ptr);
    if(a25.call_312ec)a25.call_312ec(ptr,bytes,handle);
    if(a26.call_312dc)a26.call_312dc(handle);
}
static_assert(0x1686Bu-0x16810u==91u);

void function_1686c(Machine& m,Application29Boundary& b,u32 handle) noexcept {
    if(!b.app28||!b.app28->app25||!b.app28->app26)return;
    Application25Boundary& a25=*b.app28->app25;
    Application26Boundary& a26=*b.app28->app26;
    if(a25.call_312ec)a25.call_312ec(0x258E0u,2u,handle);
    const u32 count=load_le32(dg(m,0x258E0u)); // high word is intentionally whatever was already present.
    if(count==0u)return;
    const u32 bytes=count*7u;
    const u32 ptr=a26.call_3104c?a26.call_3104c(bytes):0u;
    store_le32(dg(m,0x255DCu),ptr);
    if(a25.call_312ec)a25.call_312ec(ptr,bytes,handle);
}
static_assert(0x168C7u-0x1686Cu==91u);

void function_168c8(Machine& m,Application29Boundary& b) noexcept {
    if(!b.app28||!b.app28->app25||!b.app28->app26)return;
    Application25Boundary& a25=*b.app28->app25;
    Application26Boundary& a26=*b.app28->app26;
    const u32 handle=static_cast<u32>(a26.call_312a0?a26.call_312a0(0x704u):-1);
    if(a25.call_312ec)a25.call_312ec(0x258E8u,4u,handle);
    const u32 count=load_le32(dg(m,0x258E8u));
    if(count==0u)return; // The encoded zero-count path also leaves this handle open.
    const u32 bytes=count*0x41u;
    const u32 ptr=a26.call_3104c?a26.call_3104c(bytes):0u;
    store_le32(dg(m,0x255D8u),ptr);
    if(a25.call_312ec)a25.call_312ec(ptr,bytes,handle);
    if(a26.call_312dc)a26.call_312dc(handle);
}
static_assert(0x16931u-0x168C8u==105u);

void function_16934(Machine& m,Application29Boundary& b,u32 handle) noexcept {
    if(!b.app28||!b.app28->app25||!b.app28->app26)return;
    Application25Boundary& a25=*b.app28->app25;
    Application26Boundary& a26=*b.app28->app26;
    if(a25.call_312ec)a25.call_312ec(0x258F0u,2u,handle);
    const u32 count=load_le32(dg(m,0x258F0u)); // two-byte read, full-dword use is intentional.
    if(count==0u)return;
    const u32 bytes=count*7u;
    const u32 ptr=a26.call_3104c?a26.call_3104c(bytes):0u;
    store_le32(dg(m,0x22234u),ptr);
    if(a25.call_312ec)a25.call_312ec(ptr,bytes,handle);
}
static_assert(0x1698Fu-0x16934u==91u);

void function_16990(Machine& m,Application29Boundary& b,u32 handle) noexcept {
    if(!b.app28||!b.app28->app25||!b.app28->app26)return;
    Application25Boundary& a25=*b.app28->app25;
    Application26Boundary& a26=*b.app28->app26;
    u8 count_bytes[2]={0u,0u};
    if(b.call_312ec_local)b.call_312ec_local(count_bytes,2u,handle);
    const s32 count=static_cast<s16>(load_le16(count_bytes));
    if(count==0)return;
    const u32 table=a26.call_3104c?a26.call_3104c(static_cast<u32>(count)*4u):0u;
    store_le32(dg(m,0x2B764u),table);
    for(s32 i=0;i<count;++i) {
        const u32 index=static_cast<u32>(i);
        store_le16(dg(m,0x2B750u+index*2u),0u);
        if(a25.call_312ec)a25.call_312ec(0x2B750u+index*2u,2u,handle);
        u8 nested_bytes[4]={0u,0u,0u,0u}; // assembly zeroes the containing dword before the 2-byte read.
        if(b.call_312ec_local)b.call_312ec_local(nested_bytes,2u,handle);
        const u32 nested=load_le32(nested_bytes);
        const u32 bytes=nested*8u;
        const u32 ptr=a26.call_3104c?a26.call_3104c(bytes):0u;
        store_le32(dg(m,table+index*4u),ptr);
        if(a25.call_312ec)a25.call_312ec(ptr,bytes,handle);
    }
}
static_assert(0x16A57u-0x16990u==199u);

[[nodiscard]] u32 function_16a58(Application29Boundary& b,u32 index,u32 handle) noexcept {
    if(!b.app28||!b.app28->app25)return handle;
    Application25Boundary& a25=*b.app28->app25;
    if(a25.call_3131c)(void)a25.call_3131c(index*2u,0u,handle);
    u8 alias[4]={static_cast<u8>(handle),static_cast<u8>(handle>>8u),
                 static_cast<u8>(handle>>16u),static_cast<u8>(handle>>24u)};
    // The original reads the word directly into the stack slot containing the
    // handle argument, so only its low 16 bits are replaced before EAX reload.
    if(b.call_312ec_local)b.call_312ec_local(alias,2u,handle);
    return load_le32(alias);
}
static_assert(0x16A85u-0x16A58u==45u);
