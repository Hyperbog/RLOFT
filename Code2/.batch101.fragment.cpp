// Batch 101: 33C90..33DDC mouse position/range and drain helpers.
struct Runtime101MouseRegs {
    u32 eax=0u,ebx=0u,ecx=0u,edx=0u;
};
struct Runtime101Boundary {
    u32 (*call_2e340)()=nullptr;
    Runtime101MouseRegs (*int33)(u32,u32,u32,u32)=nullptr;
    u32 (*call_2e758)()=nullptr;
    u32 (*call_30ad8)()=nullptr;
    u32 (*call_30b1c)()=nullptr;
    u32 (*call_30b2c)()=nullptr;
    u32 (*call_30aec)()=nullptr;
};
static s32 app101_sar1_s16(u16 value) noexcept {
    const s32 v=static_cast<s32>(static_cast<s16>(value));
    return v>=0?v/2:-(((-v)+1)/2);
}

u32 function_33c90(Runtime101Boundary& b,u32 first,u32 second) noexcept {
    const u32 gate=b.call_2e340?b.call_2e340():0u;
    if(gate==0u)return gate;
    const Runtime101MouseRegs r=b.int33?b.int33(4u,0u,second+second,first):Runtime101MouseRegs{};
    return r.eax;
}

u32 function_33cb0(Machine& m,Runtime101Boundary& b) noexcept {
    const u32 gate=b.call_2e340?b.call_2e340():0u;
    if(gate!=0u){
        const Runtime101MouseRegs r=b.int33?b.int33(3u,0u,0u,0u):Runtime101MouseRegs{};
        store_le16(dg(m,0x2B7A8u),static_cast<u16>(r.ebx));
        store_le16(dg(m,0x2B7AEu),static_cast<u16>(r.edx));
        const s32 half_x=app101_sar1_s16(static_cast<u16>(r.ecx));
        store_le16(dg(m,0x2B7ACu),static_cast<u16>(half_x));
    }
    return 0x2B7A8u;
}

u32 function_33d18(Runtime101Boundary& b,u32 minimum,u32 maximum) noexcept {
    const u32 gate=b.call_2e340?b.call_2e340():0u;
    if(gate==0u)return gate;
    const Runtime101MouseRegs r=b.int33?b.int33(7u,0u,minimum,maximum):Runtime101MouseRegs{};
    return r.eax;
}

u32 function_33d38(Runtime101Boundary& b,u32 minimum,u32 maximum) noexcept {
    const u32 gate=b.call_2e340?b.call_2e340():0u;
    if(gate==0u)return gate;
    const Runtime101MouseRegs r=b.int33?b.int33(8u,0u,minimum,maximum):Runtime101MouseRegs{};
    return r.eax;
}

u32 function_33d58(Machine& m,Runtime101Boundary& b) noexcept {
    const u32 gate=b.call_2e340?b.call_2e340():0u;
    if(gate==0u)return gate;
    for(;;){
        const u32 state=function_33cb0(m,b);
        const u32 eax=b.call_2e758?b.call_2e758():0u;
        if(load_le16(dg(m,state))==0u)return eax;
    }
}

u32 function_33d78(Machine& m,Runtime101Boundary& b) noexcept {
    const u32 gate=b.call_2e340?b.call_2e340():0u;
    if(gate==0u)return b.call_30aec?b.call_30aec():0u;

    for(;;){
        const u32 state=function_33cb0(m,b);
        if(b.call_2e758)(void)b.call_2e758();
        if(load_le16(dg(m,state))==0u)break;
    }

    for(;;){
        const u32 pending=b.call_30ad8?b.call_30ad8():0u;
        if(pending!=0u)break;
        const u32 state=function_33cb0(m,b);
        if(load_le16(dg(m,state))!=0u)break;
        if(b.call_2e758)(void)b.call_2e758();
    }

    const u32 pending=b.call_30ad8?b.call_30ad8():0u;
    if(pending!=0u){
        if(b.call_30b1c)(void)b.call_30b1c();
        return b.call_30b2c?b.call_30b2c():0u;
    }

    for(;;){
        if(b.call_2e758)(void)b.call_2e758();
        const u32 state=function_33cb0(m,b);
        if(load_le16(dg(m,state))==0u)return state;
    }
}

static_assert(0x33CAFu-0x33C90u==31u);
// 33CAF is an unreferenced NOP.
static_assert(0x33D15u-0x33CB0u==101u);
// 33D15..33D17 is unreferenced LEA EAX,[EAX] alignment.
static_assert(0x33D35u-0x33D18u==29u);
// 33D35..33D37 is unreferenced LEA EAX,[EAX] alignment.
static_assert(0x33D55u-0x33D38u==29u);
// 33D55..33D57 is unreferenced LEA EAX,[EAX] alignment.
static_assert(0x33D76u-0x33D58u==30u);
// 33D76..33D77 is unreferenced MOV EAX,EAX alignment.
static_assert(0x33DDDu-0x33D78u==101u);
// 33DDD..33DDF is three zero alignment bytes; 33DE0 is independently called.
