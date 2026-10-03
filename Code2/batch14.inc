// -----------------------------------------------------------------------------
// Batch 14: 094E0..09B49 opposite-buffer hit testing, interactive selection,
// effect-slot insertion, and party status/effect application.
// -----------------------------------------------------------------------------

struct Application14Boundary {
    u32 (*call_2acdc)(u32)=nullptr;
    u32 (*call_21bf0)(u32,u32)=nullptr;
    void (*call_1c21c)(u32)=nullptr;
    void (*call_1c258)()=nullptr;
    void (*call_1a9f8)()=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    u32 (*call_30b40)()=nullptr;
    void (*call_1c2cc)()=nullptr;
    void (*call_22d70)()=nullptr;
    void (*call_2195c)(u32,u32,u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_f218)(u32,u32)=nullptr;
    void (*call_f238)(u32,u32,u32)=nullptr;
    void (*call_1ea90)(u32,u32,u32)=nullptr;
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    void (*call_28168)()=nullptr;
    void (*call_1a8c8)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_19cc4)()=nullptr;
};

[[nodiscard]] static u32 app14_toggle_low8(u32 v) noexcept {
    return (v&0xFFFFFF00u)|((v^1u)&0xFFu);
}
[[nodiscard]] static u32 app14_toggle_low16(u32 v) noexcept {
    return (v&0xFFFF0000u)|((v^1u)&0xFFFFu);
}

// 094E0..095BC: search the opposite display buffer from the highest ordering
// value down. The inner search deliberately falls through to record[count] if
// no ordering-word match is found among records 0..count-1.
[[nodiscard]] u32 function_094e0(Machine& m,Application14Boundary& b,s32 x,s32 y) noexcept {
    u32 side=app14_toggle_low8(load_le32(dg(m,0xC050u)));
    s32 order=static_cast<s32>(load_le16(dg(m,0xF038u+side*2u)))-1;
    while(order>=0) {
        u32 scan_side=app14_toggle_low8(load_le32(dg(m,0xC050u)));
        u32 index=0u;
        u32 off=0u;
        const u32 count=load_le16(dg(m,0xF038u+scan_side*2u));
        while(static_cast<s32>(index)<static_cast<s32>(count)) {
            const u32 list=scan_side*0x2260u;
            if(load_le16(dg(m,0x27152u+list+off))==static_cast<u16>(order))break;
            off+=0x16u;
            ++index;
        }
        const u32 rec=0x27148u+scan_side*0x2260u+index*0x16u;
        if(load_le16(dg(m,rec+8u))==0u) {
            const u32 id=load_le16(dg(m,rec+6u));
            const u32 mapped=load_le16(dg(m,0x14D8Eu+id*0x16u));
            if((!b.call_2acdc||b.call_2acdc(mapped)!=0u) &&
               (!b.call_21bf0||b.call_21bf0(rec,0u)!=0u) &&
               x>=static_cast<s32>(load_le16(dg(m,rec+0x0Eu))) &&
               x< static_cast<s32>(load_le16(dg(m,rec+0x10u))) &&
               y>=static_cast<s32>(load_le16(dg(m,rec+0x12u))) &&
               y< static_cast<s32>(load_le16(dg(m,rec+0x14u))))
                return id;
        }
        --order;
    }
    return 0xFFFFFFFFu;
}
static_assert(0x095BDu-0x094E0u==221u);

// 095C0..096DF: require one free opposite-buffer record slot, then poll for a
// selectable rectangle. The first/second 1A1F4 phases and ESC handling are kept
// distinct because the binary loops to different polling addresses.
[[nodiscard]] u32 function_095c0(Machine& m,Application14Boundary& b) noexcept {
    u32 side=app14_toggle_low16(load_le32(dg(m,0xC050u)));
    u32 index=0u;
    u32 off=0u;
    const u32 count=load_le16(dg(m,0xF038u+side*2u));
    while(static_cast<s32>(index)<=static_cast<s32>(count)) {
        if(load_le16(dg(m,0x27150u+side*0x2260u+off))==0u)break;
        off+=0x16u;
        ++index;
    }
    if(index==count) {
        if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x66ACu)));
        if(b.call_1c258)b.call_1c258();
        return 0xFFFFFFFFu;
    }

    if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x66B0u)));
    s32 selected=-1;
    bool cancel=false;
    if(b.call_1c258)b.call_1c258();
    if(b.call_1a9f8)b.call_1a9f8();
    while(!cancel&&selected==-1) {
        const u32 first=b.call_1a1f4?b.call_1a1f4():0u;
        if(first!=0u) {
            if(b.call_30b40&&b.call_30b40()==0x1Bu)cancel=true;
            continue;
        }
        for(;;) {
            const u32 second=b.call_1a1f4?b.call_1a1f4():0u;
            if(second!=0u)break;
            if(b.call_30b40&&b.call_30b40()==0x1Bu) { cancel=true; break; }
        }
        if(cancel)break;
        if((load_le16(dg(m,0x9E35u))&2u)!=0u)break;
        const s32 px=static_cast<s16>(load_le16(dg(m,0x9E31u)));
        const s32 py=static_cast<s16>(load_le16(dg(m,0x9E33u)));
        if(px>=4&&px<0x104&&py>=0x20&&py<0x120)
            selected=static_cast<s32>(function_094e0(m,b,px,py-0x20));
    }
    if(b.call_1c2cc)b.call_1c2cc();
    if(b.call_22d70)b.call_22d70();
    return static_cast<u32>(selected);
}
static_assert(0x096E0u-0x095C0u==288u);

// 096E0..0978C: insert one of two words into the first -1 word of the 12-slot
// per-party list. If all 12 are occupied, forward the encoded coordinate pair to
// 2195C and return the mode-derived selector.
[[nodiscard]] u32 function_096e0(Machine& m,Application14Boundary& b,
                                  u32 a0,u32 a1,u32 a2,u32 a3,u32 mode,u32 slot) noexcept {
    u32 i=0u;
    u32 p=0x21920u+slot*0x248u;
    while(i<12u&&static_cast<s16>(load_le16(dg(m,p)))!=-1) { ++i; p+=2u; }
    if(i<12u) {
        const u32 which=(mode!=0u)?0u:1u;
        store_le16(dg(m,p),static_cast<u16>((mode!=0u)?a0:a1));
        return which;
    }
    if(mode!=0u) {
        if(b.call_2195c)b.call_2195c(0x80u,a0,a2,load_le32(dg(m,0x255D0u)),load_le32(dg(m,0x255D4u)),1u,0u);
        return 0u;
    }
    if(b.call_2195c)b.call_2195c(0x80u,a1,a3,load_le32(dg(m,0x255D0u)),load_le32(dg(m,0x255D4u)),1u,0u);
    return 1u;
}
static_assert(0x0978Du-0x096E0u==173u);

static bool app14_id_is_class_d(Machine& m,s32 id) noexcept {
    if(id==-1)return false;
    const u32 u=static_cast<u32>(id);
    const u32 off=(u<<6)+u;
    return *dg(m,0x255D8u+off+0x2Fu)==0x0Du;
}
static void app14_redraw_slot(Machine& m,Application14Boundary& b,s32 slot,s32 side) noexcept {
    const s32 rem=slot%2;
    const s32 row=slot/2;
    const u32 base=static_cast<u32>(row*0xD3 + rem*0x35);
    const u32 x0=base+static_cast<u32>(side*0x21+4);
    if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0x114u,x0,0x20u,0x10u);
    const u32 toggled=app14_toggle_low16(static_cast<u32>(side));
    const u32 x1=base+static_cast<u32>(static_cast<s32>(toggled)*0x21+4);
    if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0x114u,x1,0x20u,0x10u);
}

// 09790..09B49: apply one party status/equipment effect. The two old words,
// F218-derived values, A5/80 special cases, address-distinct F238 refreshes,
// slot redraw, sound, and optional timed-event installation retain binary order.
[[nodiscard]] u32 function_09790(Machine& m,Application14Boundary& b,
                                  u32 effect,u32 slot,s32 sound_off,u32 mask,
                                  u32 deadline_index,u32 duration_a,u32 duration_b) noexcept {
    const u32 base=slot*0x248u;
    const s32 old0=static_cast<s16>(load_le16(dg(m,0x2193Cu+base)));
    const s32 old1=static_cast<s16>(load_le16(dg(m,0x2193Eu+base)));
    const u32 derived0=(b.call_f218?b.call_f218(slot,0x0Eu):0u)&0xFFu;
    const u32 derived1=(b.call_f218?b.call_f218(slot,0x0Fu):0u)&0xFFu;

    if(mask!=0xFFFFFFFFu&&(load_le32(dg(m,0x21AA2u+base))&mask)!=0u)return 0u;

    u32 result=1u;
    bool first=!(old0>=0x7E&&old0<=0x86);
    bool second=!(old1>=0x7E&&old1<=0x86);
    s32 selected=0;

    if(effect==0xA5u) {
        if(first&&app14_id_is_class_d(m,old0)&&old0!=static_cast<s32>(effect))first=false;
        if(second&&app14_id_is_class_d(m,old1)&&old1!=static_cast<s32>(effect))second=false;
    } else if(effect==0x80u) {
        if(!first||!second)return 0u;
        if(first&&old0!=-1)
            (void)function_096e0(m,b,static_cast<u32>(old0),static_cast<u32>(old1),derived0,derived1,1u,slot);
        if(second&&old1!=-1)
            (void)function_096e0(m,b,static_cast<u32>(old0),static_cast<u32>(old1),derived0,derived1,0u,slot);
        selected=1;
    } else {
        if(old0==-1)selected=0;
        else if(old1==-1)selected=1;
        else selected=static_cast<s32>(function_096e0(m,b,static_cast<u32>(old0),static_cast<u32>(old1),derived0,derived1,first?1u:0u,slot));
    }

    if(selected==1) {
        for(u32 i=0;i<4u;++i)*dg(m,0x6FA0u+slot*4u+i)=0xFFu;
    }

    if(effect==0x80u) {
        store_le16(dg(m,0x2193Cu+base),static_cast<u16>(effect));
        store_le16(dg(m,0x2193Eu+base),static_cast<u16>(effect));
        if(b.call_f238){b.call_f238(slot,0x0Eu,0u);b.call_f238(slot,0x0Fu,0u);}
    } else if(effect==0xA5u) {
        if(!first&&!second)result=0u;
        if(first) {
            store_le16(dg(m,0x2193Cu+base),static_cast<u16>(effect));
            if(b.call_f238)b.call_f238(slot,0x0Eu,0u);
        }
        if(second) {
            store_le16(dg(m,0x2193Eu+base),static_cast<u16>(effect));
            if(b.call_f238)b.call_f238(slot,0x0Fu,0u);
        }
    } else {
        store_le16(dg(m,0x2193Cu+base+static_cast<u32>(selected)*2u),static_cast<u16>(effect));
        if(b.call_f238)b.call_f238(slot,static_cast<u32>(selected+0x0E),0u);
        result=1u;
    }

    if(b.call_1ea90) {
        b.call_1ea90(slot,static_cast<u32>(selected),0xFFFFFFFFu);
        b.call_1ea90(slot,app14_toggle_low8(static_cast<u32>(selected)),0xFFFFFFFFu);
    }
    if(sound_off>=0) {
        if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+static_cast<u32>(sound_off),0xFFu,0u,1u);
        if(b.call_28168)b.call_28168();
    }
    app14_redraw_slot(m,b,static_cast<s32>(slot),selected);

    if(mask!=0xFFFFFFFFu) {
        store_le32(dg(m,0x21AA2u+base),load_le32(dg(m,0x21AA2u+base))|mask);
        const u32 delta=duration_a*duration_b;
        const u32 now=b.call_19cc4?b.call_19cc4():0u;
        store_le32(dg(m,0x21AA6u+base+deadline_index*4u),now+delta);
    }
    return result;
}
static_assert(0x09B4Au-0x09790u==954u);
