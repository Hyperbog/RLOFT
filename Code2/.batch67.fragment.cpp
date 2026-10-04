// -----------------------------------------------------------------------------
// Batch 67: 2DF2C..2E7EE direction-vector dispatch, turn/movement integration,
// drawing/image wrappers, timing/input helpers, and metadata-file loading.
// All addresses are CODE.2 object-relative.
// -----------------------------------------------------------------------------

struct Application67Boundary {
    Application66Boundary* app66=nullptr;
    u32 (*call_35680)(u32,u32)=nullptr;
    u32 (*call_369a7)(u32)=nullptr;
    void (*call_291e0)(u32,u32,u32,u32)=nullptr;
    void (*call_29208)(u32,u32,u32,u32)=nullptr;
    void (*call_29804)(u32,u32,u32,u32)=nullptr;
    void (*call_2939c)(u32,u32,u32,u32)=nullptr;
    u32 (*call_2f29c)(u32,u32)=nullptr;
    void (*call_2a258)(u32,u32)=nullptr;
    void (*call_2a308)(u32,u32)=nullptr;
    void (*call_2f580)(u32)=nullptr;
    void (*call_291b0)(u32)=nullptr;
    u32 (*call_30700)()=nullptr;
    void (*call_2f868)()=nullptr;
    u32 (*call_30ad8)()=nullptr;
    u32 (*call_30aec)()=nullptr;
    u32 (*call_33cb0)()=nullptr;
    void (*call_372f6)(u32)=nullptr;
    u32 (*call_37339)()=nullptr;
    void (*call_373bf)(u32,u32,u32)=nullptr;
    void (*call_37483)(u32,u32,u32)=nullptr;
    void (*call_indirect_f0c8)(u32)=nullptr;
    u32 (*call_312a0)(u32)=nullptr;
    void (*call_312ec)(u32,u32,u32)=nullptr;
    void (*call_312ec_word)(u16*,u32)=nullptr;
    void (*call_30d30)(u32,u32,u32)=nullptr;
    void (*call_312dc)(u32)=nullptr;
};

static s32 app67_norm_once(s32 v) noexcept {
    if(v>=0x168)v=static_cast<s32>(static_cast<u32>(v)-0x168u);
    if(v<0)v=static_cast<s32>(static_cast<u32>(v)+0x168u);
    return v;
}
static void app67_store_heading(Machine& m,u32 raw) noexcept {
    s32 v=static_cast<s32>(raw);
    if(v>=0x168)v=static_cast<s32>(static_cast<u32>(v)-0x168u);
    if(v<0)v=static_cast<s32>(static_cast<u32>(v)+0x168u);
    store_le32(dg(m,0xC658u),static_cast<u32>(v));
}

// Exact eight-entry CS dword table at 2DFEC, owned by 2DF2C.
static constexpr u32 kApp67DirectionJumpTable[8]={
    0x2E03Au,0x2E051u,0x2E068u,0x2E07Cu,
    0x2E090u,0x2E0A4u,0x2E0B8u,0x2E0CCu
};
static_assert(sizeof(kApp67DirectionJumpTable)==32u);

void function_2df2c(Machine& m,Application67Boundary& b,u32* out_dx,u32* out_dy,u32 raw_distance,u32 raw_heading) noexcept {
    s32 heading=static_cast<s32>(raw_heading);
    if(heading>=0x168)heading=static_cast<s32>(static_cast<u32>(heading)-0x168u);
    if(heading<0)heading=static_cast<s32>(static_cast<u32>(heading)+0x168u);

    if(*dg(m,0xC02Fu)==0u){
        const s16 hi=static_cast<s16>(static_cast<u16>(raw_distance>>16u));
        if(hi>=0 && hi==0 && static_cast<u16>(raw_distance)>=0xB000u)
            raw_distance=0x0000AFFFu;

        const u32 h=static_cast<u32>(heading);
        const u32 sin_factor=load_le32(dg(m,load_le32(dg(m,0xC664u))+h*4u));
        const u32 cos_factor=load_le32(dg(m,load_le32(dg(m,0xC65Cu))+h*4u));
        if(out_dy)*out_dy=b.call_35680?b.call_35680(raw_distance,sin_factor):0u;
        const u32 x=b.call_35680?b.call_35680(raw_distance,cos_factor):0u;
        if(out_dx)*out_dx=0u-x;
        return;
    }

    const s32 q=heading/0x2D;
    if(static_cast<u32>(q)>7u)return;
    static constexpr u32 dy[8]={0x00010000u,0x00010000u,0u,0xFFFF0000u,0xFFFF0000u,0xFFFF0000u,0u,0x00010000u};
    static constexpr u32 dx[8]={0u,0xFFFF0000u,0xFFFF0000u,0xFFFF0000u,0u,0x00010000u,0x00010000u,0x00010000u};
    if(out_dy)*out_dy=dy[static_cast<u32>(q)];
    if(out_dx)*out_dx=dx[static_cast<u32>(q)];
}
static_assert((0x2DFE9u-0x2DF2Cu)+(0x2E0E3u-0x2E00Cu)==404u);

void function_2e0e4(Machine& m,Application67Boundary& b,u32 heading,u32 distance) noexcept {
    u32 dx=0u,dy=0u;
    function_2df2c(m,b,&dx,&dy,distance,heading);
    if(b.app66)function_2dbac(m,*b.app66,dy,dx);
}
static_assert(0x2E11Du-0x2E0E4u==57u);

void function_2e120(Machine& m,Application67Boundary& b) noexcept {
    const u32 accum=load_le32(dg(m,0xC6E4u))+load_le32(dg(m,0xC6ECu));
    store_le32(dg(m,0xC6E4u),accum);
    s32 step=static_cast<s32>(accum)/0x14;
    store_le32(dg(m,0xC6E4u),accum-static_cast<u32>(step*0x14));

    if(*dg(m,0xC02Fu)==0u){
        store_le32(dg(m,0xC658u),load_le32(dg(m,0xC658u))-static_cast<u32>(step));
        app67_store_heading(m,load_le32(dg(m,0xC658u)));
    } else {
        if(step<0)step=-0x5A;
        else if(step>0)step=0x5A;
        store_le32(dg(m,0xC658u),load_le32(dg(m,0xC658u))-static_cast<u32>(step));
        app67_store_heading(m,load_le32(dg(m,0xC658u)));
    }

    s32 heading=static_cast<s32>(load_le32(dg(m,0xC658u)));
    const s32 turn=static_cast<s32>(load_le32(dg(m,0xC6F0u)));
    if(turn!=0){
        const u32 raw=b.call_369a7?b.call_369a7(static_cast<u32>(turn)):0u;
        store_le32(dg(m,0xC6E8u),raw);
        heading=static_cast<s32>(static_cast<u32>(heading)+(turn<0?static_cast<u32>(-0x5A):0x5Au));
        heading=app67_norm_once(heading);
    }

    const s32 velocity=static_cast<s32>(load_le32(dg(m,0xC6E8u)));
    if(velocity<0){
        u16 low=static_cast<u16>(velocity);
        low=static_cast<u16>(0u-low); // NEG WORD, then MOVSX.
        const s32 scale=static_cast<s16>(low);
        const u32 distance=static_cast<u32>(static_cast<s32>(0x96*scale));
        u32 dx=0u,dy=0u;
        function_2df2c(m,b,&dx,&dy,distance,static_cast<u32>(heading));
        if(b.app66)function_2dbac(m,*b.app66,dy,dx);
    } else if(velocity>0){
        s32 reverse=app67_norm_once(static_cast<s32>(static_cast<u32>(heading)+0xB4u));
        const u32 distance=static_cast<u32>(0x64*velocity);
        u32 dx=0u,dy=0u;
        function_2df2c(m,b,&dx,&dy,distance,static_cast<u32>(reverse));
        if(b.app66)function_2dbac(m,*b.app66,dy,dx);
    }
}
static_assert(0x2E339u-0x2E120u==537u);

[[nodiscard]] s32 function_2e340(Machine& m) noexcept {
    const u32 p=load_le32(dg(m,0xF04Cu));
    return static_cast<s16>(load_le16(dg(m,p)));
}
static_assert(0x2E349u-0x2E340u==9u);

[[nodiscard]] u32 function_2e34c(Machine& m) noexcept {
    const u32 p=load_le32(dg(m,0xF04Cu));
    return static_cast<s16>(load_le16(dg(m,p+4u)))==1?1u:0u;
}
static_assert(0x2E35Fu-0x2E34Cu==19u);

[[nodiscard]] s32 function_2e360(Machine& m) noexcept {
    const u32 p=load_le32(dg(m,0xF04Cu));
    if(load_le16(dg(m,p))==0u)return 0;
    return static_cast<s16>(load_le16(dg(m,p+2u)));
}
static_assert(0x2E373u-0x2E360u==19u);

void function_2e374(Application67Boundary& b,u32 x,u32 y,u32 width,u32 height,u32 color) noexcept {
    const u32 c=static_cast<u8>(color);
    if(b.call_291e0)b.call_291e0(x,y,height,c);
    if(static_cast<s32>(width)>2 && b.call_29208)b.call_29208(x+1u,y,width-2u,c);
    if(static_cast<s32>(width)>1 && b.call_291e0)b.call_291e0(x+width-1u,y,height,c);
    if(static_cast<s32>(width)>2 && static_cast<s32>(height)>1 && b.call_29208)
        b.call_29208(x+1u,y+height-1u,width-2u,c);
}
static_assert(0x2E404u-0x2E374u==144u);

void function_2e404(Machine& m,u32 table,u32 image) noexcept {
    for(u32 i=0;i<load_le16(dg(m,image+2u));++i)function_2aa9c(m,i,table,image);
}
static_assert(0x2E435u-0x2E404u==49u);

void function_2e438(Machine& m,Application67Boundary& b,u32 source,u32 arg,u32 start,u32 count,u32 image) noexcept {
    s32 first=static_cast<s32>(start);
    s32 end;
    if(count==0xFFFFFFFFu)end=static_cast<s32>(load_le16(dg(m,image+2u)));
    else end=static_cast<s32>(start+count);
    const u32 stride=load_le16(dg(m,image+4u));
    u32 p=source;
    for(s32 i=first;i<end;++i,p+=stride)
        if(b.call_29804)b.call_29804(static_cast<u32>(i),p,arg,image);
}
static_assert(0x2E488u-0x2E438u==80u);

void function_2e488(Machine& m,Application67Boundary& b,u32 source,u32 arg,u32 image) noexcept {
    function_2e438(m,b,source,arg,0u,0xFFFFFFFFu,image);
}
static_assert(0x2E4A1u-0x2E488u==25u);

void function_2e4a4(Machine& m,Application67Boundary& b,u32 source,u32 arg,u32 start,u32 count,u32 image) noexcept {
    s32 first=static_cast<s32>(start);
    s32 end;
    if(count==0xFFFFFFFFu)end=static_cast<s32>(load_le16(dg(m,image+2u)));
    else end=static_cast<s32>(start+count);
    const u32 stride=load_le16(dg(m,image+4u));
    u32 p=source;
    for(s32 i=first;i<end;++i,p+=stride)
        if(b.call_2939c)b.call_2939c(static_cast<u32>(i),p,arg,image);
}
static_assert(0x2E4F4u-0x2E4A4u==80u);

void function_2e4f4(Machine& m,Application67Boundary& b,u32 source,u32 arg,u32 image) noexcept {
    function_2e4a4(m,b,source,arg,0u,0xFFFFFFFFu,image);
}
static_assert(0x2E50Du-0x2E4F4u==25u);

void function_2e510(Machine& m,Application67Boundary& b,u32 source,u32 arg,u32 key0,u32 key1) noexcept {
    const u32 image=b.call_2f29c?b.call_2f29c(key0,key1):0u;
    function_2e4a4(m,b,source,arg,0u,0xFFFFFFFFu,image);
}
static_assert(0x2E536u-0x2E510u==38u);

void function_2e538(Machine& m,Application67Boundary& b,u32 image) noexcept {
    for(u32 i=0;i<load_le16(dg(m,image+2u));++i)
        if(b.call_2a258)b.call_2a258(i,image);
}
static_assert(0x2E561u-0x2E538u==41u);

void function_2e564(Machine& m,Application67Boundary& b,u32 image) noexcept {
    for(u32 i=0;i<load_le16(dg(m,image+2u));++i)
        if(b.call_2a308)b.call_2a308(i,image);
}
static_assert(0x2E58Du-0x2E564u==41u);

void function_2e590(Machine& m,Application67Boundary& b,u32 page) noexcept {
    const u8 depth=*dg(m,0xF0D0u);
    if(depth==0x0Au && b.call_2f580)b.call_2f580(0x1358u);
    *dg(m,0x2B644u+static_cast<u32>(depth))=*dg(m,0xEF4Du);
    *dg(m,0xF0D0u)=static_cast<u8>(depth+1u);
    if(b.call_291b0)b.call_291b0(static_cast<u8>(page));
}
static_assert(0x2E5D3u-0x2E590u==67u);

void function_2e5d4(Machine& m,Application67Boundary& b) noexcept {
    if(*dg(m,0xF0D0u)==0u && b.call_2f580)b.call_2f580(0x136Cu);
    const u8 depth=static_cast<u8>(*dg(m,0xF0D0u)-1u);
    *dg(m,0xF0D0u)=depth;
    if(b.call_291b0)b.call_291b0(*dg(m,0x2B644u+static_cast<u32>(depth)));
}
static_assert(0x2E608u-0x2E5D4u==52u);

void function_2e608(Machine& m,Application67Boundary& b) noexcept {
    store_le32(dg(m,0x2B640u),b.call_30700?b.call_30700():0u);
}
static_assert(0x2E613u-0x2E608u==11u);

[[nodiscard]] u32 function_2e614(Machine& m,Application67Boundary& b,u32 delay,u32 flags) noexcept {
    u32 deadline;
    if(load_le32(dg(m,0x2B640u))!=0u)deadline=load_le32(dg(m,0x2B640u))+delay;
    else deadline=(b.call_30700?b.call_30700():0u)+delay;

    u8 low=static_cast<u8>(flags);
    if((low&0x04u)!=0u)low=static_cast<u8>(low&0xFBu);
    flags=(flags&0xFFFFFF00u)|static_cast<u32>(low);

    for(;;){
        if(b.call_2f868)b.call_2f868();
        low=static_cast<u8>(flags);
        if((low&0x01u)!=0u && (b.call_30ad8?b.call_30ad8():0u)!=0u){
            store_le32(dg(m,0x2B640u),0u);
            return b.call_30aec?b.call_30aec():0u;
        }
        if((low&0x02u)!=0u && function_2e340(m)!=0){
            if(!b.call_33cb0)return 0u;
            u32 p=b.call_33cb0();
            if(load_le16(dg(m,p))!=0u){
                const s32 code=static_cast<s16>(load_le16(dg(m,p)));
                do {p=b.call_33cb0();} while(load_le16(dg(m,p))!=0u);
                store_le32(dg(m,0x2B640u),0u);
                return static_cast<u32>(code+0x1F8);
            }
        }
        // The binary cleared BL bit 2 above, so this encoded branch is dead.
        if((static_cast<u8>(flags)&0x04u)!=0u){
            store_le32(dg(m,0x2B640u),0u);
            return 0x1F0u;
        }
        const u32 now=b.call_30700?b.call_30700():0u;
        if(now<deadline)continue;
        store_le32(dg(m,0x2B640u),0u);
        return 0u;
    }
}
static_assert(0x2E6D0u-0x2E614u==188u);

[[nodiscard]] u32 function_2e6d0(Machine& m,Application67Boundary& b) noexcept {
    if(b.call_372f6)b.call_372f6(0x416u);
    (void)function_2e614(m,b,0u,6u);
    return b.call_37339?b.call_37339():0u;
}
static_assert(0x2E6EEu-0x2E6D0u==30u);

[[nodiscard]] u32 function_2e6f0(Application67Boundary& b,u32 value) noexcept {
    if(b.call_373bf)b.call_373bf(value,0x2B64Eu,0x0Au);
    return 0x2B64Eu;
}
static_assert(0x2E709u-0x2E6F0u==25u);

[[nodiscard]] u32 function_2e70c(Application67Boundary& b,u32 value) noexcept {
    if(b.call_37483)b.call_37483(value,0x2B64Eu,0x0Au);
    return 0x2B64Eu;
}
static_assert(0x2E725u-0x2E70Cu==25u);

[[nodiscard]] u32 function_2e728(s32 value) noexcept {
    if(value<0)return 0xFFFFFFFFu;
    if(value>0)return 1u;
    return 0u;
}
static_assert(0x2E740u-0x2E728u==24u);

void function_2e740(Machine& m,u32 value) noexcept { store_le32(dg(m,0xF0CCu),value); }
static_assert(0x2E74Au-0x2E740u==10u);

void function_2e74c(Machine& m,u32 value) noexcept { store_le32(dg(m,0xF0C8u),value); }
static_assert(0x2E756u-0x2E74Cu==10u);

void function_2e758(Machine& m,Application67Boundary& b) noexcept {
    const u32 target=load_le32(dg(m,0xF0C8u));
    if(target!=0u && b.call_indirect_f0c8)b.call_indirect_f0c8(target);
}
static_assert(0x2E768u-0x2E758u==16u);

void function_2e768(Machine& m,Application67Boundary& b) noexcept {
    const s32 h=b.call_312a0?static_cast<s32>(b.call_312a0(0x1384u)):-1;
    if(h==-1 && b.call_2f580)b.call_2f580(0x1390u);
    if(b.call_312ec)b.call_312ec(0xF054u,0x0Du,static_cast<u32>(h));
    if(b.call_30d30)b.call_30d30(0xF061u,0u,0x50u);
    for(u32 i=0;i<0x0Cu;++i)*dg(m,0xF054u+i)=static_cast<u8>(*dg(m,0xF054u+i)-0x30u);
    u16 len=0u;
    if(b.call_312ec_word)b.call_312ec_word(&len,static_cast<u32>(h));
    if(b.call_312ec)b.call_312ec(0xF061u,static_cast<u32>(static_cast<s32>(static_cast<s16>(len))),static_cast<u32>(h));
    if(b.call_312dc)b.call_312dc(static_cast<u32>(h));
}
static_assert(0x2E7EFu-0x2E768u==135u);

static_assert(
    (0x2DFE9u-0x2DF2Cu)+(0x2E0E3u-0x2E00Cu)+
    (0x2E11Du-0x2E0E4u)+(0x2E339u-0x2E120u)+
    (0x2E349u-0x2E340u)+(0x2E35Fu-0x2E34Cu)+(0x2E373u-0x2E360u)+
    (0x2E404u-0x2E374u)+(0x2E435u-0x2E404u)+(0x2E488u-0x2E438u)+
    (0x2E4A1u-0x2E488u)+(0x2E4F4u-0x2E4A4u)+(0x2E50Du-0x2E4F4u)+
    (0x2E536u-0x2E510u)+(0x2E561u-0x2E538u)+(0x2E58Du-0x2E564u)+
    (0x2E5D3u-0x2E590u)+(0x2E608u-0x2E5D4u)+(0x2E613u-0x2E608u)+
    (0x2E6D0u-0x2E614u)+(0x2E6EEu-0x2E6D0u)+(0x2E709u-0x2E6F0u)+
    (0x2E725u-0x2E70Cu)+(0x2E740u-0x2E728u)+(0x2E74Au-0x2E740u)+
    (0x2E756u-0x2E74Cu)+(0x2E768u-0x2E758u)+(0x2E7EFu-0x2E768u)==2161u);
