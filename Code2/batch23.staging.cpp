// -----------------------------------------------------------------------------
// Batch 23: 10FAC..11C2B panel lifecycle/redraw, item-grid presentation,
// equipment/container compatibility, nested-placement UI, and item swapping.
// CS tables at 113F4 and 11C2C are data in RESIDUAL_LEDGER.md.
// -----------------------------------------------------------------------------

struct Application23Boundary {
    Application20Boundary* app20=nullptr;
    Application21Boundary* app21=nullptr;
    Application22Boundary* app22=nullptr;
    void (*call_2e374)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_292bc)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_22d70)()=nullptr;
    void (*call_1c124)(u32)=nullptr;
    void (*call_4210f2)(u32,u32)=nullptr;
    s32 (*call_50dc)(u32,u32)=nullptr;
    void (*call_03a78)(u32,s32)=nullptr;
    u32 (*read_11630_stale_offset)()=nullptr;
};

static u8 app23_shl8(u8 value,u32 count) noexcept {
    count&=31u;
    if(count>=8u)return 0u;
    return static_cast<u8>(static_cast<u32>(value)<<count);
}
static u32 app23_item_rec(Machine& m,s32 item) noexcept {
    return load_le32(dg(m,0x255D8u))+static_cast<u32>(item*65);
}
static bool app23_special_item_range(s32 item) noexcept {
    const u32 v=static_cast<u32>(item);
    return v>=0xA7u&&v<=0xC9u;
}
static void app23_mark_special(Machine& m,s32 item) noexcept {
    if(app23_special_item_range(item))store_le32(dg(m,0xBF44u),0x3Au);
}
static void app23_select_page(Machine& m,Application23Boundary& b) noexcept {
    if(b.app20&&b.app20->call_291b0)b.app20->call_291b0(*dg(m,0xC050u));
}
static void app23_draw_cursor_item(Machine& m,Application23Boundary& b,s32 item,u32 variant) noexcept {
    app23_mark_special(m,item);
    if(b.app21&&b.app21->call_2a5b0)
        b.app21->call_2a5b0(load_le32(dg(m,0xC050u)),static_cast<u32>(item),static_cast<u8>(variant));
    *dg(m,0xC050u)^=1u;
}

void function_10fac(Machine& m,Application23Boundary& b,u32 panel,u32 xmeta,u32 ymeta,u32 zmeta) noexcept {
    const u32 state=panel*4u;
    *dg(m,0x6FA0u+state)=static_cast<u8>(xmeta);
    *dg(m,0x6FA1u+state)=static_cast<u8>(ymeta);
    *dg(m,0x6FA3u+state)=static_cast<u8>(zmeta);
    const u32 first=xmeta*2u+0x14u;
    for(u32 i=first;i<first+4u;++i)
        if(b.app22&&b.app22->call_3f29c)(void)b.app22->call_3f29c(i,2u);
    const u32 dst=panel*0x50u;
    for(u32 mode=0u;mode<4u;++mode) {
        app23_select_page(m,b);
        *dg(m,0x6FA2u+state)=static_cast<u8>(mode);
        if(b.app22)function_10bbc(m,*b.app22,panel);
        const s32 item=static_cast<s16>(load_le16(dg(m,0x8600u)));
        const u32 variant=*dg(m,0x8602u);
        app23_mark_special(m,item);
        if(b.app21&&b.app21->call_2a724)
            b.app21->call_2a724(load_le32(dg(m,0xC050u)),static_cast<u32>(item),variant,
                                0xC4u,dst,0x64u,0x50u);
        *dg(m,0xC050u)^=1u;
    }
}
static_assert(0x110B4u-0x10FACu==264u);

void function_110b4(Machine& m,Application23Boundary& b,u32 panel) noexcept {
    const u32 dst=panel*0x50u;
    const u32 state=panel*4u;
    for(s32 mode=3;mode>=0;--mode) {
        app23_select_page(m,b);
        *dg(m,0x6FA2u+state)=static_cast<u8>(mode);
        if(b.app22)function_10bbc(m,*b.app22,panel);
        const s32 item=static_cast<s16>(load_le16(dg(m,0x8600u)));
        const u32 variant=*dg(m,0x8602u);
        app23_mark_special(m,item);
        if(b.app21&&b.app21->call_2a724)
            b.app21->call_2a724(load_le32(dg(m,0xC050u)),static_cast<u32>(item),variant,
                                0xC4u,dst,0x64u,0x50u);
        *dg(m,0xC050u)^=1u;
    }
    *dg(m,0x6FA0u+state)=0xFFu;
    *dg(m,0x6FA1u+state)=0xFFu;
    *dg(m,0x6FA3u+state)=0xFFu;
}
static_assert(0x1118Bu-0x110B4u==215u);

void function_1118c(Machine& m,Application23Boundary& b,u32 slot) noexcept {
    const u32 dst=load_le32(dg(m,0x7F6Cu));
    if(b.app21&&b.app21->call_3975c)b.app21->call_3975c(0u,0x45u,dst+0x38u,1u,2u);
    if(b.app21&&b.app21->call_3e510)b.app21->call_3e510(0x128u,dst,4u,2u);
    const u32 actor=slot*0x248u;
    for(u32 i=0u;i<12u;++i) {
        const s32 item=static_cast<s16>(load_le16(dg(m,0x21920u+actor+i*2u)));
        if(item==-1)continue;
        const u32 rec=app23_item_rec(m,item);
        const s32 sprite=static_cast<s16>(load_le16(dg(m,rec+0x2Du)));
        if(item==0xA6) {
            if(b.app21&&b.app21->call_3975c)
                b.app21->call_3975c(0u,0x127u,dst,static_cast<u32>(sprite),1u);
            continue;
        }
        const s32 q=static_cast<s32>(i)/4;
        const s32 r=static_cast<s32>(i)%4;
        const u32 x=static_cast<u32>(0x129+q*0x22);
        const u32 y=static_cast<u32>(static_cast<s32>(dst)+5+r*0x12);
        if(b.app20)function_0f308(m,*b.app20,x,y,static_cast<u32>(sprite),static_cast<u32>(item));
    }
    if(b.app22)function_10bbc(m,*b.app22,slot);
}
static_assert(0x11268u-0x1118Cu==220u);

void function_11268(Machine& m,Application23Boundary& b,u32 slot) noexcept {
    const u32 actor=slot*0x248u;
    const u32 dst=load_le32(dg(m,0x7F6Cu));
    if((load_le16(dg(m,0x218F0u+actor))&0x8000u)!=0u) {
        if(b.app21&&b.app21->call_3e510)b.app21->call_3e510(0u,dst,2u,2u);
    } else {
        if(b.app21)function_0ff3c(m,*b.app21,slot);
        if(load_le32(dg(m,0x6F90u+slot*4u))!=0u) {
            if(b.app22)function_10158(m,*b.app22,slot);
        } else function_1118c(m,b,slot);
    }
    if(slot==load_le32(dg(m,0x7F70u))&&b.call_2e374)
        b.call_2e374(0u,slot*0x50u,0x190u,0x50u,0x1Au);
}
static_assert(0x112EAu-0x11268u==130u);

void function_112ec(Machine& m,Application23Boundary& b) noexcept {
    app23_select_page(m,b);
    for(u32 slot=0u;slot<4u;++slot) {
        store_le32(dg(m,0x7F6Cu),slot*0x50u);
        function_11268(m,b,slot);
    }
    const u32 buffer=b.app20&&b.app20->call_3104c?b.app20->call_3104c(0x1400u):0u;
    store_le32(dg(m,0x7F68u),buffer);
    if(b.call_292bc)b.call_292bc(buffer,0x62u,0u,0x10u,0x140u);
    const s32 item=static_cast<s16>(load_le16(dg(m,0x8600u)));
    const u32 variant=*dg(m,0x8602u);
    if(b.app21)function_0f790(m,*b.app21,item,variant);
    app23_draw_cursor_item(m,b,item,variant);
    if(b.call_22d70)b.call_22d70();
    *dg(m,0xD680u)=1u;
}
static_assert(0x113A9u-0x112ECu==189u);

[[nodiscard]] u32 function_113ac(u32 outer,u32 inner) noexcept {
    if(outer==0u)return inner==3u||inner==5u?1u:0u;
    if(outer==1u)return inner==1u||inner==6u||inner==2u?0u:1u;
    if(outer==2u)return inner==2u?0u:1u;
    return 0u;
}
static_assert(0x113F2u-0x113ACu==70u);

[[nodiscard]] u32 function_11404(Machine& m,Application23Boundary& b,u32 slot,u32 index,s32 candidate,u32 variant) noexcept {
    const u32 actor=slot*0x248u;
    const s32 current=static_cast<s16>(load_le16(dg(m,0x21920u+actor+index*2u)));
    const u32 current_rec=app23_item_rec(m,current);
    const s32 group=static_cast<s8>(*dg(m,current_rec+0x31u));
    const s32 goff_s=group*10;
    const u32 goff=static_cast<u32>(goff_s);
    const u32 list=load_le32(dg(m,0x7335u+goff));
    const u32 mode=*dg(m,0x7334u+goff);

    u32 candidate_rec=0u;
    if(candidate!=-1) {
        candidate_rec=app23_item_rec(m,candidate);
        const u32 type=*dg(m,candidate_rec+0x2Fu);
        if(mode>=3u&&mode<=6u) {
            static constexpr u8 expected[4]={0x0Du,0x0Eu,0x0Cu,0x0Fu};
            if(type!=expected[mode-3u])return 0u;
        }
        if(mode<2u&&(*dg(m,candidate_rec+0x35u)&0x03u)!=0u)return 0u;
        if(type==1u&&candidate!=0x50) {
            const s32 nested_group=static_cast<s8>(*dg(m,candidate_rec+0x31u));
            const u32 nested_mode=*dg(m,0x7334u+static_cast<u32>(nested_group*10));
            if(function_113ac(mode,nested_mode)==0u)return 0u;
        }
    }
    if(mode==4u&&(candidate==0x114||candidate==0x115||candidate==0x128))return 0u;

    const u32 count=*dg(m,0x7330u+goff);
    u32 free_index=0u;
    for(;free_index<count;++free_index)
        if(static_cast<s16>(load_le16(dg(m,list+free_index*2u)))==-1)break;
    if(free_index==count)return 0u;

    s32 total=static_cast<s32>(function_0f5f8(m,static_cast<u32>(group)))-static_cast<s32>(*dg(m,current_rec+0x30u));
    if(candidate!=-1) {
        if(*dg(m,candidate_rec+0x2Fu)==1u)
            total+=static_cast<s32>(function_0f5f8(m,static_cast<u32>(static_cast<s8>(*dg(m,candidate_rec+0x31u)))));
        total+=static_cast<s32>(*dg(m,candidate_rec+0x30u));
    }
    if(total>static_cast<s32>(*dg(m,0x7332u+goff))*2)return 0u;

    app23_select_page(m,b);
    store_le16(dg(m,list+free_index*2u),static_cast<u16>(candidate));
    const u8 bit=static_cast<u8>(1u<<static_cast<u8>(free_index));
    u8 flags=*dg(m,0x7339u+goff);
    flags=static_cast<u8>(flags&static_cast<u8>(~bit));
    flags=static_cast<u8>(flags|app23_shl8(static_cast<u8>(variant),free_index));
    *dg(m,0x7339u+goff)=flags;
    return 1u;
}
static_assert(0x1162Eu-0x11404u==554u);

[[nodiscard]] u32 function_11630(Machine& m,Application23Boundary& b,u32 slot,s32 item) noexcept {
    s32 ranges[12]{};
    for(u32 i=0u;i<12u;++i)ranges[i]=static_cast<s32>(load_le32(dg(m,0xEEB0u+i*4u)));
    s32 group=0;
    s32 offset=0;
    bool found=false;
    for(;group<6;++group) {
        if(item<=ranges[group*2]) { offset=item-ranges[group*2+1]; found=true; break; }
    }
    if(!found)offset=b.read_11630_stale_offset?static_cast<s32>(b.read_11630_stale_offset()):0;
    const u32 actor=slot*0x248u;
    const u32 gbase=static_cast<u32>(group*10);
    u32 used=0u;
    for(u32 i=0u;i<10u;++i)
        if((*dg(m,0x2194Au+actor+gbase+i)&0x0Fu)!=0x0Fu)++used;
    const u32 limit=*dg(m,0x6153u+*dg(m,0x21919u+actor));
    const u32 target=0x2194Au+actor+gbase+static_cast<u32>(offset);
    u32 message=0u;
    if(used==limit)message=load_le32(dg(m,0x6D9Cu));
    else if((*dg(m,target)&0x0Fu)!=0x0Fu)message=load_le32(dg(m,0x6DA4u));
    else {
        if(b.app21&&b.app21->call_2c368)b.app21->call_2c368(0x759Cu,gbase+0x42u+static_cast<u32>(offset));
        u32 dst=0x759Cu; while(*dg(m,dst)!=0u)++dst;
        u32 src=load_le32(dg(m,0x6DA0u));
        for(;;){const u8 c=*dg(m,src++);*dg(m,dst++)=c;if(c==0u)break;}
        *dg(m,0x6DACu)=1u;
        const u32 token=b.app21&&b.app21->call_40700?b.app21->call_40700():0u;
        store_le32(dg(m,0x6DA8u),token+0x36u);
        if(b.app21)function_0f790(m,*b.app21,static_cast<s16>(load_le16(dg(m,0x8600u))),*dg(m,0x8602u));
        *dg(m,target)=0u;
        return 1u;
    }
    if(b.call_4210f2)b.call_4210f2(0x759Cu,message);
    *dg(m,0x6DACu)=1u;
    const u32 token=b.app21&&b.app21->call_40700?b.app21->call_40700():0u;
    store_le32(dg(m,0x6DA8u),token+0x36u);
    if(b.app21)function_0f790(m,*b.app21,static_cast<s16>(load_le16(dg(m,0x8600u))),*dg(m,0x8602u));
    return 0u;
}
static_assert(0x117A0u-0x11630u==368u);

void function_117a0(Machine& m,Application23Boundary& b,u32 slot,s32 index,u32 allow_nested) noexcept {
    const u32 actor=slot*0x248u;
    const u32 slot_addr=actor+static_cast<u32>(index*2);
    const s32 current=static_cast<s16>(load_le16(dg(m,0x21920u+slot_addr)));
    const u32 shift=static_cast<u32>(index)&31u;
    const u32 old_variant=(load_le32(dg(m,0x21A0Bu+actor))>>shift)&1u;
    s32 selected=static_cast<s16>(load_le16(dg(m,0x8600u)));
    const u32 selected_variant=*dg(m,0x8602u);
    if(static_cast<u32>(current)>=0x7Eu&&static_cast<u32>(current)<=0x86u)return;

    const u32 had_48=function_0f2d4(m,slot,0x48u);
    if(current==0xA6&&app23_special_item_range(selected)) {
        if(function_11630(m,b,slot,selected)!=0u) {
            if(b.app20&&b.app20->call_1a344)b.app20->call_1a344(-1,0u);
            if(b.app21)function_0f790(m,*b.app21,-1,0u);
        }
        return;
    }
    if(current==0x35&&static_cast<u32>(selected)>=0x61u&&static_cast<u32>(selected)<=0x64u) {
        if(b.call_1c124)b.call_1c124(static_cast<u32>(selected-0x61));
        if(b.app20&&b.app20->call_1a344)b.app20->call_1a344(-1,0u);
        if(b.app21)function_0f790(m,*b.app21,-1,0u);
        return;
    }

    if(!(selected==-1&&current==-1)) {
        app23_select_page(m,b);
        if(selected!=-1&&current!=-1&&*dg(m,app23_item_rec(m,current)+0x2Fu)==1u&&allow_nested!=0u) {
            if(function_11404(m,b,slot,static_cast<u32>(index),selected,selected_variant)!=0u) {
                if(b.app21)function_0fed0(m,*b.app21,slot);
                if(b.app21)function_0f790(m,*b.app21,-1,0u);
                app23_draw_cursor_item(m,b,-1,0u);
                return;
            }
        }

        store_le16(dg(m,0x21920u+slot_addr),static_cast<u16>(selected));
        function_0f238(m,slot,static_cast<u32>(index),selected_variant);
        if(index<12) {
            const s32 q=index/4;
            const s32 r=index%4;
            const u32 x=static_cast<u32>(0x129+q*0x22);
            const u32 y=static_cast<u32>(static_cast<s32>(load_le32(dg(m,0x7F6Cu)))+5+r*0x12);
            if(b.app21&&b.app21->call_39b40)b.app21->call_39b40(x,y,0x20u,0x10u,0u);
            if(selected!=-1&&b.app20) {
                const s32 sprite=static_cast<s16>(load_le16(dg(m,app23_item_rec(m,selected)+0x2Du)));
                const s32 stored=static_cast<s16>(load_le16(dg(m,0x21920u+slot_addr)));
                function_0f308(m,*b.app20,x,y,static_cast<u32>(sprite),static_cast<u32>(stored));
            }
        } else if(b.app22)function_10bbc(m,*b.app22,slot);

        if(b.app21)function_0fed0(m,*b.app21,slot);
        if(b.app21)function_0f790(m,*b.app21,current,old_variant);
        app23_draw_cursor_item(m,b,current,current==-1?0u:old_variant);
    }

    if(function_0f2d4(m,slot,0x48u)==0u&&had_48==1u&&b.call_50dc) {
        const s32 first=b.call_50dc(1u,slot);
        if(first!=0) {
            const s32 second=b.call_50dc(1u,slot);
            const s32 level=static_cast<s16>(load_le16(dg(m,0x21900u+actor+static_cast<u32>(second*2))));
            if(b.call_03a78)b.call_03a78(slot,level);
        }
    }
    const s32 hand0=static_cast<s16>(load_le16(dg(m,0x2193Cu+actor)));
    const s32 hand1=static_cast<s16>(load_le16(dg(m,0x2193Eu+actor)));
    if(hand0==0x67||hand1==0x67)*dg(m,0x21A14u+actor)=static_cast<u8>(*dg(m,0x21A14u+actor)|0x40u);
    else *dg(m,0x21A14u+actor)=static_cast<u8>(*dg(m,0x21A14u+actor)&0xBFu);
}
static_assert(0x11AD8u-0x117A0u==824u);

void function_11ad8(Machine& m,Application23Boundary& b,u32 slot,u32 flags) noexcept {
    if((flags&2u)!=0u) {
        u32 mode=load_le32(dg(m,0x6F90u+slot*4u));
        mode=(mode&0xFFFF0000u)|static_cast<u16>(static_cast<u16>(mode)^1u);
        app23_select_page(m,b);
        for(u32 i=0u;i<4u;++i) {
            store_le32(dg(m,0x6F90u+i*4u),mode);
            store_le32(dg(m,0x7F6Cu),i*0x50u);
            function_11268(m,b,i);
        }
    } else {
        *dg(m,0x6F90u+slot*4u)^=1u;
        app23_select_page(m,b);
        function_11268(m,b,slot);
    }
    const s32 item=static_cast<s16>(load_le16(dg(m,0x8600u)));
    const u32 variant=*dg(m,0x8602u);
    if(b.app21)function_0f790(m,*b.app21,item,variant);
    if((flags&2u)!=0u)app23_draw_cursor_item(m,b,item,variant);
    else {
        app23_mark_special(m,item);
        if(b.app21&&b.app21->call_2a724)
            b.app21->call_2a724(load_le32(dg(m,0xC050u)),static_cast<u32>(item),variant,
                                0u,load_le32(dg(m,0x7F6Cu)),0x190u,0x50u);
        *dg(m,0xC050u)^=1u;
    }
}
static_assert(0x11C20u-0x11AD8u==328u);

[[nodiscard]] u32 function_11c20(u32 flags) noexcept { return (flags&2u)==0u?1u:0u; }
static_assert(0x11C2Cu-0x11C20u==12u);
