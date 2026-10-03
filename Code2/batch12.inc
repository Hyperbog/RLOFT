
// -----------------------------------------------------------------------------
// Batch 12: 080AC..08CD7 left-panel dispatcher, status expirations,
// timed-effect sweeper, moving-record constructor, and record compaction.
// 0808C..080AB and 086A0..086C3 are directly decoded CS/data tables.
// -----------------------------------------------------------------------------

struct Application12Boundary {
    u32 (*call_19ccc)(u32)=nullptr;
    u32 (*call_19cf4)()=nullptr;
    u32 (*call_19cc4)()=nullptr;
    void (*call_1a9f8)()=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    void (*call_291b0)(u32)=nullptr;
    u32 (*call_30070)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_30040)(u32)=nullptr;
    void (*call_2975c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_30508)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_1a8c8)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_1a344)(s32,u32)=nullptr;
    u32 (*call_05c84)()=nullptr;
    u32 (*call_05d34)()=nullptr;
    void (*call_1f8fc)()=nullptr;
    void (*call_325b0)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_322ec)(u32,u32,u32,u32,u32,u32)=nullptr;
    void (*call_03634)(u32)=nullptr;
    void (*call_0461c)()=nullptr;
    u32 (*call_07b94)()=nullptr;
    void (*call_0803c)(u32)=nullptr;
    void (*call_171fc)(u32)=nullptr;
    u32 (*call_079e0)()=nullptr;
    u32 (*call_07d44)(u32)=nullptr;
    void (*call_205b4)(u32)=nullptr;
    void (*call_06548)()=nullptr;
    void (*call_05dd0)()=nullptr;
    void (*call_2939c)(u32,u32,u32,u32)=nullptr;
    void (*call_22d70)()=nullptr;

    void (*call_24a9c)(u32,u32)=nullptr;
    void (*call_3210f)(u32,u32,u32)=nullptr;
    void (*call_1c368)(u32,u32)=nullptr;
    void (*call_1c21c)(u32)=nullptr;
    void (*call_31f38)(u32)=nullptr;
    void (*call_1202c)(u32,u32)=nullptr;
    u32 (*call_12098)(u32)=nullptr;
    u32 (*call_1c328)(u32,u32,u32,u32)=nullptr;

    void (*call_e578)(u32)=nullptr;
    u32 (*call_30ca2)()=nullptr;
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    u32 (*call_2c9dc)(u32,u32)=nullptr;
    void (*call_22e90)(u32,u32)=nullptr;
    u32 (*call_1f1c8)()=nullptr;
    void (*call_1ea04)()=nullptr;
    void (*call_1ea78)()=nullptr;
    void (*call_2f580)(u32)=nullptr;
    void (*call_30cd4)(u32,u32,u32)=nullptr;
};

static s32 app12_signed_row(Machine& m) noexcept {
    return static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x9E33u))))/0x28;
}

static void app12_draw_menu_row(Machine& m,Application12Boundary& b,s32 row,u32 sprite) noexcept {
    const u32 y=static_cast<u32>(row*0x28);
    if(b.call_2975c)b.call_2975c(sprite,0u,y,load_le32(dg(m,0x258DCu))+0x49u,0u);
    if(b.call_30508)b.call_30508(0x1Bu,y,0x28u,load_le32(dg(m,0x60F0u+static_cast<u32>(row)*4u)),1u);
}

static void app12_update_menu_row(Machine& m,Application12Boundary& b,s32 row) noexcept {
    if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0u,static_cast<u32>(row*0x28),0x40u,0x28u);
}

static void app12_refresh_menu_capture(Machine& m,Application12Boundary& b,u32& capture,bool overlay) noexcept {
    if(b.call_1f8fc)b.call_1f8fc();
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    if(b.call_30040)b.call_30040(capture);
    capture=b.call_30070?b.call_30070(0u,0u,0x40u,0x140u,0xFFu):0u;
    if(overlay) {
        if(b.call_325b0)b.call_325b0(0xC054u,0u,load_le32(dg(m,0xC034u)),0x390u,0x100u);
        if(b.call_322ec)b.call_322ec(0xC054u,0u,0xFFu,0u,0u,0u);
    }
}

// 080AC..0869E: eight-way left-panel dispatcher using the already-proven
// 0808C..080AB CS table. The two consecutive 1A1F4 polls before accepting a
// press, signed IDIV row selection, and per-case capture refresh asymmetry are
// retained exactly.
void function_080ac(Machine& m,Application12Boundary& b) noexcept {
    if(static_cast<s16>(load_le16(dg(m,0x9E31u)))>=0x40) {
        store_le16(dg(m,0xA6C2u),0u);
        return;
    }

    *dg(m,0xA79Du)=1u;
    if(b.call_19ccc)(void)b.call_19ccc(0u);
    if(b.call_1a9f8)b.call_1a9f8();
    const u8 saved_d680=*dg(m,0xD680u);
    *dg(m,0xD680u)=3u;
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    u32 capture=b.call_30070?b.call_30070(0u,0u,0x40u,0x140u,0xFFu):0u;
    bool redraw=true;
    s32 selected=0;

    for(;;) {
        if(static_cast<s16>(load_le16(dg(m,0x9E31u)))>=0x40)break;

        if(redraw) {
            selected=app12_signed_row(m);
            if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
            for(s32 row=0;row<8;++row)app12_draw_menu_row(m,b,row,row==selected?1u:0u);
            if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0u,0u,0x40u,0x140u);
            redraw=false;
        }

        for(;;) {
            const u32 first=b.call_1a1f4?b.call_1a1f4():0u;
            if(first!=0u)break;
            if(static_cast<s16>(load_le16(dg(m,0x9E31u)))>=0x40)break;
            const s32 next=app12_signed_row(m);
            if(next==selected)continue;
            const s32 old=selected;
            selected=next;
            if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
            app12_draw_menu_row(m,b,old,0u);
            app12_draw_menu_row(m,b,selected,1u);
            app12_update_menu_row(m,b,old);
            app12_update_menu_row(m,b,selected);
        }

        // 82B2 performs a second poll even after the first poll reported a press.
        if(!b.call_1a1f4||b.call_1a1f4()==0u)continue;

        const s32 old=selected;
        selected=app12_signed_row(m);
        if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
        app12_draw_menu_row(m,b,old,0u);
        app12_draw_menu_row(m,b,selected,2u);
        app12_update_menu_row(m,b,old);
        app12_update_menu_row(m,b,selected);

        const s32 action=app12_signed_row(m);
        if(static_cast<u32>(action)>7u)continue;

        switch(action) {
            case 0:
                if(b.call_0461c)b.call_0461c();
                if(b.call_1a1f4)while(b.call_1a1f4()!=0u){}
                continue;
            case 1:
            case 2: {
                *dg(m,0x6460u)=0u;
                const s32 saved_word=static_cast<s16>(load_le16(dg(m,0x8600u)));
                const u8 saved_byte=*dg(m,0x8602u);
                if(b.call_1a344)b.call_1a344(-1,0u);
                const u32 changed=(action==1)
                    ? (b.call_05c84?b.call_05c84():0u)
                    : (b.call_05d34?b.call_05d34():0u);
                if(changed!=0u) {
                    *dg(m,0xCF58u)=0u;
                    app12_refresh_menu_capture(m,b,capture,true);
                    redraw=true;
                }
                if(b.call_1a344)b.call_1a344(saved_word,saved_byte);
                continue;
            }
            case 3:
                redraw=true;
                if(b.call_03634)b.call_03634(0u);
                continue;
            case 4: {
                const u32 r=b.call_07b94?b.call_07b94():0u;
                if(r==1u&&b.call_0803c)b.call_0803c(load_le32(dg(m,0x60C4u)));
                app12_refresh_menu_capture(m,b,capture,false);
                redraw=true;
                continue;
            }
            case 5: {
                *dg(m,0xA79Du)=0u;
                if(b.call_171fc)b.call_171fc(load_le32(dg(m,0xC6C8u)));
                *dg(m,0xA79Du)=1u;
                const u32 r=b.call_079e0?b.call_079e0():0u;
                if(r==1u&&b.call_0803c)b.call_0803c(load_le32(dg(m,0x60C8u)));
                app12_refresh_menu_capture(m,b,capture,false);
                redraw=true;
                continue;
            }
            case 6: {
                const u32 r=b.call_07d44?b.call_07d44(load_le32(dg(m,0x60CCu))):0u;
                if(r==0u) {
                    app12_refresh_menu_capture(m,b,capture,false);
                    redraw=true;
                } else if(b.call_205b4)b.call_205b4(0xFFFFFFFFu);
                continue;
            }
            case 7:
                if(b.call_06548)b.call_06548();
                if(b.call_05dd0)b.call_05dd0();
                app12_refresh_menu_capture(m,b,capture,false);
                redraw=true;
                continue;
        }

    }

    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    if(b.call_2939c)b.call_2939c(0u,0u,0u,capture);
    if(b.call_30040)b.call_30040(capture);
    if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0u,0u,0x40u,0x140u);
    if(b.call_22d70)b.call_22d70();
    *dg(m,0xA79Du)=0u;
    store_le16(dg(m,0xA6C2u),0u);
    *dg(m,0xD680u)=saved_d680;
    if(b.call_19cf4)(void)b.call_19cf4();
    store_le16(dg(m,0x1457Cu),0u);
}
static_assert(0x0869Fu-0x080ACu==1523u);

// 086C4..0871A: compare two per-slot words against one 67CAh table word and
// notify 24A9C independently for each match.
void function_086c4(Machine& m,Application12Boundary& b,u32 slot,u32 index) noexcept {
    const u32 base=slot*0x248u;
    const u16 wanted=load_le16(dg(m,0x67CAu+index*2u));
    if(load_le16(dg(m,0x2193Cu+base))==wanted&&b.call_24a9c)b.call_24a9c(slot,0u);
    if(load_le16(dg(m,0x2193Eu+base))==wanted&&b.call_24a9c)b.call_24a9c(slot,1u);
}
static_assert(0x0871Bu-0x086C4u==87u);

// 0871C..0893B: expire one 21A12h status bit, format its message, clear its
// deadline, then perform the address-distinct bit-specific side effect.
void function_0871c(Machine& m,Application12Boundary& b,u32 slot,u32 bit) noexcept {
    const u32 base=slot*0x248u;
    const u32 mask=1u<<(bit&31u);
    store_le32(dg(m,0x21A12u+base),load_le32(dg(m,0x21A12u+base))^mask);
    if(b.call_3210f)b.call_3210f(0x759Cu,load_le32(dg(m,0x669Cu)),0x218E0u+base);
    const u32 end=0x759Cu+app11_strlen(m,0x759Cu);
    if(b.call_1c368)b.call_1c368(end,bit+0x24Cu);
    app09_append_cstr(m,0x759Cu,load_le32(dg(m,0x66A0u)));
    if(b.call_1c21c)b.call_1c21c(0x759Cu);
    *dg(m,0xCF5Au+slot)=1u;
    store_le32(dg(m,0x21A16u+base+bit*4u),0u);

    switch(bit) {
        case 1u:
        case 10u:
            if(b.call_31f38)b.call_31f38(load_le32(dg(m,0xF91Cu)));
            break;
        case 6u:
            store_le32(dg(m,0x21AD2u+base),load_le32(dg(m,0x21AD2u+base))-1u);
            break;
        case 11u:
            store_le32(dg(m,0x21ACAu+base),0u);
            break;
        case 12u:
            if(b.call_1202c)b.call_1202c(slot,0u-load_le32(dg(m,0x21A03u+base)));
            break;
        case 13u:
            *dg(m,0x21915u+base)=static_cast<u8>(*dg(m,0x21915u+base)-*dg(m,0x21A07u+base));
            break;
        case 17u:
            store_le32(dg(m,0xFD08u),load_le32(dg(m,0xFD08u))-1u);
            break;
        case 18u:
            store_le16(dg(m,0xF03Cu),static_cast<u16>(load_le16(dg(m,0xF03Cu))-1u));
            break;
        case 20u:
            for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
            store_le16(dg(m,0x6DAEu),static_cast<u16>(load_le16(dg(m,0x6DAEu))-1u));
            break;
        case 21u:
            for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
            for(u32 off=0u;off<0x920u;off+=0x248u)
                *dg(m,0x21A14u+off)=static_cast<u8>(*dg(m,0x21A14u+off)&0xDFu);
            break;
        case 23u:
            if(b.call_1202c)b.call_1202c(slot,0u-load_le32(dg(m,0x21A9Eu+base)));
            break;
        case 25u:
            store_le16(dg(m,0xF03Eu),static_cast<u16>(load_le16(dg(m,0xF03Eu))-1u));
            break;
        default:
            break;
    }
}
static_assert(0x0893Cu-0x0871Cu==544u);

// 0893C..08A7C: sweep four party slots for nine event deadlines, one refresh
// deadline, and 26 status-bit deadlines. All deadline comparisons are signed.
void function_0893c(Machine& m,Application12Boundary& b) noexcept {
    const u32 now=b.call_19cc4?b.call_19cc4():0u;
    for(u32 slot=0u,base=0u;slot<4u;++slot,base+=0x248u) {
        if(static_cast<u32>(load_le16(dg(m,0x218F0u+base)))>=0x4000u)continue;

        if(load_le32(dg(m,0x21AA2u+base))!=0u) {
            for(u32 event=0u;event<9u;++event) {
                const u32 d=0x21AA6u+base+event*4u;
                const u32 deadline=load_le32(dg(m,d));
                if(deadline!=0u&&static_cast<s32>(now)>=static_cast<s32>(deadline)) {
                    store_le32(dg(m,0x21AA2u+base),load_le32(dg(m,0x21AA2u+base))^(1u<<(event&31u)));
                    *dg(m,0xCF5Au+slot)=1u;
                    store_le32(dg(m,d),0u);
                    function_086c4(m,b,slot,event);
                }
            }
        }

        const u32 refresh=load_le32(dg(m,0x21ACEu+base));
        if(refresh!=0u&&static_cast<s32>(now)>=static_cast<s32>(refresh)) {
            if(b.call_1202c)b.call_1202c(slot,1u);
            store_le32(dg(m,0x21ACEu+base),0u);
            const u32 current=b.call_12098?b.call_12098(slot):0u;
            if(current!=static_cast<u32>(*dg(m,0x21912u+base))) {
                const u32 factor=b.call_1c328?b.call_1c328(2u,4u,0u,0u):0u;
                const u32 fresh=b.call_19cc4?b.call_19cc4():0u;
                store_le32(dg(m,0x21ACEu+base),fresh+factor*0x2A30u);
            }
        }

        if(load_le32(dg(m,0x21A12u+base))!=0u) {
            for(u32 bit=0u;bit<26u;++bit) {
                const u32 deadline=load_le32(dg(m,0x21A16u+base+bit*4u));
                if(static_cast<s32>(deadline)>0&&static_cast<s32>(now)>=static_cast<s32>(deadline))
                    function_0871c(m,b,slot,bit);
            }
        }
    }
}
static_assert(0x08A7Du-0x0893Cu==321u);

constexpr u32 kMovingRecordSoundOffsets[9]={
    0x1Bu,0x36u,0x2Bu,0x0Du,0x1Du,0x3Au,0x38u,0x38u,0x1Bu
};

static u32 app12_abs_bits(u32 v) noexcept {
    return (v&0x80000000u)!=0u ? 0u-v : v;
}

// 08A80..08C87: create one 16h-byte moving record. Packed coordinates are
// passed as dwords, but the tile calculation intentionally consumes their high
// words with 16-bit IMUL/truncation. The sound-table index has no encoded bounds.
void function_08a80(Machine& m,Application12Boundary& b,u32 type,s32 angle,s32 owner,
                     u32 packed_x,u32 packed_y) noexcept {
    if(b.call_e578)b.call_e578(static_cast<u32>(owner));
    if(static_cast<s32>(load_le32(dg(m,0xC038u)))>=0x96)return;

    const u32 rec=0x248ECu+load_le32(dg(m,0xC038u))*0x16u;
    store_le32(dg(m,rec),packed_x);
    store_le32(dg(m,rec+4u),packed_y);
    store_le32(dg(m,rec+8u),static_cast<u32>(angle));
    const u16 tile=static_cast<u16>(
        static_cast<u16>(static_cast<u32>(static_cast<u16>(packed_x>>16))*load_le16(dg(m,0x25619u)))+
        static_cast<u16>(packed_y>>16));
    store_le16(dg(m,rec+0x0Cu),tile);
    store_le16(dg(m,rec+0x0Eu),static_cast<u16>(type));
    store_le16(dg(m,rec+0x10u),0u);

    u16 flags=static_cast<u16>(load_le16(dg(m,rec+0x14u))&0xC43Fu);
    const u16 owner_bits=static_cast<u16>((static_cast<u8>(owner)&7u)<<11);
    flags=static_cast<u16>(flags|owner_bits);
    store_le16(dg(m,rec+0x14u),flags);

    if(type==8u) {
        const u32 r=b.call_30ca2?b.call_30ca2():0u;
        store_le16(dg(m,rec+0x12u),static_cast<u16>(((r*10u)>>15)+0x14u));
    }

    store_le32(dg(m,0xC038u),load_le32(dg(m,0xC038u))+1u);
    *dg(m,rec+0x14u)=static_cast<u8>(*dg(m,rec+0x14u)&0xF9u);

    bool play=owner>=0&&owner<4;
    if(!play) {
        if((*dg(m,0x198E0u+static_cast<u32>(tile)*2u)&0x40u)!=0u)play=true;
        else {
            const u32 dx=app12_abs_bits(load_le32(dg(m,rec))-load_le32(dg(m,0x255D4u)));
            if(static_cast<s32>(dx)<10) {
                const u32 dy=app12_abs_bits(load_le32(dg(m,rec+4u))-load_le32(dg(m,0x255D0u)));
                if(static_cast<s32>(dy)<10)play=true;
            }
        }
    }
    if(play&&b.call_28110)
        b.call_28110(load_le32(dg(m,0x258D4u))+kMovingRecordSoundOffsets[type],0xFFu,0u,1u);

    angle=static_cast<s32>(static_cast<u32>(angle)+0x16u);
    if(angle>=0x168)angle-=0x168;
    if(angle<0)angle+=0x168;
    const s32 quotient=angle/0x2D;
    const u32 orientation=static_cast<u32>(8-quotient)&7u;
    *dg(m,rec+0x14u)=static_cast<u8>(*dg(m,rec+0x14u)&0xC7u);
    store_le16(dg(m,rec+0x14u),static_cast<u16>(load_le16(dg(m,rec+0x14u))|
                                                static_cast<u16>(orientation<<3)));

    if(type!=8u) {
        for(;;) {
            const u32 hit=b.call_2c9dc?b.call_2c9dc(load_le32(dg(m,rec)),load_le32(dg(m,rec+4u))):0u;
            if(hit==0u)break;
            if(b.call_22e90)b.call_22e90(rec,0u);
            if((*dg(m,rec+0x14u)&6u)!=0u)break;
        }
    }

    if((*dg(m,rec+0x14u)&6u)!=0u) {
        if(b.call_19ccc)(void)b.call_19ccc(0u);
        const u8 saved=*dg(m,0xBF38u);
        *dg(m,0xBF38u)=0u;
        if(b.call_1f1c8)(void)b.call_1f1c8();
        if(b.call_1ea04)b.call_1ea04();
        if(b.call_1ea78)b.call_1ea78();
        *dg(m,0xBF38u)=saved;
        if(b.call_19cf4)(void)b.call_19cf4();
    }

    if(owner>=0&&owner<4) {
        const u32 base=static_cast<u32>(owner)*0x248u;
        store_le32(dg(m,0x21A2Au+base),0u);
        store_le32(dg(m,0x21A3Au+base),0u);
        store_le16(dg(m,0x21A12u+base),static_cast<u16>(load_le16(dg(m,0x21A12u+base))&0xFDDFu));
        *dg(m,0xCF5Au+static_cast<u32>(owner))=1u;
    }
}
static_assert(0x08C88u-0x08A80u==520u);

// 08C88..08CD7: compact the 16h-byte moving-record array. An out-of-range
// signed index reports 66A4h but deliberately continues into the copy anyway.
void function_08c88(Machine& m,Application12Boundary& b,s32 index) noexcept {
    if(index>=static_cast<s32>(load_le32(dg(m,0xC038u)))&&b.call_2f580)
        b.call_2f580(load_le32(dg(m,0x66A4u)));
    const u32 i=static_cast<u32>(index);
    const u32 count=(load_le32(dg(m,0xC038u))-1u-i)*0x16u;
    if(b.call_30cd4)b.call_30cd4(0x248ECu+i*0x16u,0x248ECu+(i+1u)*0x16u,count);
    store_le32(dg(m,0xC038u),load_le32(dg(m,0xC038u))-1u);
}
static_assert(0x08CD8u-0x08C88u==80u);
