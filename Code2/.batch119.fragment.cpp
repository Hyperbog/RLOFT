// Batch 119: verified CODE.2 39526..3967B runtime helpers.
struct Runtime119Boundary {
    u32 (*call_3a165)()=nullptr;
    u32 (*alloca_guest)(u32)=nullptr;
    void (*call_3a38a)()=nullptr;
    u32 (*call_20608)(u32,u32)=nullptr;
    void (*call_352be)(u32)=nullptr;
    void (*call_guest)(u32,u32)=nullptr;
};

s32 function_39526(Machine& m,u32 lhs,u32 rhs,u32 count) noexcept {
    while(count!=0u){
        u8 a=*dg(m,lhs), b=*dg(m,rhs);
        if(a>=0x41u&&a<=0x5Au)a=static_cast<u8>(a+0x20u);
        if(b>=0x41u&&b<=0x5Au)b=static_cast<u8>(b+0x20u);
        if(a!=b)return static_cast<s32>(static_cast<u32>(a)-static_cast<u32>(b));
        if(b==0u)return 0;
        ++lhs;++rhs;--count;
    }
    return 0;
}
static_assert(0x39589u-0x39526u==99u);

void function_39589(Machine& m,Runtime119Boundary& b) noexcept {
    const u32 raw=load_le32(dg(m,0x1264Cu));
    const u32 rounded=(raw+3u)&0xFFFFFFFCu;
    const u32 available=b.call_3a165?b.call_3a165():0u;
    u32 base=0u;
    if(rounded<available)base=b.alloca_guest?b.alloca_guest(rounded):0u;
    store_le32(dg(m,0x12650u),base+load_le32(dg(m,0x1264Cu)));
    if(b.call_3a38a)b.call_3a38a();
    const u32 arg0=load_le32(dg(m,0x2BD00u));
    const u32 arg1=load_le32(dg(m,0x2BD04u));
    const u32 value=b.call_20608?b.call_20608(arg0,arg1):0u;
    if(b.call_352be)b.call_352be(value);
}
static_assert(0x395E2u-0x39589u==89u);

void function_395e2(Machine& m,Runtime119Boundary& b,u32 edx) noexcept {
    for(;;){
        u32 best=0x12738u;
        u8 limit=static_cast<u8>(edx);
        for(u32 p=0x12720u;p<0x12738u;p+=6u){
            if(*dg(m,p)!=2u && *dg(m,p+1u)<=limit){best=p;limit=*dg(m,p+1u);}
        }
        if(best==0x12738u)return;
        const u32 callback=load_le32(dg(m,best+2u));
        if(callback!=0u&&b.call_guest)b.call_guest(callback,edx);
        *dg(m,best)=2u;
    }
}
static_assert(0x3962Du-0x395E2u==75u);

void function_3962d(Machine& m,Runtime119Boundary& b,u8 al,u8 dl) noexcept {
    const u8 ceiling=dl;
    const u8 initial=al;
    for(;;){
        u32 best=0x1273Eu;
        u8 selected=initial;
        for(u32 p=0x12738u;p<0x1273Eu;p+=6u){
            if(*dg(m,p)!=2u && selected<=*dg(m,p+1u)){best=p;selected=*dg(m,p+1u);}
        }
        if(best==0x1273Eu)return;
        if(selected<=ceiling){
            const u32 callback=load_le32(dg(m,best+2u));
            if(callback!=0u&&b.call_guest)b.call_guest(callback,(static_cast<u32>(ceiling)<<8)|initial);
        }
        *dg(m,best)=2u;
    }
}
static_assert(0x3967Cu-0x3962Du==79u);
