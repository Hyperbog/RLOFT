// -----------------------------------------------------------------------------
// Batch 41: 1C77C..1CE65 modal actor display, party selectors, and selector
// composition helper. 1C9A0..1CA2B is the exact 35-entry CS dispatch table;
// 1C99F, 1CD47 and 1CE66..1CE67 are compiler separators. Next routine 1CE68.
// -----------------------------------------------------------------------------

struct Application41Boundary {
    Application39Boundary* app39=nullptr;
    Application40Boundary* app40=nullptr;
    u32 (*call_3104c)(u32)=nullptr;
    void (*call_30d30)(u32,u32,u32)=nullptr;
    void (*call_1a9f8)()=nullptr;
    void (*call_291b0)(u32)=nullptr;
    void (*call_14118)(u32,u32,u32)=nullptr;
    u32 (*call_13fd4)(u32,u32,u32,u32,u32,u32,u32)=nullptr;
    void (*call_2975c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_146b8)(u32)=nullptr;
    void (*call_1a510)(u32)=nullptr;
    void (*call_2e614)(u32,u32)=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    void (*call_22d70)()=nullptr;
    void (*call_27ff0)()=nullptr;
    void (*call_310b0)(u32)=nullptr;
    u32 (*call_30ca2)()=nullptr;
};

void function_1c77c(Machine& m,Application41Boundary& b,u32 slot,u32 text_index) noexcept {
    const u32 buffer=b.call_3104c?b.call_3104c(0x800u):0u;
    if(b.call_30d30)b.call_30d30(buffer,0u,0x800u);
    if(b.call_1a9f8)b.call_1a9f8();
    if(b.call_291b0)b.call_291b0(static_cast<u32>(*dg(m,0xC050u)));
    if(b.app39)function_1c368(m,*b.app39,0x759Cu,text_index);
    if(b.call_14118)b.call_14118(0x759Cu,buffer,0x800u);

    const u32 first=b.call_13fd4?
        b.call_13fd4(1u,0x7Eu,0x55u,0x0Cu,0x96u,buffer,1u):0u;
    const u32 actor=slot*0x248u;
    u32 graphic=static_cast<u32>(static_cast<s32>(static_cast<s8>(*dg(m,0x218F2u+actor))));
    graphic+=static_cast<u32>(*dg(m,0x218F3u+actor))*0x17u;
    graphic+=load_le32(dg(m,0x258E4u));
    if((load_le16(dg(m,0x218F0u+actor))&0x0100u)!=0u)graphic+=0x2Eu;

    const u32 panel_x=(first+1u)*0x0Cu+0x5Eu;
    if(b.app40)function_1c40c(m,*b.app40,load_le32(dg(m,0xC050u)),0x20u,0x50u,panel_x,0xA0u);
    if(b.call_2975c)b.call_2975c(0u,0x24u,0x86u,graphic,1u);
    if(b.call_13fd4)(void)b.call_13fd4(1u,0x7Eu,0x55u,0x0Cu,0x96u,buffer,1u);

    const u32 overlay=load_le32(dg(m,0xBF2Cu));
    if(overlay!=0u&&b.call_146b8)b.call_146b8(load_le32(dg(m,0x25614u))+overlay);
    if(b.call_1a510)b.call_1a510(load_le32(dg(m,0xC050u)));
    store_le16(dg(m,0xA6BEu),0u);
    store_le16(dg(m,0xA6BCu),0u);
    *dg(m,0xC050u)^=1u; // encoded XOR BYTE PTR; upper three bytes are untouched.
    if(b.call_2e614)b.call_2e614(9u,0u);

    if(b.call_1a1f4) {
        while(b.call_1a1f4()!=0u){}
        while(b.call_1a1f4()==0u){}
        while(b.call_1a1f4()!=0u){}
    }

    if(b.call_1a510)b.call_1a510(load_le32(dg(m,0xC050u)));
    *dg(m,0xC050u)^=1u;
    if(b.call_1a9f8)b.call_1a9f8();
    if(b.call_22d70)b.call_22d70();
    if(load_le32(dg(m,0xBF2Cu))!=0u) {
        if(b.call_27ff0)b.call_27ff0();
        store_le32(dg(m,0xBF2Cu),0u);
    }
    if(b.call_310b0)b.call_310b0(buffer);
}
static_assert(0x1C924u-0x1C77Cu==424u);

[[nodiscard]] u32 function_1c924(Machine& m,Application41Boundary& b,u32 /*preserved_stack_arg*/) noexcept {
    const u32 random=b.call_30ca2?b.call_30ca2():0u;
    const u32 start=(random*4u)>>15u; // LEA EAX,[EAX*4] then logical SHR 15.
    u32 slot=start;

    for(;;) {
        const u16 flags=load_le16(dg(m,0x218F0u+slot*0x248u));
        if((flags&0x0080u)==0u&&(flags&0xF240u)==0u)return slot;
        slot=(slot+1u)%4u;
        if(slot==start)break;
    }

    // Fallback deliberately ignores bit 0080h and can loop forever if all four
    // slots keep one of the F240h rejection bits set, exactly like the binary.
    for(;;) {
        const u16 flags=load_le16(dg(m,0x218F0u+slot*0x248u));
        if((flags&0xF240u)==0u)return slot;
        slot=(slot+1u)%4u;
    }
}
static_assert(0x1C99Fu-0x1C924u==123u);

static constexpr u32 kFunction1ca2cDispatch[35]={
    0x1CA93u,0x1CAA2u,0x1CAB4u,0x1CAC6u,0x1CAD8u,0x1CAEAu,0x1CAFCu,
    0x1CB0Eu,0x1CB20u,0x1CB32u,0x1CB44u,0x1CB56u,0x1CB68u,0x1CB7Au,
    0x1CB8Cu,0x1CB9Eu,0x1CBB0u,0x1CBC2u,0x1CBD4u,0x1CBE3u,0x1CBF5u,
    0x1CC07u,0x1CC19u,0x1CC2Bu,0x1CC3Du,0x1CC8Cu,0x1CCAFu,0x1CC7Au,
    0x1CD15u,0x1CCF8u,0x1CCDBu,0x1CC59u,0x1CC68u,0x1CD32u,0x1CA7Au
};
static_assert(sizeof(kFunction1ca2cDispatch)/sizeof(kFunction1ca2cDispatch[0])==35u);

[[nodiscard]] u32 function_1ca2c(Machine& m,u32 criterion) noexcept {
    for(u32 slot=0u;slot<4u;++slot) {
        const u32 actor=0x218E0u+slot*0x248u;
        const u16 flags=load_le16(dg(m,actor+0x10u));
        if((flags&0xF240u)!=0u||criterion>0x22u)continue;

        bool match=false;
        switch(kFunction1ca2cDispatch[criterion]) {
            case 0x1CA93u: match=*dg(m,actor+0x1Au)==0u; break;
            case 0x1CAA2u: match=*dg(m,actor+0x1Au)==2u; break;
            case 0x1CAB4u: match=*dg(m,actor+0x1Au)==4u; break;
            case 0x1CAC6u: match=*dg(m,actor+0x1Au)==1u; break;
            case 0x1CAD8u: match=*dg(m,actor+0x1Au)==3u; break;
            case 0x1CAEAu: match=*dg(m,actor+0x1Au)==5u; break;
            case 0x1CAFCu: match=static_cast<u32>(*dg(m,actor+0x3Du))>0x10u; break;
            case 0x1CB0Eu: match=static_cast<u32>(*dg(m,actor+0x3Du))<0x0Au; break;
            case 0x1CB20u: match=static_cast<u32>(*dg(m,actor+0x33u))>0x10u; break;
            case 0x1CB32u: match=static_cast<u32>(*dg(m,actor+0x33u))<0x0Au; break;
            case 0x1CB44u: match=static_cast<u32>(*dg(m,actor+0x39u))>0x10u; break;
            case 0x1CB56u: match=static_cast<u32>(*dg(m,actor+0x39u))<0x0Au; break;
            case 0x1CB68u: match=static_cast<u32>(*dg(m,actor+0x35u))>0x10u; break;
            case 0x1CB7Au: match=static_cast<u32>(*dg(m,actor+0x35u))<0x0Au; break;
            case 0x1CB8Cu: match=static_cast<u32>(*dg(m,actor+0x37u))>0x10u; break;
            case 0x1CB9Eu: match=static_cast<u32>(*dg(m,actor+0x37u))<0x0Au; break;
            case 0x1CBB0u: match=static_cast<u32>(*dg(m,actor+0x3Bu))>0x10u; break;
            case 0x1CBC2u: match=static_cast<u32>(*dg(m,actor+0x3Bu))<0x0Au; break;
            case 0x1CBD4u: match=*dg(m,actor+0x14u)==0u; break;
            case 0x1CBE3u: match=*dg(m,actor+0x14u)==5u; break;
            case 0x1CBF5u: match=*dg(m,actor+0x14u)==1u; break;
            case 0x1CC07u: match=*dg(m,actor+0x14u)==4u; break;
            case 0x1CC19u: match=*dg(m,actor+0x14u)==3u; break;
            case 0x1CC2Bu: match=*dg(m,actor+0x14u)==2u; break;
            case 0x1CC3Du: match=*dg(m,actor+0x14u)!=0u&&*dg(m,actor+0x14u)!=1u; break;
            case 0x1CC8Cu:
                match=*dg(m,actor+0x15u)==0u||*dg(m,actor+0x16u)==0u||*dg(m,actor+0x17u)==0u;
                break;
            case 0x1CCAFu:
                match=static_cast<s8>(*dg(m,actor+0x15u))==2||
                      static_cast<s8>(*dg(m,actor+0x16u))==2||
                      static_cast<s8>(*dg(m,actor+0x17u))==2;
                break;
            case 0x1CC7Au: match=static_cast<s8>(*dg(m,actor+0x15u))==1; break;
            case 0x1CD15u:
                match=static_cast<s8>(*dg(m,actor+0x15u))==4||
                      static_cast<s8>(*dg(m,actor+0x16u))==4||
                      static_cast<s8>(*dg(m,actor+0x17u))==4;
                break;
            case 0x1CCF8u:
                match=static_cast<s8>(*dg(m,actor+0x15u))==3||
                      static_cast<s8>(*dg(m,actor+0x16u))==3||
                      static_cast<s8>(*dg(m,actor+0x17u))==3;
                break;
            case 0x1CCDBu:
                match=static_cast<s8>(*dg(m,actor+0x15u))==5||
                      static_cast<s8>(*dg(m,actor+0x16u))==5||
                      static_cast<s8>(*dg(m,actor+0x17u))==5;
                break;
            case 0x1CC59u: match=*dg(m,actor+0x13u)==0u; break;
            case 0x1CC68u: match=*dg(m,actor+0x13u)==1u; break;
            case 0x1CD32u: match=false; break;
            case 0x1CA7Au: match=(flags&0x0080u)!=0u; break;
            default: break;
        }
        if(match)return slot;
    }
    return 0xFFFFFFFFu;
}
static_assert(0x1CD47u-0x1CA2Cu==795u);

void function_1cd48(Machine& m,Application41Boundary& b,
                     u32 latch,
                     u32 criterion0,u32 text0,
                     u32 criterion1,u32 text1,
                     u32 criterion2,u32 text2,
                     u32 criterion3,u32 text3) noexcept {
    const u32 latch_bit=latch+0x80u;
    if(latch!=0xFFFFFFFFu&&function_1c1dc(m,latch_bit)!=0u) {
        function_1c124(m,latch_bit);
        return;
    }

    const u32 criteria[4]={criterion0,criterion1,criterion2,criterion3};
    const u32 texts[4]={text0,text1,text2,text3};
    for(u32 i=0u;i<4u;++i) {
        const u32 text=texts[i]+0x292u;
        const u32 selected=criteria[i]==0x21u?
            function_1c924(m,b,text):function_1ca2c(m,criteria[i]);
        if(selected!=0xFFFFFFFFu) {
            // For criterion 21h the binary leaves the pushed text argument on
            // the stack and reuses it as function_1C77C's second argument.
            function_1c77c(m,b,selected,text);
            break;
        }
    }

    if(latch!=0xFFFFFFFFu)
        function_1c124(m,latch_bit); // same signed /32 + masked remainder arithmetic.
}
static_assert(0x1CE66u-0x1CD48u==286u);
static_assert((0x1C924u-0x1C77Cu)+(0x1C99Fu-0x1C924u)+
              (0x1CD47u-0x1CA2Cu)+(0x1CE66u-0x1CD48u)==1628u);
