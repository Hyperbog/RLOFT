// -----------------------------------------------------------------------------
// Batch 68: 2E7F0..2F0B1 resource-slot initialization, per-item loading,
// dispatch, lifetime tracking, and release. All addresses are CODE.2 object-relative.
// -----------------------------------------------------------------------------

struct Application68Boundary {
    Application67Boundary* app67=nullptr;
    Application55Boundary* app55=nullptr;
    u32 (*call_3104c)(u32)=nullptr;
    void (*call_310b0)(u32)=nullptr;
    s32 (*call_312a0_stack)(const u8*)=nullptr;
    void (*call_312ec)(u32,u32,u32)=nullptr;
    void (*call_312dc)(u32)=nullptr;
    void (*call_31334)(u32,u32)=nullptr;
    void (*call_2f580)(u32)=nullptr;
    void (*call_2f580_stack)(const u8*)=nullptr;
    u32 (*call_30700)()=nullptr;
    void (*call_indirect_f0cc)(u32,u32)=nullptr;
    u32 (*call_2fd9c)(u32,u32)=nullptr;
    u32 (*call_301a8)(u32,u32)=nullptr;
    u32 (*call_374e0)(u32,u32)=nullptr;
    u32 (*call_37590)(u32,u32)=nullptr;
    void (*call_30040)(u32)=nullptr;
    void (*call_3024c)(u32)=nullptr;
    void (*call_3757c)(u32)=nullptr;
    void (*call_375f0)(u32)=nullptr;
};

static void app68_copy_guest_z_pairs(Machine& m,u8* dst,u32 src) noexcept {
    for(;;){
        const u8 a=*dg(m,src); *dst=a;
        if(a==0u)return;
        const u8 c=*dg(m,src+1u); dst+=2u; src+=2u; dst[-1]=c;
        if(c==0u)return;
    }
}
static void app68_append_guest_z_pairs(Machine& m,u8* dst,u32 src) noexcept {
    while(*dst!=0u)++dst;
    app68_copy_guest_z_pairs(m,dst,src);
}
static void app68_copy_fixed(Machine& m,u8* dst,u32 src,u32 n) noexcept {
    for(u32 i=0;i<n;++i)dst[i]=*dg(m,src+i);
}
static u32 app68_slot(Machine& m,u32 slot) noexcept {
    return load_le32(dg(m,0xF0B8u))+slot*0x10u;
}
static u32 app68_item(Machine& m,u32 slot,u32 item) noexcept {
    return load_le32(dg(m,app68_slot(m,slot)+0x0Cu))+item*9u;
}

void function_2efd0(Machine& m,Application68Boundary& b,u32 item,u32 slot) noexcept;

void function_2e7f0(Machine& m,Application68Boundary& b,u32 base_name,u32 raw_count) noexcept {
    if(b.app67)function_2e768(m,*b.app67);
    const u32 slots=b.call_3104c?b.call_3104c(raw_count*0x10u):0u;
    store_le32(dg(m,0xF0B8u),slots);
    store_le32(dg(m,0xF0B4u),raw_count);

    for(s32 i=0;i<static_cast<s32>(raw_count);++i){
        const u32 iu=static_cast<u32>(i);
        const u32 rec=slots+iu*0x10u;
        u8 name[0x54]{};
        if(*dg(m,0xF054u+iu)!=0u){
            app68_copy_guest_z_pairs(m,name,0xF061u);
            app68_append_guest_z_pairs(m,name,0x13B8u);
            app68_append_guest_z_pairs(m,name,base_name);
        } else {
            app68_copy_guest_z_pairs(m,name,base_name);
        }
        if(b.app67)app68_append_guest_z_pairs(m,name,function_2e6f0(*b.app67,iu));
        const s32 handle=b.call_312a0_stack?b.call_312a0_stack(name):-1;
        store_le32(dg(m,rec),static_cast<u32>(handle));
        if(handle==-1 && b.call_2f580)b.call_2f580(0x13BCu);
        if(b.call_312ec)b.call_312ec(rec+8u,4u,static_cast<u32>(handle));
        store_le32(dg(m,rec+4u),0u);
        const u32 count=load_le32(dg(m,rec+8u));
        const u32 items=b.call_3104c?b.call_3104c(count*9u):0u;
        store_le32(dg(m,rec+0x0Cu),items);
        for(s32 j=0;j<static_cast<s32>(count);++j){
            const u32 p=items+static_cast<u32>(j)*9u;
            *dg(m,p)=static_cast<u8>((*dg(m,p)&0xE0u)|0x0Fu);
            *dg(m,p+2u)=0u;
            store_le16(dg(m,p+3u),0u);
            store_le32(dg(m,p+5u),0u);
        }
    }
}
static_assert(0x2E9CBu-0x2E7F0u==475u);

void function_2e9cc(Machine& m,Application68Boundary& b) noexcept {
    for(s32 slot=0;slot<static_cast<s32>(load_le32(dg(m,0xF0B4u)));++slot){
        const u32 su=static_cast<u32>(slot);
        const u32 rec=app68_slot(m,su);
        for(s32 item=0;item<static_cast<s32>(load_le32(dg(m,rec+8u)));++item){
            const u32 iu=static_cast<u32>(item);
            const u32 p=app68_item(m,su,iu);
            if(*dg(m,p+2u)!=0u)*dg(m,p)=static_cast<u8>(*dg(m,p)&0xEFu);
            function_2efd0(m,b,iu,su);
        }
    }
}
static_assert(0x2EA27u-0x2E9CCu==91u);

void function_2ea28(Machine& m,u32 raw_time) noexcept {
    const u16 stamp=static_cast<u16>(raw_time>>5u);
    const u32 base=load_le32(dg(m,0xF0B8u));
    for(s32 slot=0;slot<static_cast<s32>(load_le32(dg(m,0xF0B4u)));++slot){
        const u32 rec=base+static_cast<u32>(slot)*0x10u;
        for(s32 item=0;item<static_cast<s32>(load_le32(dg(m,rec+8u)));++item){
            const u32 p=load_le32(dg(m,rec+0x0Cu))+static_cast<u32>(item)*9u;
            if(*dg(m,p+2u)!=0u)store_le16(dg(m,p+3u),stamp);
        }
    }
    store_le32(dg(m,0xF0B8u),base);
}
static_assert(0x2EA9Au-0x2EA28u==114u);

void function_2ea9c(Machine& m,Application68Boundary& b) noexcept {
    function_2e9cc(m,b);
    for(s32 slot=0;slot<static_cast<s32>(load_le32(dg(m,0xF0B4u)));++slot){
        const u32 rec=app68_slot(m,static_cast<u32>(slot));
        if(load_le32(dg(m,rec))!=0u){
            if(b.call_312dc)b.call_312dc(load_le32(dg(m,rec)));
            store_le32(dg(m,rec),0u);
            store_le32(dg(m,rec+8u),0u);
        }
        if(load_le32(dg(m,rec+0x0Cu))!=0u){
            if(b.call_310b0)b.call_310b0(load_le32(dg(m,rec+0x0Cu)));
            store_le32(dg(m,rec+0x0Cu),0u);
        }
    }
    if(b.call_310b0)b.call_310b0(load_le32(dg(m,0xF0B8u)));
    store_le32(dg(m,0xF0B8u),0u);
}
static_assert(0x2EB26u-0x2EA9Cu==138u);

[[nodiscard]] u32 function_2eb28(Machine& m) noexcept {
    u32 total=0u;
    const u32 base=load_le32(dg(m,0xF0B8u));
    for(s32 slot=0;slot<static_cast<s32>(load_le32(dg(m,0xF0B4u)));++slot)
        total+=load_le32(dg(m,base+static_cast<u32>(slot)*0x10u+4u));
    store_le32(dg(m,0xF0B8u),base);
    return total;
}
static_assert(0x2EB5Eu-0x2EB28u==54u);

void function_2eb60(Machine& m,Application68Boundary& b,u32 dst,u32 index,u32 stream,u32 slot) noexcept {
    if(b.call_31334)b.call_31334(index,stream);
    if(b.call_312ec)b.call_312ec(dst,5u,stream);
    const s32 len=static_cast<s16>(load_le16(dg(m,dst+3u)));
    if(len!=0){
        const u32 p=b.call_3104c?b.call_3104c(static_cast<u32>(len)):0u;
        store_le32(dg(m,dst+5u),p);
        if(b.call_312ec)b.call_312ec(p,static_cast<u32>(len),stream);
    } else {
        store_le32(dg(m,dst+5u),0u);
    }
    if(load_le16(dg(m,dst))!=0x4845u){
        u8 msg[0x50]{};
        app68_copy_fixed(m,msg,0x13DCu,0x24u);
        if(b.app67)app68_append_guest_z_pairs(m,msg,function_2e6f0(*b.app67,index));
        app68_append_guest_z_pairs(m,msg,0x1400u);
        if(b.app67)app68_append_guest_z_pairs(m,msg,function_2e70c(*b.app67,slot));
        if(b.call_2f580_stack)b.call_2f580_stack(msg);
    }
}
static_assert(0x2EC73u-0x2EB60u==275u);

[[nodiscard]] u32 function_2ec74(Machine& m,u32 slot) noexcept {
    return load_le32(dg(m,app68_slot(m,slot)));
}
static_assert(0x2EC84u-0x2EC74u==16u);

[[nodiscard]] u32 function_2ec84(Machine& m,u32 slot) noexcept {
    return load_le32(dg(m,app68_slot(m,slot)+8u));
}
static_assert(0x2EC95u-0x2EC84u==17u);

static constexpr u32 kApp68LoadJumpTable[7]={
    0x2EE00u,0x2EE28u,0x2EE50u,0x2EE78u,0x2EEA0u,0x2EEC5u,0x2EEEAu
};
static_assert(sizeof(kApp68LoadJumpTable)==28u);

void function_2ecb4(Machine& m,Application68Boundary& b,u32 raw_item,u32 slot) noexcept {
    const s32 item=static_cast<s32>(raw_item);
    const u32 rec=app68_slot(m,slot);
    if(item<0 || item>=static_cast<s32>(load_le32(dg(m,rec+8u)))){
        u8 msg[0x50]{};
        app68_copy_fixed(m,msg,0x1404u,0x1Fu);
        if(b.app67)app68_append_guest_z_pairs(m,msg,function_2e6f0(*b.app67,raw_item));
        app68_append_guest_z_pairs(m,msg,0x1400u);
        if(b.app67)app68_append_guest_z_pairs(m,msg,function_2e70c(*b.app67,slot));
        if(b.call_2f580_stack)b.call_2f580_stack(msg);
    }

    const u32 p=app68_item(m,slot,raw_item);
    if(*dg(m,p+2u)==0u){
        if(load_le32(dg(m,0xF0CCu))!=0u && b.call_indirect_f0cc)b.call_indirect_f0cc(raw_item,slot);
        function_2eb60(m,b,0x2B670u,raw_item,load_le32(dg(m,rec)),slot);
        const u8 raw_type=*dg(m,0x2B672u);
        const u8 selector=static_cast<u8>(raw_type-1u);
        u32 loaded=0u;
        const u32 handle=load_le32(dg(m,rec));
        if(selector<=6u){
            switch(selector){
                case 0u: loaded=b.call_2fd9c?b.call_2fd9c(0x2B670u,handle):0u; break;
                case 1u: loaded=b.call_301a8?b.call_301a8(0x2B670u,handle):0u; break;
                case 2u: loaded=(b.app55?function_286c4(m,*b.app55,0x2B670u,handle):0u); break;
                case 3u: loaded=(b.app55?function_28020(m,*b.app55,0x2B670u,handle):0u); break;
                case 4u: loaded=b.call_374e0?b.call_374e0(0x2B670u,handle):0u; break;
                case 5u: loaded=b.call_37590?b.call_37590(0x2B670u,handle):0u; break;
                case 6u: loaded=b.call_37590?b.call_37590(0x2B670u,handle):0u; break;
            }
            store_le32(dg(m,p+5u),loaded);
        } else if(b.call_2f580){
            b.call_2f580(0x1424u);
        }
        *dg(m,p+2u)=1u;
        *dg(m,p)=static_cast<u8>((*dg(m,p)&0xE0u)|(raw_type&0x0Fu));
        if(load_le32(dg(m,0x2B675u))!=0u && b.call_310b0)b.call_310b0(load_le32(dg(m,0x2B675u)));
        store_le32(dg(m,rec+4u),load_le32(dg(m,rec+4u))+1u);
    }
    const u32 now=b.call_30700?b.call_30700():0u;
    store_le16(dg(m,p+3u),static_cast<u16>(now>>5u));
}
static_assert(0x2EFB3u-0x2ECB4u==767u);

static constexpr u32 kApp68FreeJumpTable[7]={
    0x2F021u,0x2F032u,0x2F03Au,0x2F042u,0x2F04Au,0x2F052u,0x2F063u
};
static_assert(sizeof(kApp68FreeJumpTable)==28u);

void function_2efd0(Machine& m,Application68Boundary& b,u32 item,u32 slot) noexcept {
    const u32 rec=app68_slot(m,slot);
    const u32 p=app68_item(m,slot,item);
    if((*dg(m,p)&0x10u)!=0u || *dg(m,p+2u)==0u)return;
    const u8 selector=static_cast<u8>((*dg(m,p)&0x0Fu)-1u);
    const u32 payload=load_le32(dg(m,p+5u));
    if(selector<=6u){
        switch(selector){
            case 0u:
                if(*dg(m,p+2u)==1u && b.call_30040)b.call_30040(payload);
                break;
            case 1u:
                if(b.call_3024c)b.call_3024c(payload);
                break;
            case 2u:
                if(b.app55)function_2871c(m,*b.app55,payload);
                break;
            case 3u:
                if(b.app55)function_28078(m,*b.app55,payload);
                break;
            case 4u:
                if(b.call_3757c)b.call_3757c(payload);
                break;
            case 5u:
            case 6u:
                if(*dg(m,p+2u)==1u && b.call_375f0)b.call_375f0(payload);
                break;
        }
    }
    *dg(m,p+2u)=0u;
    store_le16(dg(m,p+3u),0u);
    store_le32(dg(m,p+5u),0u);
    store_le32(dg(m,rec+4u),load_le32(dg(m,rec+4u))-1u);
}
static_assert(0x2F0B2u-0x2EFD0u==226u);

static_assert(
    (0x2E9CBu-0x2E7F0u)+(0x2EA27u-0x2E9CCu)+(0x2EA9Au-0x2EA28u)+
    (0x2EB26u-0x2EA9Cu)+(0x2EB5Eu-0x2EB28u)+(0x2EC73u-0x2EB60u)+
    (0x2EC84u-0x2EC74u)+(0x2EC95u-0x2EC84u)+(0x2EFB3u-0x2ECB4u)+
    (0x2F0B2u-0x2EFD0u)==2173u);
