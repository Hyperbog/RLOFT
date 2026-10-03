
// -----------------------------------------------------------------------------
// Batch 13: 08CF4..094DC moving-record collision/effect resolution, party-hit
// propagation, proximity-gated sound playback, and pointer-selected angle input.
// 08CD8..08CF3 and 0934C..09367 are directly decoded CS dispatch tables.
// -----------------------------------------------------------------------------

struct Application13Boundary {
    u32 (*call_1c328)(u32,u32,u32,u32)=nullptr;
    void (*call_e284)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_23f50)(u32)=nullptr;
    s32 (*call_14a60)(u32)=nullptr;
    u32 (*call_f2d4)(u32,u32)=nullptr;
    u32 (*call_cf2c)(u32,u32)=nullptr;
    void (*call_cd44)(u32,u32)=nullptr;
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    void (*call_1c21c)(u32)=nullptr;
    void (*call_1c258)()=nullptr;
    void (*call_1a9f8)()=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    void (*call_1c2cc)(u32)=nullptr;
    void (*call_22d70)()=nullptr;
};

static u32 app13_owner_multiplier(Machine& m,u32 moving,bool owner7_is_five) noexcept {
    const u32 owner=(load_le16(dg(m,moving+0x14u))>>11)&7u;
    if(owner==4u)return 3u;
    if(owner7_is_five&&owner==7u)return 5u;
    const u32 base=owner*0x248u;
    u32 result=1u;
    if(static_cast<s8>(*dg(m,0x218F5u+base))==4)
        result=static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x21900u+base)))));
    if(static_cast<s8>(*dg(m,0x218F6u+base))==4)
        result=static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x21902u+base)))));
    if(static_cast<s8>(*dg(m,0x218F7u+base))==4)
        result=static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x21904u+base)))));
    return result;
}

static bool app13_mode_filter(Machine& m,u32 moving_type,u32 candidate_def) noexcept {
    const u8 f=*dg(m,candidate_def+0x32u);
    switch(moving_type) {
        case 0u:return (f&0x21u)!=0u;
        case 1u:return (f&0x03u)!=0u;
        case 2u:return (f&0x09u)!=0u;
        case 3u:return (f&0x09u)!=0u;
        case 4u:return (f&0x21u)!=0u;
        case 5u:return (f&0x01u)!=0u;
        case 6u:return (f&0x01u)!=0u;
        default:return false;
    }
}

// 08CF4..0906C: scan active 16h-byte world records whose type is below 22h or
// exactly 50h and whose radius box contains the moving record. A geometric hit
// clears the result even when the mode-specific type filter suppresses E284.
[[nodiscard]] u32 function_08cf4(Machine& m,Application13Boundary& b,u32 moving,u32 mode) noexcept {
    const u32 moving_type=load_le16(dg(m,moving+0x0Eu));
    const u32 cfg=0x66C8u+moving_type*4u;
    const u32 multiplier=mode!=0u?app13_owner_multiplier(m,moving,false):1u;
    u32 result=1u;
    const s32 count=static_cast<s16>(load_le16(dg(m,0x258A2u)));
    for(s32 i=0;i<count;++i) {
        const u32 rec=0x14D80u+static_cast<u32>(i)*0x16u;
        if((*dg(m,rec+0x0Bu)&2u)!=0u)continue;
        const u32 candidate_type=load_le16(dg(m,rec+0x0Eu));
        if(candidate_type>=0x22u&&candidate_type!=0x50u)continue;
        const u32 candidate_def=load_le32(dg(m,0x22230u))+candidate_type*0x45u;
        const u32 radius=load_le16(dg(m,candidate_def+0x16u));
        const u32 cy=load_le32(dg(m,rec+4u)),cx=load_le32(dg(m,rec));
        const u32 my=load_le32(dg(m,moving+4u)),mx=load_le32(dg(m,moving));
        if(my<cy-radius||cy+radius<my||mx<cx-radius||cx+radius<mx)continue;
        result=0u;

        if(mode!=0u) {
            if(!app13_mode_filter(m,moving_type,candidate_def))continue;
            u32 amount=b.call_1c328?b.call_1c328(*dg(m,candidate_def),*dg(m,candidate_def+1u),*dg(m,cfg+2u),0u):0u;
            if(*dg(m,cfg+3u)!=0u)amount*=multiplier;
            if(b.call_e284)b.call_e284(rec,candidate_def,static_cast<u32>(i),amount,0u);
            continue;
        }

        const u32 moving_def=load_le32(dg(m,0x255D8u))+moving_type*0x41u;
        if(moving_type==0x5Au) {
            const u32 amount=b.call_1c328?b.call_1c328(*dg(m,moving_def+0x38u),*dg(m,moving_def+0x39u),*dg(m,moving_def+0x3Au),0u):0u;
            if(b.call_e284)b.call_e284(rec,candidate_def,static_cast<u32>(i),amount,1u);
            if((*dg(m,candidate_def+0x32u)&3u)==0u&&b.call_e284)
                b.call_e284(rec,candidate_def,static_cast<u32>(i),0x14u,0u);
            *dg(m,moving+0x15u)=static_cast<u8>((*dg(m,moving+0x15u)&0x3Fu)|0x40u);
            continue;
        }

        if(!b.call_23f50||b.call_23f50(moving_type)==0u||*dg(m,moving_def+0x38u)==0u)continue;
        u32 amount=b.call_1c328?b.call_1c328(*dg(m,moving_def+0x38u),*dg(m,moving_def+0x39u),*dg(m,moving_def+0x3Au),*dg(m,moving_def+0x37u)):0u;
        if(moving_type==0x58u&&(*dg(m,candidate_def+0x3Au)&0x20u)!=0u) {
            *dg(m,moving+0x15u)=static_cast<u8>((*dg(m,moving+0x15u)&0x3Fu)|0x40u);
            amount=static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,rec+0x12u)))));
        }
        if(b.call_e284)b.call_e284(rec,candidate_def,static_cast<u32>(i),amount,1u);
    }
    return result;
}
static_assert(0x0906Du-0x08CF4u==889u);

static u32 app13_half_signed(u32 v) noexcept {
    return static_cast<u32>(static_cast<s32>(v)/2);
}

// 09070..09349: apply one moving record against the four party slots. In mode
// zero, types 57h/59h clear the encoded bit and return before the proximity box.
void function_09070(Machine& m,Application13Boundary& b,u32 moving,u32 mode) noexcept {
    const u32 moving_type=load_le16(dg(m,moving+0x0Eu));
    u32 cfg=0u,multiplier=1u;
    if(mode==0u&&(moving_type==0x59u||moving_type==0x57u)) {
        *dg(m,moving+0x15u)=static_cast<u8>(*dg(m,moving+0x15u)&0xFDu);
        return;
    }
    if(mode!=0u) {
        cfg=0x66C8u+moving_type*4u;
        multiplier=app13_owner_multiplier(m,moving,true);
    }

    const u32 p0=load_le32(dg(m,0x255D0u)),p4=load_le32(dg(m,0x255D4u));
    const u32 v4=load_le32(dg(m,moving+4u)),v0=load_le32(dg(m,moving));
    if(v4<p0-0x8FA0u||p0+0x8FA0u<v4||v0<p4-0x8FA0u||p4+0x8FA0u<v0)return;

    for(u32 slot=0u,base=0u;slot<4u;++slot,base+=0x248u) {
        if((load_le16(dg(m,0x218F0u+base))&0xF240u)!=0u)continue;
        if(b.call_14a60&&b.call_14a60(0x510u)==static_cast<s32>(slot))continue;

        if(mode!=0u) {
            u32 amount=b.call_1c328?b.call_1c328(*dg(m,cfg),*dg(m,cfg+1u),*dg(m,cfg+2u),0u):0u;
            if(*dg(m,cfg+3u)!=0u)amount*=multiplier;
            u32 flavor=0u;
            bool stronger=false;
            if(moving_type==0u) {
                stronger=(b.call_f2d4&&b.call_f2d4(slot,0x44u)!=0u)||((*dg(m,0x21A14u+base)&8u)!=0u);
            } else if(moving_type==1u) {
                stronger=static_cast<s16>(load_le16(dg(m,0x2193Cu+base)))==0x67||
                         static_cast<s16>(load_le16(dg(m,0x2193Eu+base)))==0x67;
            }
            if(stronger) {
                const u32 middle=static_cast<u32>(*dg(m,cfg+1u))-2u;
                amount=b.call_1c328?b.call_1c328(*dg(m,cfg),middle,*dg(m,cfg+2u),0u):0u;
                amount*=multiplier;
                flavor=2u;
            }
            if(b.call_cf2c&&b.call_cf2c(slot,flavor)!=0u)amount=app13_half_signed(amount);
            if(amount!=0u&&b.call_cd44)b.call_cd44(slot,amount);
        } else {
            const u32 def=load_le32(dg(m,0x255D8u))+moving_type*0x41u;
            u32 amount=0u;
            if(*dg(m,def+0x38u)!=0u)
                amount=b.call_1c328?b.call_1c328(*dg(m,def+0x38u),*dg(m,def+0x39u),*dg(m,def+0x3Au),0u):0u;
            else
                amount=b.call_1c328?b.call_1c328(1u,2u,0u,0u):0u;
            if(b.call_cd44)b.call_cd44(slot,amount);
            *dg(m,moving+0x15u)=static_cast<u8>(*dg(m,moving+0x15u)&0xFDu);
        }
    }
}
static_assert(0x0934Au-0x09070u==730u);

// 09368..09417: choose one sound offset from the seven-entry 0934C CS table,
// then gate distant playback by orientation bits and tile visibility.
void function_09368(Machine& m,Application13Boundary& b,u32 moving) noexcept {
    const s32 dy=static_cast<s32>(static_cast<s16>(load_le16(dg(m,moving+2u))))-
                 static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))));
    const s32 dx=static_cast<s32>(static_cast<s16>(load_le16(dg(m,moving+6u))))-
                 static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))));
    const s32 ady=dy<0?-dy:dy,adx=dx<0?-dx:dx;
    s32 sound=-1;
    switch(load_le16(dg(m,moving+0x0Eu))) {
        case 0u:sound=0x1C;break;
        case 1u:sound=0x37;break;
        case 4u:sound=0x1E;break;
        case 6u:sound=0x39;break;
        default:break;
    }
    const u16 phase=static_cast<u16>(load_le16(dg(m,moving+0x14u))&0x3800u);
    if(phase>=0x2000u) {
        const u32 tile=load_le16(dg(m,moving+0x0Cu));
        if((*dg(m,0x198E0u+tile*2u)&0x40u)==0u&&(ady>=10||adx>=10))return;
    }
    if(sound!=-1&&b.call_28110)
        b.call_28110(load_le32(dg(m,0x258D4u))+static_cast<u32>(sound),0xFFu,1u,1u);
}
static_assert(0x09418u-0x09368u==176u);

// 09418..094DC: modal pointer selection. With mode!=0, Y rotates C658 in
// signed thirds around A0h; otherwise the returned value is Y-20h.
[[nodiscard]] s32 function_09418(Machine& m,Application13Boundary& b,u32 mode) noexcept {
    if(*dg(m,0xC02Fu)!=0u)
        return mode!=0u?static_cast<s32>(load_le32(dg(m,0xC658u))):0x80;
    if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x66A8u)));
    if(b.call_1c258)b.call_1c258();
    if(b.call_1a9f8)b.call_1a9f8();
    for(;;) {
        if(b.call_1a1f4)while(b.call_1a1f4()!=0u){}
        if(b.call_1a1f4)while(b.call_1a1f4()==0u){}
        const s32 x=static_cast<s16>(load_le16(dg(m,0x9E31u)));
        const s32 y=static_cast<s16>(load_le16(dg(m,0x9E33u)));
        if(x<4||x>=0x104||y<0x20||y>=0x120)continue;
        s32 result=0;
        if(mode!=0u) {
            const s32 q=(y-0xA0)/3;
            result=static_cast<s32>(load_le32(dg(m,0xC658u)))-q;
            if(result<0)result+=0x168;
            if(result>=0x168)result-=0x168;
        } else result=y-0x20;
        if(b.call_1c2cc)b.call_1c2cc(static_cast<u32>(result));
        if(b.call_22d70)b.call_22d70();
        return result;
    }
}
static_assert(0x094DDu-0x09418u==197u);
