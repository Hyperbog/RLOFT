// -----------------------------------------------------------------------------
// Batch 16: 0A794..0B515 status wrappers, map-object helpers, event clearing,
// nearby-object effects, projectile spawning, and moving-record conversion.
// -----------------------------------------------------------------------------

struct Application16Boundary {
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    void (*call_1f494)(u32)=nullptr;
    void (*call_09b4c)(s32,u32,u32)=nullptr;
    u32 (*call_22e40)(u32,u32)=nullptr;
    u32 (*call_19cc4)()=nullptr;
    u32 (*call_1202c)(u32,u32)=nullptr;
    u32 (*call_1e8c8)(u32,u32)=nullptr;
    void (*call_20c0c)(u32,u32,u32)=nullptr;
    u32 (*call_20cf4)(u32)=nullptr;
    void (*call_002a8)(s32)=nullptr;
    u32 (*call_1c924)(u32)=nullptr;
    void (*call_1c77c)(u32)=nullptr;
    void (*call_0871c)(u32,u32)=nullptr;
    void (*call_086c4)(u32,u32)=nullptr;
    void (*call_0893c)()=nullptr;
    void (*call_2df2c)(u32*,u32*,u32,u32)=nullptr;
    u32 (*call_2d5d0)(u32,u32,s32)=nullptr;
    void (*call_1f1c8)()=nullptr;
    void (*call_1ea78)()=nullptr;
    u32 (*call_1c328)(u32,u32,u32,u32)=nullptr;
    u32 (*call_2acdc)(u32)=nullptr;
    void (*call_e284)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_30ca2)()=nullptr;
    u32 (*call_2c9dc)(u32,u32)=nullptr;
    u32 (*call_1e930)(u32)=nullptr;
    void (*call_28168)()=nullptr;
    void (*call_08c88)(s32)=nullptr;
};

static void app16_dirty_all(Machine& m) noexcept {
    for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
}

static void app16_suspend_ui(Machine& m,Application16Boundary& b) noexcept {
    app16_dirty_all(m);
    store_le32(dg(m,0x12BA0u),static_cast<u32>(*dg(m,0xD681u)));
    *dg(m,0xD681u)=0u;
    store_le32(dg(m,0x12BA4u),static_cast<u32>(*dg(m,0xD682u)));
    *dg(m,0xD682u)=0u;
    if(b.call_1f494)b.call_1f494(1u);
}

static void app16_restore_ui(Machine& m,Application16Boundary& b) noexcept {
    *dg(m,0xD681u)=*dg(m,0x12BA0u);
    *dg(m,0xD682u)=*dg(m,0x12BA4u);
    if(*dg(m,0xD681u)!=0u)*dg(m,0xD681u)=2u;
    if(*dg(m,0xD682u)!=0u)*dg(m,0xD682u)=2u;
    app16_dirty_all(m);
    if(b.call_1f494)b.call_1f494(1u);
}

void function_0a794(Machine& m,Application16Boundary& b,u32 slot) noexcept {
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x30u,0xFFu,0u,1u);
    app16_suspend_ui(m,b);
    if(b.call_09b4c)b.call_09b4c(static_cast<s32>(slot),2u,1u);
    app16_restore_ui(m,b);
    const u32 target=slot*0x248u;
    for(u32 party=0u,base=0u;party<4u;++party,base+=0x248u) {
        if((load_le16(dg(m,0x218F0u+base))&0x8000u)!=0u)continue;
        *dg(m,0xCF5Au+party)=1u;
        *dg(m,0x21A12u+target)=static_cast<u8>(*dg(m,0x21A12u+target)|0x20u);
    }
}
static_assert(0x0A8A0u-0x0A794u==268u);

u32 function_0a8a0(Machine& m,Application16Boundary& b,u32 slot) noexcept {
    const u32 base=slot*0x248u;
    if((*dg(m,0x21A13u+base)&0x10u)!=0u)return 0u;
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x1Au,0xFFu,0u,1u);
    app16_suspend_ui(m,b);
    if(b.call_09b4c)b.call_09b4c(static_cast<s32>(slot),2u,0u);
    app16_restore_ui(m,b);
    *dg(m,0x21A13u+base)=static_cast<u8>(*dg(m,0x21A13u+base)|0x10u);
    const u32 first=b.call_22e40?b.call_22e40(3u,slot):0u;
    const u32 now=b.call_19cc4?b.call_19cc4():0u;
    store_le32(dg(m,0x21A46u+base),now+first*0x438u);
    const s32 rolled=static_cast<s32>(b.call_22e40?b.call_22e40(3u,slot):0u);
    s32 delta=rolled/3+1;
    const s32 current=static_cast<s32>(*dg(m,0x21912u+base));
    const s32 cap=*dg(m,0x218F4u+base)==2u?0x11:0x12;
    if(current+delta>cap)delta=cap-current;
    const u32 applied=b.call_1202c?b.call_1202c(slot,static_cast<u32>(delta)):0u;
    store_le32(dg(m,0x21A03u+base),applied);
    return 1u;
}
static_assert(0x0AA2Du-0x0A8A0u==397u);

u32 function_0aa30(Machine& m,Application16Boundary& b,u32 slot) noexcept {
    const u32 base=slot*0x248u;
    if((*dg(m,0x21A13u+base)&0x20u)!=0u)return 0u;
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x19u,0xFFu,0u,1u);
    app16_suspend_ui(m,b);
    if(b.call_09b4c)b.call_09b4c(static_cast<s32>(slot),2u,0u);
    app16_restore_ui(m,b);
    *dg(m,0x21A13u+base)=static_cast<u8>(*dg(m,0x21A13u+base)|0x20u);
    const u32 first=b.call_22e40?b.call_22e40(3u,slot):0u;
    const u32 now=b.call_19cc4?b.call_19cc4():0u;
    store_le32(dg(m,0x21A4Au+base),now+first*0x438u);
    const s32 rolled=static_cast<s32>(b.call_22e40?b.call_22e40(3u,slot):0u);
    s32 delta=rolled/3+1;
    const s32 current=static_cast<s32>(*dg(m,0x21915u+base));
    const u8 cls=*dg(m,0x218F4u+base);
    const s32 cap=(cls==2u||cls==5u)?0x13:0x12;
    if(current+delta>cap)delta=cap-current;
    store_le32(dg(m,0x21A07u+base),static_cast<u32>(delta));
    *dg(m,0x21915u+base)=static_cast<u8>(*dg(m,0x21915u+base)+static_cast<u8>(delta));
    return 1u;
}
static_assert(0x0ABC4u-0x0AA30u==404u);

static void app16_div_remainders(Machine& m,u32 table_index,s32& rx,s32& ry) noexcept {
    const s32 packed=static_cast<s32>(load_le32(dg(m,0x15A2Cu+table_index*0x1Cu)));
    const s32 width=static_cast<s16>(load_le16(dg(m,0x25619u)));
    const s32 height=static_cast<s16>(load_le16(dg(m,0x2561Bu)));
    rx=packed%width;
    ry=packed%height;
}

void function_0abc4(Machine& m,Application16Boundary& b,u32 table_index) noexcept {
    s32 rx=0,ry=0;
    app16_div_remainders(m,table_index,rx,ry);
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x49u,0xFFu,0u,1u);
    const u32 obj=b.call_1e8c8?b.call_1e8c8(static_cast<u32>(rx),static_cast<u32>(ry)):0u;
    if(obj!=0u&&*dg(m,obj)==1u)*dg(m,obj+1u)=static_cast<u8>(*dg(m,obj+1u)|0x40u);
}
static_assert(0x0AC2Cu-0x0ABC4u==104u);

void function_0ac2c(Machine& m,Application16Boundary& b,u32 table_index) noexcept {
    s32 rx=0,ry=0;
    app16_div_remainders(m,table_index,rx,ry);
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x32u,0xFFu,0u,1u);
    const u32 obj=b.call_1e8c8?b.call_1e8c8(static_cast<u32>(rx),static_cast<u32>(ry)):0u;
    if(obj==0u)return;
    const u8 type=*dg(m,obj);
    if(type==1u) {
        if((*dg(m,obj+1u)&0x40u)!=0u) {
            *dg(m,obj+1u)=static_cast<u8>(*dg(m,obj+1u)&0xBFu);
            return;
        }
        if(*dg(m,obj+6u)==0u)return;
        const u8 saved_bit2=static_cast<u8>((*dg(m,obj+1u)>>2)&1u);
        *dg(m,obj+1u)=static_cast<u8>(*dg(m,obj+1u)|0x04u);
        if(b.call_20c0c)b.call_20c0c(static_cast<u32>(rx),static_cast<u32>(ry),obj);
        *dg(m,obj+1u)=static_cast<u8>((*dg(m,obj+1u)&0xFBu)|(saved_bit2<<2));
        return;
    }
    if(type!=4u)return;
    const u32 active=b.call_20cf4?b.call_20cf4(*dg(m,obj+6u)):0u;
    if(active!=0u) {
        if(b.call_002a8)b.call_002a8(static_cast<s8>(*dg(m,obj+2u)));
        *dg(m,obj+1u)=static_cast<u8>(*dg(m,obj+1u)&0xFBu);
        return;
    }
    const u32 token=b.call_1c924?b.call_1c924(0x320u):0u;
    if(b.call_1c77c)b.call_1c77c(token);
}
static_assert(0x0AD24u-0x0AC2Cu==248u);

void function_0ad24(Machine& m,Application16Boundary& b,s32 selected) noexcept {
    const u32 begin=selected==-1?0u:static_cast<u32>(selected);
    const u32 end=selected==-1?4u:static_cast<u32>(selected+1);
    for(u32 slot=begin;slot<end;++slot) {
        const u32 base=slot*0x248u;
        if((load_le16(dg(m,0x218F0u+base))&0x8000u)!=0u)continue;
        for(u32 bit=0;bit<26u;++bit) {
            if((load_le32(dg(m,0x21A12u+base))&(1u<<(bit&31u)))!=0u&&b.call_0871c)
                b.call_0871c(slot,bit);
        }
    }
    for(u32 slot=begin;slot<end;++slot) {
        const u32 base=slot*0x248u;
        if((load_le16(dg(m,0x218F0u+base))&0x8000u)!=0u)continue;
        for(u32 event=0;event<9u;++event) {
            const u32 mask=1u<<(event&31u);
            if((load_le32(dg(m,0x21AA2u+base))&mask)==0u)continue;
            store_le32(dg(m,0x21AA2u+base),load_le32(dg(m,0x21AA2u+base))^mask);
            *dg(m,0xCF5Au+slot)=1u;
            store_le32(dg(m,0x21AA6u+base+event*4u),0u);
            if(b.call_086c4)b.call_086c4(slot,event);
        }
    }
    if(b.call_0893c)b.call_0893c();
}
static_assert(0x0AE2Du-0x0AD24u==265u);

void function_0ae30(Machine& m,Application16Boundary& b) noexcept {
    const u32 saved0=load_le32(dg(m,0x255D0u));
    const u32 saved4=load_le32(dg(m,0x255D4u));
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x48u,0xFFu,0u,1u);
    for(;;) {
        u32 d0=0u,d4=0u;
        if(b.call_2df2c)b.call_2df2c(&d0,&d4,0x5000u,load_le32(dg(m,0xC658u)));
        store_le32(dg(m,0x255D0u),load_le32(dg(m,0x255D0u))+d0);
        store_le32(dg(m,0x255D4u),load_le32(dg(m,0x255D4u))+d4);
        const u32 blocked=b.call_2d5d0?b.call_2d5d0(load_le32(dg(m,0x255D4u)),load_le32(dg(m,0x255D0u)),-2):0u;
        if(static_cast<u16>(blocked)==0u)break;
        if(b.call_1f1c8)b.call_1f1c8();
        if(b.call_1ea78)b.call_1ea78();
    }
    store_le32(dg(m,0x255D0u),saved0);
    store_le32(dg(m,0x255D4u),saved4);
    if(b.call_1f1c8)b.call_1f1c8();
    if(b.call_1ea78)b.call_1ea78();
}
static_assert(0x0AED7u-0x0AE30u==167u);

void function_0aed8(Machine& m,Application16Boundary& b) noexcept {
    s32 budget=static_cast<s32>(b.call_1c328?b.call_1c328(4u,0x14u,0u,0u):0u);
    const s32 y0=static_cast<s16>(load_le16(dg(m,0x255D2u)))-1;
    const s32 x0=static_cast<s16>(load_le16(dg(m,0x255D6u)))-1;
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x12u,0xFFu,0u,1u);
    const s32 width=static_cast<s16>(load_le16(dg(m,0x25619u)));
    const s32 height=static_cast<s16>(load_le16(dg(m,0x2561Bu)));
    const s32 last=static_cast<s16>(load_le16(dg(m,0x258A2u)));
    for(s32 y=y0;y<y0+2;++y) {
        if(y<0||y>=height)continue;
        for(s32 x=x0;x<x0+2;++x) {
            if(x<0||x>=width)continue;
            const u32 tile=static_cast<u32>(y*width+x);
            for(s32 i=0;i<last;++i) {
                const u32 rec=0x14D80u+static_cast<u32>(i)*0x16u;
                if((*dg(m,rec+0x0Bu)&0x02u)!=0u)continue;
                const u32 type=load_le16(dg(m,rec+0x0Eu));
                if((b.call_2acdc?b.call_2acdc(type):0u)==0u||type==8u)continue;
                if(load_le16(dg(m,rec+0x0Cu))!=tile)continue;
                if(budget<=0)return;
                const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
                if((*dg(m,desc+0x31u)&0x80u)!=0u)continue;
                const u32 amount=static_cast<u32>(static_cast<s16>(load_le16(dg(m,rec+0x12u)))+1);
                if(b.call_e284)b.call_e284(rec,desc,static_cast<u32>(i),amount,0u);
                --budget;
            }
        }
    }
}
static_assert(0x0B02Cu-0x0AED8u==340u);

static s32 app16_wrap_angle(s32 angle) noexcept {
    while(angle<0)angle+=0x168;
    while(angle>=0x168)angle-=0x168;
    return angle;
}

static s16 app16_hiword(u32 v) noexcept { return static_cast<s16>(v>>16); }

void function_0b02c(Machine& m,Application16Boundary& b,s32 angle_first,s32 angle_last,
                    s32 max_steps,u32 callback_value,u32 descriptor_mask,u32 duplicate_guard,
                    u32 moving_type,s32 sound_offset,u32 initial_y,u32 initial_x) noexcept {
    const u8 saved_c02f=*dg(m,0xC02Fu);
    *dg(m,0xC02Fu)=0u;
    for(s32 angle=angle_first;angle<=angle_last;++angle) {
        const u32 rnd=b.call_30ca2?b.call_30ca2():0u;
        const u32 orientation_seed=(rnd*3u)>>15;
        const s32 heading=app16_wrap_angle(static_cast<s32>(load_le32(dg(m,0xC658u)))+angle);
        u32 dy=0u,dx=0u;
        if(b.call_2df2c)b.call_2df2c(&dy,&dx,0xA000u,static_cast<u32>(heading));
        u32 x=initial_x;
        u32 y=initial_y;
        while((b.call_2c9dc?b.call_2c9dc(x,y):0u)!=0u) { x+=dx; y+=dy; }
        const s16 height=static_cast<s16>(load_le16(dg(m,0x2561Bu)));
        const s16 width=static_cast<s16>(load_le16(dg(m,0x25619u)));
        if(app16_hiword(x)>height||app16_hiword(y)>width) { *dg(m,0xC02Fu)=saved_c02f; return; }
        bool blocked=false;
        for(s32 step=0;step<=max_steps;++step) {
            const u32 orient=(orientation_seed&7u)<<6;
            x+=dx; y+=dy;
            const s16 hy=app16_hiword(x);
            const s16 hx=app16_hiword(y);
            if(hy>=height||hx>=width)continue;
            const u32 tile=static_cast<u32>(hy*width+hx);
            const u8 tile_flags=*dg(m,0x198E0u+tile*2u);
            if((tile_flags&0x80u)!=0u) {
                if(static_cast<u16>(b.call_1e930?b.call_1e930(tile):0u)!=0u)blocked=true;
            } else if((tile_flags&0x3Fu)!=0u)blocked=true;
            if(blocked)break;

            u32 record_index=load_le32(dg(m,0xC038u));
            if(duplicate_guard!=0u) {
                record_index=0u;
                const u32 count=load_le32(dg(m,0xC038u));
                for(;record_index<count;++record_index) {
                    if(load_le16(dg(m,0x248F8u+record_index*0x16u))==static_cast<u16>(tile))break;
                }
            }
            if(record_index!=load_le32(dg(m,0xC038u)))continue;

            const u32 rec=0x248ECu+record_index*0x16u;
            u32 stored_x=x,stored_y=y;
            if(duplicate_guard!=0u) {
                stored_x=(stored_x&0xFFFF0000u)|0x8000u;
                stored_y=(stored_y&0xFFFF0000u)|0x8000u;
            }
            store_le32(dg(m,rec),stored_x);
            store_le32(dg(m,rec+4u),stored_y);
            store_le16(dg(m,rec+0x0Cu),static_cast<u16>(tile));
            store_le16(dg(m,rec+0x0Eu),static_cast<u16>(moving_type));
            store_le16(dg(m,rec+0x14u),static_cast<u16>((load_le16(dg(m,rec+0x14u))&0xFE3Fu)|static_cast<u16>(orient)));
            if(moving_type==8u) {
                const u32 r=b.call_30ca2?b.call_30ca2():0u;
                const u16 ro=static_cast<u16>((((r*3u)>>15)&7u)<<6);
                store_le16(dg(m,rec+0x14u),static_cast<u16>((load_le16(dg(m,rec+0x14u))&0xFE3Fu)|ro));
            }
            store_le32(dg(m,0xC038u),load_le32(dg(m,0xC038u))+1u);

            const s32 world_last=static_cast<s16>(load_le16(dg(m,0x258A2u)));
            for(s32 i=0;i<=world_last;++i) {
                const u32 wrec=0x14D80u+static_cast<u32>(i)*0x16u;
                if((*dg(m,wrec+0x0Bu)&0x02u)!=0u)continue;
                if(load_le16(dg(m,wrec+0x0Cu))!=static_cast<u16>(tile))continue;
                if((*dg(m,wrec+0x0Bu)&0x10u)==0u)continue;
                if(load_le16(dg(m,wrec+0x12u))==0u)continue;
                const u32 type=load_le16(dg(m,wrec+0x0Eu));
                const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
                if(moving_type==0x0Bu) {
                    if(type!=8u||static_cast<s16>(load_le16(dg(m,wrec+0x12u)))>10)continue;
                    store_le16(dg(m,wrec+0x12u),static_cast<u16>(load_le16(dg(m,wrec+0x12u))-2u));
                    if(b.call_e284)b.call_e284(wrec,desc,static_cast<u32>(i),0u,0u);
                } else {
                    if((load_le32(dg(m,desc+0x31u))&descriptor_mask)!=0u)continue;
                    if(b.call_e284)b.call_e284(wrec,desc,static_cast<u32>(i),callback_value,0u);
                }
            }
        }
    }

    u32 sound=0u;
    if(moving_type==8u||moving_type==0u)sound=load_le32(dg(m,0x258BCu))+0x19u;
    else sound=load_le32(dg(m,0x258D4u))+static_cast<u32>(sound_offset);
    if(b.call_28110)b.call_28110(sound,0xFFu,0u,1u);
    if(b.call_1f1c8)b.call_1f1c8();
    if(b.call_1ea78)b.call_1ea78();
    if(b.call_28168)b.call_28168();
    for(u32 i=0;i<3u;++i) {
        if(b.call_1f1c8)b.call_1f1c8();
        if(b.call_1ea78)b.call_1ea78();
    }
    for(;;) {
        const u32 count=load_le32(dg(m,0xC038u));
        u32 i=0u;
        for(;i<count;++i)if(load_le16(dg(m,0x248FAu+i*0x16u))==static_cast<u16>(moving_type))break;
        if(i==count)break;
        if(b.call_08c88)b.call_08c88(static_cast<s32>(i));
        else break;
    }
    *dg(m,0xC02Fu)=saved_c02f;
}
static_assert(0x0B40Eu-0x0B02Cu==994u);

void function_0b410(Machine& m,Application16Boundary& b,u32 index) noexcept {
    const u32 wrec=0x14D80u+index*0x16u;
    const u32 moving_index=load_le32(dg(m,0xC038u));
    const u32 rec=0x248ECu+moving_index*0x16u;
    store_le16(dg(m,rec+0x0Eu),0x0Au);
    store_le32(dg(m,rec+4u),load_le32(dg(m,wrec+4u)));
    store_le32(dg(m,rec),load_le32(dg(m,wrec)));
    store_le16(dg(m,rec+0x0Cu),load_le16(dg(m,wrec+0x0Cu)));
    store_le32(dg(m,0xC038u),moving_index+1u);
    const u32 roll=b.call_1c328?b.call_1c328(6u,8u,0u,0u):0u;
    const u32 type=load_le16(dg(m,wrec+0x0Eu));
    const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
    if((*dg(m,desc+0x32u)&0x21u)==0u&&b.call_e284)b.call_e284(wrec,desc,index,roll,0u);
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x20u,0xFFu,0u,1u);
    if(b.call_1f1c8)b.call_1f1c8();
    if(b.call_1ea78)b.call_1ea78();
    if(b.call_28168)b.call_28168();
    if(b.call_1f1c8)b.call_1f1c8();
    if(b.call_1ea78)b.call_1ea78();
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x21u,0xFFu,0u,1u);
    for(u32 i=0;i<2u;++i) {
        if(b.call_28168)b.call_28168();
        if(b.call_1f1c8)b.call_1f1c8();
        if(b.call_1ea78)b.call_1ea78();
    }
    if(b.call_28168)b.call_28168();
    if(b.call_08c88)b.call_08c88(static_cast<s32>(load_le32(dg(m,0xC038u))-1u));
}
static_assert(0x0B516u-0x0B410u==262u);
