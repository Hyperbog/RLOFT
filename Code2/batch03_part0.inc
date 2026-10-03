// -----------------------------------------------------------------------------
// Batch 03: 006C0..011A4 application map/search/render helpers.
// -----------------------------------------------------------------------------

struct Application03Boundary {
    u32 (*call_2acdc)(u32)=nullptr;
    u32 (*call_2ad18)(u32)=nullptr;
    u32 (*call_2ade0)(u32)=nullptr;
    void (*call_2975c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_2df2c)(u32*,u32*,u32,u32)=nullptr;
};

// 006C0..006FC: derive the two signed camera-relative coordinates and clamp
// each stored coordinate to -40. EAX's unclamped second value is not used by
// any known caller, so only the encoded memory side effects are exposed.
[[nodiscard]] u32 function_006c0(Machine& m) noexcept {
    s32 x=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))))-0x18;
    const s32 y=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))))-0x18;
    store_le32(dg(m,0x12740u),static_cast<u32>(y));
    if(x<-40)x=-40;
    if(static_cast<s32>(load_le32(dg(m,0x12740u)))<-40)
        store_le32(dg(m,0x12740u),static_cast<u32>(-40));
    store_le32(dg(m,0x12744u),static_cast<u32>(x));
    return static_cast<u32>(y);
}
static_assert(0x006FDu-0x006C0u==61u);

// 00700..00791: search either the primary 2223A/2223E record set or the
// alternate list rooted at 6048, selected by byte 5FCB. The 6048 base is
// snapshotted and written back on every return path exactly as encoded.
[[nodiscard]] s32 function_00700(Machine& m,s32 key6,s32 key2,s32 start) noexcept {
    const u32 base=load_le32(dg(m,0x6048u));
    s32 i=start;
    if(*dg(m,0x5FCBu)!=0u) {
        u32 off=static_cast<u32>(i)*0x16u;
        while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A6u))))) {
            if(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x2223Eu+off))))==key6 &&
               static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x2223Au+off))))==key2) {
                store_le32(dg(m,0x6048u),base);
                return i;
            }
            off+=0x16u;
            ++i;
        }
    } else {
        u32 off=static_cast<u32>(i)*0x16u;
        while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x604Cu))))) {
            const u32 p=base+off;
            if(static_cast<s32>(static_cast<s16>(load_le16(dg(m,p+6u))))==key6 &&
               static_cast<s32>(static_cast<s16>(load_le16(dg(m,p+2u))))==key2) {
                store_le32(dg(m,0x6048u),base);
                return i;
            }
            off+=0x16u;
            ++i;
        }
    }
    store_le32(dg(m,0x6048u),base);
    return -1;
}
static_assert(0x00792u-0x00700u==146u);

// 00794..007E1: when 5FCB is set, search the 1761A/1761E 22-byte record set.
[[nodiscard]] s32 function_00794(Machine& m,s32 key1e,s32 key1a,s32 start) noexcept {
    if(*dg(m,0x5FCBu)==0u)return -1;
    s32 i=start;
    u32 off=static_cast<u32>(i)*0x16u;
    while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A4u))))) {
        if(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x1761Eu+off))))==key1e &&
           static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x1761Au+off))))==key1a)
            return i;
        off+=0x16u;
        ++i;
    }
    return -1;
}
static_assert(0x007E2u-0x00794u==78u);

// 007E4..00838: search type-54h records in the primary 2223A/2223E table.
[[nodiscard]] s32 function_007e4(Machine& m,s32 key3e,s32 key3a,s32 start) noexcept {
    s32 i=start;
    u32 off=static_cast<u32>(i)*0x16u;
    while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A6u))))) {
        if(load_le16(dg(m,0x22246u+off))==0x54u &&
           static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x2223Eu+off))))==key3e &&
           static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x2223Au+off))))==key3a)
            return i;
        off+=0x16u;
        ++i;
    }
    return -1;
}
static_assert(0x00839u-0x007E4u==85u);

// 0083C..00930: filtered type search. Admission is controlled by 2ACDC and
// F03C/F03E, then by table bit 40h plus the encoded +/-10 coordinate test.
[[nodiscard]] s32 function_0083c(Machine& m,Application03Boundary& b,s32 key86,s32 key82,s32 start) noexcept {
    s32 i=start;
    u32 off=static_cast<u32>(i)*0x16u;
    while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A2u))))) {
        bool found=false;
        if((*dg(m,0x14D8Bu+off)&2u)==0u) {
            const u32 type=load_le16(dg(m,0x14D8Eu+off));
            if((b.call_2acdc?b.call_2acdc(type):0u)!=0u &&
               static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D86u+off))))==key86 &&
               static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D82u+off))))==key82) {
                if(load_le16(dg(m,0xF03Eu))!=0u) {
                    found=true;
                } else if(load_le16(dg(m,0xF03Cu))!=0u) {
                    const u32 p=load_le32(dg(m,0x22230u))+type*0x45u;
                    found=(*dg(m,p+0x3Au)&0x20u)!=0u;
                } else if((*dg(m,0x198E0u+static_cast<u32>(load_le16(dg(m,0x14D8Cu+off)))*2u)&0x40u)!=0u) {
                    s32 dx=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D86u+off))))-
                           static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))));
                    if(dx<0)dx=-dx;
                    s32 dy=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D82u+off))))-
                           static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))));
                    if(dy<0)dy=-dy;
                    found=dx<=10&&dy<=10;
                }
            }
        }
        if(found)return i;
        off+=0x16u;
        ++i;
    }
    return -1;
}
static_assert(0x00931u-0x0083Cu==245u);

// 00934..009AF: alternate type-predicate record search using 2AD18/2ADE0.
[[nodiscard]] s32 function_00934(Machine& m,Application03Boundary& b,s32 key86,s32 key82,s32 start) noexcept {
    s32 i=start;
    u32 off=static_cast<u32>(i)*0x16u;
    while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A2u))))) {
        if((*dg(m,0x14D8Bu+off)&2u)==0u) {
            const u32 type=load_le16(dg(m,0x14D8Eu+off));
            const bool admitted=(b.call_2ad18&&b.call_2ad18(type)!=0u)||
                                (b.call_2ade0&&b.call_2ade0(type)!=0u);
            if(admitted &&
               static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D86u+off))))==key86 &&
               static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D82u+off))))==key82)
                return i;
        }
        off+=0x16u;
        ++i;
    }
    return -1;
}
static_assert(0x009B0u-0x00934u==124u);

// 009B0..00B6B: classify a grid cell from the four orthogonal neighbors.
// EAX tracks neighbors below 20h, EDX tracks 2Eh/2Fh neighbors; the current
// cell byte selects special return codes or the dword table at 5FF0.
[[nodiscard]] u32 function_009b0(Machine& m,u32 cell,s32 col,s32 row) noexcept {
    u32 low_mask=0u;
    u32 slash_mask=0u;
    if(col!=0) {
        const u8 v=*dg(m,cell+0x175Fu);
        if(v<0x20u)low_mask=1u;
        if(v==0x2Fu||v==0x2Eu)slash_mask=1u;
    }
    const s32 width=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12756u))));
    if(width-1>col) {
        const u8 v=*dg(m,cell+0x1761u);
        if(v<0x20u)low_mask+=8u;
        if(v==0x2Fu||v==0x2Eu)slash_mask+=8u;
    }
    if(row!=0) {
        const u32 p=cell-static_cast<u32>(width);
        const u8 v=*dg(m,p+0x1760u);
        if(v<0x20u)low_mask+=2u;
        if(v==0x2Fu||v==0x2Eu)slash_mask+=2u;
    }
    const s32 height=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12754u))));
