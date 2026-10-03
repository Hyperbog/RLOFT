// -----------------------------------------------------------------------------
// Batch 24: 11C3C..12985 container-grid placement, inventory/equipment hit
// testing, party-slot swapping, and status-list modal presentation.
// The six-entry CS table at 12168 and compiler separators are non-code/data.
// -----------------------------------------------------------------------------

struct Application24Boundary {
    Application20Boundary* app20=nullptr;
    Application21Boundary* app21=nullptr;
    Application22Boundary* app22=nullptr;
    Application23Boundary* app23=nullptr;
    void (*call_1a5b0)(u32,u32,u32)=nullptr;
    u32 (*call_23eac)(u32,u32)=nullptr;
    u32 (*call_1a8c8)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_30cd4)(u32,u32,u32)=nullptr;
    void (*call_1a724)(u32,u32,u32,u32,u32,u32,u32)=nullptr;
};

static u32 app24_item_rec(Machine& m,s32 item) noexcept {
    return load_le32(dg(m,0x255D8u))+static_cast<u32>(item*65);
}
static s32 app24_div(s32 n,s32 d) noexcept { return n/d; }
static u32 app24_grid_bit(u8 bits,s32 index) noexcept {
    const u32 count=static_cast<u32>(index)&31u;
    if(count>=8u)return 0u;
    return (static_cast<u32>(bits)>>count)&1u;
}
static void app24_draw_cursor(Machine& m,Application24Boundary& b,s32 item,u32 variant) noexcept {
    app23_mark_special(m,item);
    if(b.call_1a5b0)b.call_1a5b0(load_le32(dg(m,0xC050u)),static_cast<u32>(item),variant);
    *dg(m,0xC050u)^=1u;
}

[[nodiscard]] u32 function_11c3c(Machine& m,Application24Boundary& b,u32 panel,s32 x,s32 y,u32 flags) noexcept {
    if((flags&2u)!=0u||load_le32(dg(m,0x6F90u+panel*4u))!=0u)return 0u;
    const s32 group=static_cast<s8>(*dg(m,0x6FA1u+panel*4u));
    const s32 g10=group*10;
    const u32 list=load_le32(dg(m,static_cast<u32>(static_cast<s32>(0x7335)+g10)));
    const s32 top=*dg(m,static_cast<u32>(static_cast<s32>(0x7333)+g10));
    const s32 width=*dg(m,static_cast<u32>(static_cast<s32>(0x7331)+g10));
    if(y<top||y>top+width*0x12)return 1u;

    const s32 selected=static_cast<s16>(load_le16(dg(m,0x8600u)));
    const u32 selected_variant=*dg(m,0x8602u);
    const u8 kind=*dg(m,static_cast<u32>(static_cast<s32>(0x7334)+g10));
    if(selected!=-1&&kind>=3u&&kind<=6u) {
        const u8 expected[4]={0x0Du,0x0Eu,0x0Cu,0x0Fu};
        if(*dg(m,app24_item_rec(m,selected)+0x2Fu)!=expected[kind-3u])return 1u;
    }
    if(kind<2u&&selected!=-1) {
        const u8 low=static_cast<u8>(load_le16(dg(m,app24_item_rec(m,selected)+0x35u)))&3u;
        if(low>=2u)return 1u;
    }
    if(selected!=-1) {
        const u32 rec=app24_item_rec(m,selected);
        if(*dg(m,rec+0x2Fu)==1u) {
            const s32 nested=static_cast<s8>(*dg(m,rec+0x31u));
            if(*dg(m,static_cast<u32>(static_cast<s32>(0x7334)+nested*10))==kind)return 1u;
        }
    }

    const s32 column=app24_div(x-0xDB,0x22);
    const s32 row=app24_div(y-top,0x12);
    if(row>=width)return 1u;
    const s32 index=column*width+row;
    const s32 count=*dg(m,static_cast<u32>(static_cast<s32>(0x7330)+g10));
    if(index>=count)return 1u;
    const u32 entry=list+static_cast<u32>(index*2);
    const s32 old_item=static_cast<s16>(load_le16(dg(m,entry)));
    const u32 old_variant=app24_grid_bit(*dg(m,static_cast<u32>(static_cast<s32>(0x7339)+g10)),index);
    if(selected==-1&&old_item==-1)return 1u;

    u32 burden=function_0f5f8(m,static_cast<u32>(group));
    const s32 anchor=static_cast<s8>(*dg(m,0x6FA3u+panel*4u));
    burden-=*dg(m,app24_item_rec(m,anchor)+0x30u);
    if(old_item!=-1) {
        const u32 rec=app24_item_rec(m,old_item);
        if(*dg(m,rec+0x2Fu)==1u)burden-=function_0f5f8(m,static_cast<u32>(static_cast<s8>(*dg(m,rec+0x31u))));
        burden-=*dg(m,rec+0x30u);
    }
    if(selected!=-1) {
        const u32 rec=app24_item_rec(m,selected);
        if(*dg(m,rec+0x2Fu)==1u)burden+=function_0f5f8(m,static_cast<u32>(static_cast<s8>(*dg(m,rec+0x31u))));
        burden+=*dg(m,rec+0x30u);
    }
    const u32 capacity=static_cast<u32>(*dg(m,static_cast<u32>(static_cast<s32>(0x7332)+g10)))*2u;
    if(static_cast<s32>(burden)>static_cast<s32>(capacity))return 1u;

    if(b.app23)app23_select_page(m,*b.app23);
    store_le16(dg(m,entry),static_cast<u16>(selected));
    const u32 shift=static_cast<u32>(index)&31u;
    if(shift<8u) {
        u8& bits=*dg(m,static_cast<u32>(static_cast<s32>(0x7339)+g10));
        bits=static_cast<u8>(bits&~static_cast<u8>(1u<<shift));
        bits=static_cast<u8>(bits|static_cast<u8>((selected_variant&1u)<<shift));
    }
    if(b.app22)function_10bbc(m,*b.app22,panel);
    if(b.app21)function_0f790(m,*b.app21,old_item,old_item==-1?0u:old_variant);
    app24_draw_cursor(m,b,old_item,old_item==-1?0u:old_variant);
    return 1u;
}
static_assert(0x11FAEu-0x11C3Cu==882u);

[[nodiscard]] u32 function_11fb0(Machine& m,Application24Boundary& b,u32 panel,s32 x,s32 y,u32 flags) noexcept {
    if((flags&2u)!=0u||load_le32(dg(m,0x6F90u+panel*4u))!=0u)return 0u;
    const s32 a=app24_div(x-0x129,0x22);
    const s32 c=app24_div(y-5,0x12);
    const s32 index=(a%0x20)*4+(c%0x10);
    if(b.app23)function_117a0(m,*b.app23,panel,index,1u);
    return 1u;
}
static_assert(0x1202Au-0x11FB0u==122u);

[[nodiscard]] s32 function_1202c(Machine& m,u32 slot,s32 amount) noexcept {
    const u32 actor=slot*0x248u;
    if(static_cast<s16>(load_le16(dg(m,0x21948u+actor)))==0x54) {
        const s32 old=static_cast<s32>(load_le32(dg(m,0x21A96u+actor)));
        if(old+amount>0x14)amount=0x14-old;
        store_le32(dg(m,0x21A96u+actor),load_le32(dg(m,0x21A96u+actor))+static_cast<u32>(amount));
    } else {
        const s32 old=*dg(m,0x21913u+actor);
        if(old+amount>0x14)amount=0x14-old;
        *dg(m,0x21913u+actor)=static_cast<u8>(*dg(m,0x21913u+actor)+static_cast<u8>(amount));
    }
    return amount;
}
static_assert(0x12097u-0x1202Cu==107u);

[[nodiscard]] u32 function_12098(Machine& m,u32 slot) noexcept {
    const u32 actor=slot*0x248u;
    if(static_cast<s16>(load_le16(dg(m,0x21948u+actor)))==0x54)return load_le32(dg(m,0x21A96u+actor));
    return *dg(m,0x21913u+actor);
}
static_assert(0x120BBu-0x12098u==35u);

void function_120bc(Machine& m,u32 slot,s32 old_item,s32 new_item) noexcept {
    const u32 actor=slot*0x248u;
    if(old_item==0x54)*dg(m,0x21913u+actor)=*dg(m,0x21A96u+actor);
    if(new_item==0x54) {
        const u32 old=*dg(m,0x21913u+actor);
        *dg(m,0x21913u+actor)=0x14u;
        store_le32(dg(m,0x21A96u+actor),old);
    }
    if(old_item==0x53)--*dg(m,0x21915u+actor);
    if(new_item==0x53)++*dg(m,0x21915u+actor);
}
static_assert(0x12125u-0x120BCu==105u);

[[nodiscard]] u32 function_12128(u32 item) noexcept {
    return item==0x10u||item==0x5Fu||item==0x0Cu||item==0xECu||item==0xEEu?1u:0u;
}
static_assert(0x12165u-0x12128u==61u);

[[nodiscard]] u32 function_12180(Machine& m,Application24Boundary& b,u32 slot,s32 x,s32 y,u32 flags) noexcept {
    if((flags&2u)!=0u||load_le32(dg(m,0x6F90u+slot*4u))!=0u)return 0u;
    const u32 actor=slot*0x248u;
    const s32 cursor=static_cast<s16>(load_le16(dg(m,0x8600u)));
    s32 target=-1;
    if(cursor!=-1) {
        const u8 type=*dg(m,app24_item_rec(m,cursor)+0x2Fu);
        switch(type) {
            case 3u: target=0x11; break;
            case 4u: {
                target=0x10;
                if(static_cast<s8>(*dg(m,0x218F5u+actor))==4&&static_cast<s8>(*dg(m,0x218F6u+actor))==-1)target=-1;
                const u32 gate=b.call_23eac?b.call_23eac(slot,5u):0u;
                if(gate!=0u&&function_12128(static_cast<u32>(cursor))==0u)target=-1;
                break;
            }
            case 5u: target=y<=0x28?0x0C:0x0D; break;
            case 6u: target=0x12; break;
            case 7u: target=0x14; break;
            case 8u: target=0x10; break;
            default:
                if(y<=0x28)target=function_0f168(static_cast<u32>(cursor))!=0u?0x13:0x0E;
                else target=0x0F;
                break;
        }
    } else {
        const u32 table=load_le32(dg(m,0x12C00u));
        for(u32 off=0u;off<0xF0u;off+=0x18u) {
            const u32 e=table+off;
            if(x<static_cast<s32>(load_le32(dg(m,e+8u)))||x>static_cast<s32>(load_le32(dg(m,e+0x10u))))continue;
            if(y<static_cast<s32>(load_le32(dg(m,e+0x0Cu)))||y>static_cast<s32>(load_le32(dg(m,e+0x14u))))continue;
            if((load_le32(dg(m,e+4u))&flags)==0u)continue;
            const s32 index=static_cast<s32>(load_le32(dg(m,e)));
            if(static_cast<s16>(load_le16(dg(m,0x21920u+actor+static_cast<u32>(index*2))))==-1)continue;
            target=index;
            break;
        }
    }
    if(target==-1)return 0u;
    const s32 existing=static_cast<s16>(load_le16(dg(m,0x21920u+actor+static_cast<u32>(target*2))));
    if(static_cast<u32>(existing)>=0x7Eu&&static_cast<u32>(existing)<=0x86u)return 0u;

    if(target==0x0E||target==0x0F) {
        if(target==0x0F&&existing!=-1&&*dg(m,app24_item_rec(m,existing)+0x2Fu)==1u&&b.app23)
            function_110b4(m,*b.app23,slot);
        if(b.app23)function_117a0(m,*b.app23,slot,target,0u);
        if(target==0x0F&&cursor!=-1) {
            const u32 rec=app24_item_rec(m,cursor);
            if(*dg(m,rec+0x2Fu)==1u&&b.app23) {
                const s32 nested=static_cast<s8>(*dg(m,rec+0x31u));
                const u32 kind=*dg(m,static_cast<u32>(static_cast<s32>(0x7334)+nested*10));
                function_10fac(m,*b.app23,slot,kind,static_cast<u32>(nested),static_cast<u32>(cursor));
            }
        }
    } else {
        if(target==0x14)function_120bc(m,slot,existing,cursor);
        if(b.app23)function_117a0(m,*b.app23,slot,target,1u);
    }
    return 1u;
}
static_assert(0x1246Fu-0x12180u==751u);

static u16 app24_replace_actor_bits(u16 word,u32 from,u32 to) noexcept {
    const u32 encoded=(word>>11u)&7u;
    if(encoded==static_cast<u16>(from))return static_cast<u16>((word&0xC7FFu)|((to&7u)<<11u));
    if(encoded==static_cast<u16>(to))return static_cast<u16>((word&0xC7FFu)|((from&7u)<<11u));
    return word;
}

void function_12470(Machine& m,Application24Boundary& b,u32 source) noexcept {
    if(b.app23)app23_select_page(m,*b.app23);
    store_le32(dg(m,0x7F70u),source);
    if(b.app23)function_11268(m,*b.app23,source);
    const u32 source_dst=source*0x50u;
    if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0u,source_dst,0x190u,0x50u);
    while(b.app20&&b.app20->call_1a1f4&&b.app20->call_1a1f4()!=0u){}
    while(b.app20&&b.app20->call_1a1f4&&b.app20->call_1a1f4()==0u){}
    const s32 target=app24_div(static_cast<s16>(load_le16(dg(m,0x9E33u))),0x50);
    store_le32(dg(m,0x7F70u),0xFFFFFFFFu);
    if(target==static_cast<s32>(source)) {
        if(b.app23)function_11268(m,*b.app23,source);
        if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0u,source_dst,0x190u,0x50u);
        if(b.app23&&b.app23->call_22d70)b.app23->call_22d70();
        return;
    }

    const u32 target_u=static_cast<u32>(target);
    const u32 temp=b.app20&&b.app20->call_3104c?b.app20->call_3104c(0x248u):0u;
    const u32 source_actor=0x218E0u+source*0x248u;
    const u32 target_actor=0x218E0u+target_u*0x248u;
    if(b.call_30cd4) {
        b.call_30cd4(temp,source_actor,0x248u);
        b.call_30cd4(source_actor,target_actor,0x248u);
        b.call_30cd4(target_actor,temp,0x248u);
    }
    if(b.app20&&b.app20->call_310b0)b.app20->call_310b0(temp);

    const u32 so=source*4u, to=target_u*4u;
    const u32 sv=load_le32(dg(m,0x6F90u+so)), tv=load_le32(dg(m,0x6F90u+to));
    store_le32(dg(m,0x6F90u+so),tv); store_le32(dg(m,0x6F90u+to),sv);
    for(u32 i=0u;i<4u;++i) {
        const u8 a=*dg(m,0x6FA0u+so+i), c=*dg(m,0x6FA0u+to+i);
        *dg(m,0x6FA0u+so+i)=c; *dg(m,0x6FA0u+to+i)=a;
    }

    store_le32(dg(m,0x7F6Cu),source*0x50u);
    if(b.app23)function_11268(m,*b.app23,source);
    if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0u,source*0x50u,0x190u,0x50u);
    store_le32(dg(m,0x7F6Cu),target_u*0x50u);
    if(b.app23)function_11268(m,*b.app23,target_u);
    if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0u,target_u*0x50u,0x190u,0x50u);

    const s32 count1=static_cast<s32>(load_le32(dg(m,0xC038u)));
    if(count1>0)for(s32 i=0;i<count1;++i) {
        const u32 p=0x24900u+static_cast<u32>(i)*0x16u;
        store_le16(dg(m,p),app24_replace_actor_bits(load_le16(dg(m,p)),source,target_u));
    }
    const s32 count2=static_cast<s16>(load_le16(dg(m,0x258A4u)));
    for(s32 i=0;i<count2;++i) {
        const u32 p=0x1762Cu+static_cast<u32>(i)*0x16u;
        store_le16(dg(m,p),app24_replace_actor_bits(load_le16(dg(m,p)),source,target_u));
    }
    if(b.app23&&b.app23->call_22d70)b.app23->call_22d70();
}
static_assert(0x12723u-0x12470u==691u);

void function_12724(Machine& m,Application24Boundary& b,u32 slot) noexcept {
    const u32 actor=slot*0x248u;
    *dg(m,0x6DACu)=0u;
    if(load_le32(dg(m,0x21A12u+actor))==0u)return;

    if(b.app23)app23_select_page(m,*b.app23);
    if(b.app21)function_0ff3c(m,*b.app21,slot);
    if(b.app21&&b.app21->call_3e510)b.app21->call_3e510(0x73u,load_le32(dg(m,0x7F6Cu)),0x25u,2u);
    if(load_le32(dg(m,0x6F90u+slot*4u))==0u&&b.app21&&b.app21->call_3975c)
        b.app21->call_3975c(0u,0x45u,load_le32(dg(m,0x7F6Cu))+0x38u,1u,2u);

    u32 bit=1u;
    u32 resource=0x24Cu;
    u32 sprite_index=2u;
    u32 y_sprite=0x88u;
    u32 y_text=0x8Cu;
    u32 shown=0u;
    while(shown<14u&&bit<=0x02000000u) {
        if((load_le32(dg(m,0x21A12u+actor))&bit)!=0u) {
            if(b.app20&&b.app20->call_1c368)b.app20->call_1c368(0x759Cu,resource);
            if(b.app20&&b.app20->call_2975c)b.app20->call_2975c(sprite_index,y_sprite,load_le32(dg(m,0x7F6Cu))+7u,
                                                                 load_le32(dg(m,0x258DCu))+0x4Du,0u);
            if(b.app20&&b.app20->call_3060c)b.app20->call_3060c(y_text,load_le32(dg(m,0x7F6Cu))+0x10u,0x759Cu,1u);
            ++shown;
        }
        ++resource; ++sprite_index; y_sprite+=0x12u; y_text+=0x12u; bit+=bit;
    }

    const s32 item=static_cast<s16>(load_le16(dg(m,0x8600u)));
    const u32 variant=*dg(m,0x8602u);
    if(b.app21)function_0f790(m,*b.app21,item,variant);
    app23_mark_special(m,item);
    if(b.call_1a724)b.call_1a724(load_le32(dg(m,0xC050u)),static_cast<u32>(item),variant,
                                  0x73u,slot*0x50u,0x11Du,0x50u);
    *dg(m,0xC050u)^=1u;
    while(b.app20&&b.app20->call_1a1f4&&b.app20->call_1a1f4()!=0u){}
    while(b.app20&&b.app20->call_1a1f4&&b.app20->call_1a1f4()==0u){}
    if(b.app23)app23_select_page(m,*b.app23);
    if(b.app23)function_11268(m,*b.app23,slot);

    const s32 item2=static_cast<s16>(load_le16(dg(m,0x8600u)));
    const u32 variant2=*dg(m,0x8602u);
    if(b.app21)function_0f790(m,*b.app21,item2,variant2);
    app23_mark_special(m,item2);
    if(b.call_1a724)b.call_1a724(load_le32(dg(m,0xC050u)),static_cast<u32>(item2),variant2,
                                  0u,load_le32(dg(m,0x7F6Cu)),0x190u,0x50u);
    *dg(m,0xC050u)^=1u;
    while(b.app20&&b.app20->call_1a1f4&&b.app20->call_1a1f4()!=0u){}
    if(b.app23&&b.app23->call_22d70)b.app23->call_22d70();
}
static_assert(0x12986u-0x12724u==610u);
