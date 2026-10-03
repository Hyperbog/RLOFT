// -----------------------------------------------------------------------------
// Batch 06: 03634..04619 map-mode setup, resource tables, nibble grids,
// proximity/collision helpers, and record movement/accounting.
// -----------------------------------------------------------------------------

struct Application06Boundary {
    u32 (*call_19ccc)(u32)=nullptr;
    void (*call_1a9f8)()=nullptr;
    void (*call_30d30)(u32,u32,u32)=nullptr;
    void (*call_22d70)()=nullptr;
    void (*call_22d8c)()=nullptr;
    void (*call_30cd4)(u32,u32,u32)=nullptr;
    void (*call_310b0)(u32)=nullptr;
    u32 (*call_19cf4)()=nullptr;
    u32 (*call_312a0)(u32)=nullptr;
    u32 (*call_3104c)(u32)=nullptr;
    void (*call_312ec)(u32,u32,u32)=nullptr;
    u32 (*call_312ec_local)(u32,u32)=nullptr;
    void (*call_312dc)(u32)=nullptr;
    void (*call_05bdc)(u32,u32,u32,u32)=nullptr;
    void (*call_1c40c)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_30b40)()=nullptr;
    u32 (*call_2c8e4)(u32,u32,u32)=nullptr;
    u16 (*call_2d5d0)(u32,u32,u32)=nullptr;
    u32 (*call_50dc)(u32,u32)=nullptr;
    void (*call_9d90)(u32,u32,u32)=nullptr;
    void (*call_1f494)(u32)=nullptr;
};

[[nodiscard]] u32 function_03634(Machine& m,Application06Boundary& b,Application05Boundary& a5,Application04Boundary& a4,
                                  Application03Boundary& a3,Graphics2Boundary& g) noexcept {
    if(b.call_19ccc)(void)b.call_19ccc(0u);
    for(u32 i=0;i<0x100u;++i)*dg(m,0x759Cu+i)=static_cast<u8>(i);
    *dg(m,0x6058u)=0u;
    if(b.call_1a9f8)b.call_1a9f8();
    if(a4.call_291b0)a4.call_291b0(*dg(m,0xC050u));
    store_le32(dg(m,0x12744u),static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))))-0x18));
    store_le32(dg(m,0x12740u),static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))))-0x18));
    if(static_cast<s32>(load_le32(dg(m,0x12744u)))<-40)store_le32(dg(m,0x12744u),static_cast<u32>(-40));
    if(static_cast<s32>(load_le32(dg(m,0x12740u)))<-40)store_le32(dg(m,0x12740u),static_cast<u32>(-40));
    store_le16(dg(m,0x12756u),load_le16(dg(m,0x25619u)));
    store_le16(dg(m,0x12754u),load_le16(dg(m,0x2561Bu)));
    store_le32(dg(m,0x1274Cu),load_le32(dg(m,0xC6C8u)));
    *dg(m,0x5FCBu)=1u;
    const s32 w=static_cast<s16>(load_le16(dg(m,0x12756u)));
    const s32 h=static_cast<s16>(load_le16(dg(m,0x12754u)));
    if(b.call_30d30)b.call_30d30(0x1760u,0xFFu,static_cast<u32>(w*h));
    function_01558(m,a4);
    function_01834(m,a4,a3);
    function_0139c(m,a4,a3,g,static_cast<s32>(load_le32(dg(m,0x12744u))),
                   static_cast<s32>(load_le32(dg(m,0x12740u))),0x30,0x2F,1u);
    if(a4.call_1a5b0)a4.call_1a5b0(load_le32(dg(m,0xC050u)),
        static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x8600u))))),*dg(m,0x8602u));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    if(b.call_22d70)b.call_22d70();
    *dg(m,0xD680u)=4u;
    while(*dg(m,0xD680u)==4u) {
        if(*dg(m,0x5FCAu)!=0u)function_034b0(m,a5);
        if(load_le16(dg(m,0x9E35u))==0u)*dg(m,0x5FCAu)=0u;
        if(b.call_22d8c)b.call_22d8c();
    }
    if(load_le32(dg(m,0x6050u))!=0u) {
        if(b.call_30d30)b.call_30d30(0x5760u,0u,0x800u);
        if(b.call_30cd4)b.call_30cd4(0x5760u,load_le32(dg(m,0x6050u)),load_le32(dg(m,0x6054u)));
        if(b.call_310b0)b.call_310b0(load_le32(dg(m,0x6050u)));
        store_le32(dg(m,0x6050u),0u);store_le32(dg(m,0x6054u),0u);
    }
    if(load_le32(dg(m,0x6048u))!=0u) {
        if(b.call_310b0)b.call_310b0(load_le32(dg(m,0x6048u)));
        store_le32(dg(m,0x6048u),0u);store_le16(dg(m,0x604Cu),0u);
    }
    return b.call_19cf4?b.call_19cf4():0u;
}
static_assert(0x0380Du-0x03634u==473u);

void function_03810(Machine& m,Application06Boundary& b) noexcept {
    if(b.call_310b0)b.call_310b0(load_le32(dg(m,0x608Cu)));
}
static_assert(0x0381Fu-0x03810u==15u);

[[nodiscard]] u32 function_03820(Machine& m,u32 size) noexcept {
    const u32 r=load_le32(dg(m,0x608Cu))+load_le32(dg(m,0x6094u));
    store_le32(dg(m,0x6094u),load_le32(dg(m,0x6094u))+size);
    return r;
}
static_assert(0x03836u-0x03820u==22u);

void function_03838(Machine& m,Application06Boundary& b) noexcept {
    const u32 h=b.call_312a0?b.call_312a0(0x24u):0u;
    u32 p=b.call_3104c?b.call_3104c(0x708u):0u;store_le32(dg(m,0xC65Cu),p);
    if(b.call_312ec)b.call_312ec(p,0x708u,h);
    store_le32(dg(m,0xC664u),load_le32(dg(m,0xC65Cu))+0x168u);
    p=b.call_3104c?b.call_3104c(0xE10u):0u;store_le32(dg(m,0xC660u),p);
    if(b.call_312ec)b.call_312ec(p,0xE10u,h);
    const struct ReadSpec{u32 dst,size;} specs[]={{0x19660u,0x100u},{0x193E0u,0x100u},{0xC6D8u,4u},{0xC6D4u,4u},{0xC6D0u,4u},{0xC6DCu,4u},{0xC6E0u,4u},{0x2560Cu,4u},{0xC6CCu,4u},{0x255E0u,4u},{0xC73Cu,0x400u},{0xC738u,4u},{0xC730u,4u},{0xC734u,4u}};
    if(b.call_312ec)for(const auto& q:specs)b.call_312ec(q.dst,q.size,h);
    const u32 words=(load_le32(dg(m,0xC730u))+1u)*2u;
    p=b.call_3104c?b.call_3104c(words):0u;store_le32(dg(m,0x258C4u),p);if(b.call_312ec)b.call_312ec(p,words,h);
    if(b.call_312ec)b.call_312ec(0x6090u,4u,h);
    p=b.call_3104c?b.call_3104c(load_le32(dg(m,0x6090u))):0u;store_le32(dg(m,0x608Cu),p);store_le32(dg(m,0x15618u),0u);
    if(static_cast<s32>(load_le32(dg(m,0xC738u)))>0) {
        u32 table_off=4u;
        for(s32 i=0;i<static_cast<s32>(load_le32(dg(m,0xC738u)));++i,table_off+=4u) {
            const u32 n=(b.call_312ec_local?b.call_312ec_local(2u,h):0u)&0xFFFFu;
            u32 dst=function_03820(m,n);
            store_le32(dg(m,0x18FE0u+table_off),dst);
            store_le32(dg(m,0x15618u+table_off),table_off);
            if(b.call_312ec)b.call_312ec(dst,n,h);
            dst=load_le32(dg(m,0x18FE0u+table_off))+0x204u;
            store_le32(dg(m,0x18FE0u+table_off),dst);
        }
    }
    store_le32(dg(m,0x18FE0u),load_le32(dg(m,0x18FE4u)));
    if(b.call_312dc)b.call_312dc(h);
}
static_assert(0x03A64u-0x03838u==556u);

static void application06_constrain_nibbles(Machine& m,u32 base,u32 byte_base) noexcept {
    for(u32 row=0;row<8u;++row) {
        const u32 cap=*dg(m,0x6208u+row);u32 low_sum=0u,high_sum=0u;
        const s32 count=static_cast<s32>(load_le32(dg(m,0x6440u+row*4u)));
        if(count<=0)continue;
        for(s32 i=0;i<count;++i) {
            u8& cell=*dg(m,byte_base+base+row*10u+static_cast<u32>(i));
            u32 low=cell&0x0Fu;if(low==0x0Fu)continue;
            if(cap<low_sum+low){low=(cap-low_sum)&0x0Fu;cell=static_cast<u8>((cell&0xF0u)|low);}
            low_sum+=cell&0x0Fu;
            u32 high=cell>>4;low=cell&0x0Fu;if(high>low){high=low;cell=static_cast<u8>(low|(high<<4));}
            high=cell>>4;
            if(high_sum+high>cap){high=(cap-high_sum)&0x0Fu;cell=static_cast<u8>((cell&0x0Fu)|(high<<4));}
            high_sum+=cell>>4;
        }
    }
}

void function_03a78(Machine& m,Application06Boundary& b,u32 slot,u32 phase) noexcept {
    if(b.call_05bdc)b.call_05bdc(1u,slot,phase-1u,0x6210u);
    application06_constrain_nibbles(m,slot*0x248u,0x2194Au);
}
static_assert(0x03C03u-0x03A78u==395u);

void function_03c04(Machine& m,Application06Boundary& b,u32 slot,u32 phase) noexcept {
    const u32 base=slot*0x248u;const s32 kind=static_cast<s8>(*dg(m,base+0x218F5u));
    if(b.call_05bdc)b.call_05bdc(0u,slot,phase-1u,kind==1?0x6350u:0x62B0u);
    if(kind==1){*dg(m,base+0x219A1u)=0x11u;if(static_cast<s32>(phase-1u)<3)*dg(m,base+0x219A2u)=0xFFu;else *dg(m,base+0x219A2u)=0x11u;}
    else *dg(m,base+0x219A2u)=0x11u;
    application06_constrain_nibbles(m,base,0x2199Au);
}
static_assert(0x03DE3u-0x03C04u==479u);

