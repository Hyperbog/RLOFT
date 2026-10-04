// Batch 47 staging source; integrated by integrate_batch47.py and removed by workflow.

struct Application47Boundary {
    Dispatch002A8Boundary* dispatch=nullptr;
    s32 (*call_1c328)(u32,u32,u32,u32)=nullptr;
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    void (*call_1a344)(s32,u32)=nullptr;
    u32 (*call_f390)(s32,u32)=nullptr;
    void (*call_23438)(s32,u32,u32)=nullptr;
    void (*call_f290)(s32,s32,u32)=nullptr;
    void (*call_1c21c)(u32)=nullptr;
    void (*call_13118)()=nullptr;
    void (*call_291b0)(u32)=nullptr;
    u32 (*call_f218)(u32,u32)=nullptr;
    void (*call_f238)(u32,u32,u32)=nullptr;
    void (*call_1ea90)(u32,u32,u32)=nullptr;
    void (*call_1a8c8)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_24478)(u32,u32)=nullptr;
    void (*call_26018)(u32,u32,u32)=nullptr;
};

static void app47_draw_and_dispatch(Machine& m,Application47Boundary& b,u32 record) noexcept {
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258BCu))+6u,0xFFu,1u,2u);
    if(b.dispatch)function_002a8(m,*b.dispatch,static_cast<s8>(*dg(m,record+2u)));
    *dg(m,record+1u)=static_cast<u8>(*dg(m,record+1u)&0xFBu);
}

u32 function_20e08(Machine& m,Application47Boundary& b,s32 x,s32 y,u32 kind,u32 record) noexcept {
    if((*dg(m,record+1u)&4u)==0u)return 0u;
    bool accepted=false;
    if(kind==0x15Bu) accepted=(x>=0x12&&x<=0x33&&y>=0x15&&y<=0x29);
    else if(kind==0x182u) accepted=(x>=0x26&&x<=0x42&&y>=0x17&&y<=0x29);
    else if(kind==0x18Bu) accepted=(x>=0x1C&&x<=0x35&&y>=0x19&&y<=0x29);
    else {
        for(u32 off=0;;off+=2u) {
            const s32 entry=static_cast<s16>(load_le16(dg(m,0xD828u+off)));
            if(entry==static_cast<s32>(kind)) {
                if(x>=0x1D&&x<=0x35&&y>=0x1D&&y<=0x28)accepted=true;
                break;
            }
            if(entry==-1)break;
        }
    }
    if(!accepted)return 0u;
    const s32 current=static_cast<s16>(load_le16(dg(m,0x8600u)));
    if(current==0x32) {
        if(function_20cf4(static_cast<u32>(*dg(m,record+6u)))==0u) {
            store_le32(dg(m,0xBF44u),0x3Fu);
            return 1u;
        }
        s32 value=-1,actor=-1;
        function_20d7c(m,actor,value);
        if(actor!=-1) {
            const s32 probe=b.call_1c328?b.call_1c328(1u,0x14u,0u,0u):0;
            value+=3;
            if(probe!=0x14&&probe>=value) {
                app47_draw_and_dispatch(m,b,record);
                return 1u;
            }
        }
        const s32 probe=b.call_1c328?b.call_1c328(1u,0x14u,0u,0u):0;
        if(probe<=2) {
            if(b.call_1a344)b.call_1a344(-1,0u);
            store_le32(dg(m,0xBF44u),0x41u);
        } else store_le32(dg(m,0xBF44u),0x40u);
        return 1u;
    }
    if(current!=-1) {
        const u32 ok=b.call_f390?b.call_f390(current,static_cast<u32>(*dg(m,record+6u))):0u;
        if(ok!=0u) app47_draw_and_dispatch(m,b,record);
        else store_le32(dg(m,0xBF44u),0x3Eu);
    }
    return 1u;
}
static_assert(0x210AFu-0x20E08u==679u);

u32 function_210b0(Machine& m,Application47Boundary& b,s32 first,s32 second,u32 code,u32 record) noexcept {
    if((*dg(m,record+1u)&4u)==0u)return 0u;
    u32 table_index=0u;
    for(;;++table_index) {
        const u32 off=table_index*6u;
        const u32 entry=static_cast<u32>(load_le16(dg(m,0xD734u+off)));
        if(code==entry||code==entry+1u) {
            const s32 min_first=static_cast<u32>(*dg(m,0xD737u+off));
            const s32 max_first=static_cast<u32>(*dg(m,0xD739u+off));
            const s32 min_second=static_cast<u32>(*dg(m,0xD736u+off));
            const s32 max_second=static_cast<u32>(*dg(m,0xD738u+off));
            if(first>=min_first&&first<=max_first&&second>=min_second&&second<=max_second)break;
            return 0u;
        }
        // MOVZX word followed by CMP -1 cannot recognize FFFFh; do not invent a terminator.
    }
    const s32 width=static_cast<s16>(load_le16(dg(m,0x25619u)));
    const u32 map_index=static_cast<u32>(width*static_cast<s32>(*dg(m,record+4u))+
                                          static_cast<s32>(*dg(m,record+5u)));
    const u16 tile=load_le16(dg(m,0x198E0u+map_index*2u));
    const u32 tile_index=static_cast<u32>(static_cast<u8>(tile))&0x3Fu;
    const s32 key=static_cast<s16>(load_le16(dg(m,0x25622u+tile_index*2u)));
    s32 graphic=3;
    for(u32 i=0;i<17u;++i) {
        if(static_cast<s16>(load_le16(dg(m,0xD858u+i*4u)))==key) {
            graphic=static_cast<s16>(load_le16(dg(m,0xD85Au+i*4u)));
            break;
        }
    }
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258BCu))+static_cast<u32>(graphic),0xFFu,1u,2u);
    u8 direction=static_cast<u8>(*dg(m,record+1u)&3u);
    direction=static_cast<u8>((direction^1u)&3u);
    *dg(m,record+1u)=static_cast<u8>((*dg(m,record+1u)&0xFCu)|direction);
    if(b.dispatch)function_002a8(m,*b.dispatch,static_cast<s8>(*dg(m,record+2u)));
    return 1u;
}
static_assert(0x211D8u-0x210B0u==296u);

static bool app47_env_qualifies(Machine& m,u32 env_base,s32 env_index) noexcept {
    const u32 off=static_cast<u32>(env_index)*65u;
    if(*dg(m,env_base+off+0x2Fu)!=1u)return false;
    const s32 subtype=static_cast<s8>(*dg(m,env_base+off+0x31u));
    return *dg(m,static_cast<u32>(0x7334+subtype*10))==3u;
}
static bool app47_insert_list(Machine& m,Application47Boundary& b,s32 subtype) noexcept {
    const u32 desc=static_cast<u32>(0x7330+subtype*10);
    const s32 count=static_cast<u32>(*dg(m,desc));
    const u32 list=load_le32(dg(m,desc+5u));
    for(s32 i=0;i<count;++i) {
        const u32 p=list+static_cast<u32>(i)*2u;
        const s32 old=static_cast<s16>(load_le16(dg(m,p)));
        if(old==-1) {
            store_le16(dg(m,p),load_le16(dg(m,0x8600u)));
            if(b.call_f290)b.call_f290(subtype,i,static_cast<u32>(*dg(m,0x8602u)));
            if(b.call_1a344)b.call_1a344(old,0u);
            return true;
        }
    }
    return false;
}

void function_211d8(Machine& m,Application47Boundary& b,s32 actor_index,u32 arg2,u32 arg3,u32 flags) noexcept {
    const s32 pair=actor_index/2;
    if(*dg(m,0xD681u+static_cast<u32>(pair))!=0u) {
        if(b.call_23438)b.call_23438(pair,arg2,arg3);
        return;
    }
    const u32 actor_off=static_cast<u32>(actor_index)*0x248u;
    if((flags&8u)!=0u) {
        const s32 current=static_cast<s16>(load_le16(dg(m,0x8600u)));
        if(current==-1)return;
        const u32 env_base=load_le32(dg(m,0x255D8u));
        const u32 current_off=static_cast<u32>(current)*65u;
        const u32 type=static_cast<u32>(*dg(m,env_base+current_off+0x2Fu));
        bool inserted=false;
        if(type==0x0Fu) {
            const s32 linked=static_cast<s16>(load_le16(dg(m,0x21946u+actor_off)));
            if(linked!=-1) {
                const u32 linked_off=static_cast<u32>(linked)*65u;
                const s32 subtype=static_cast<s8>(*dg(m,env_base+linked_off+0x31u));
                inserted=app47_insert_list(m,b,subtype);
            }
        } else if(type==0x0Du) {
            s32 linked=static_cast<s16>(load_le16(dg(m,0x2193Cu+actor_off)));
            s32 selected=-1;
            if(linked!=-1) {
                if(app47_env_qualifies(m,env_base,linked))selected=0;
                else linked=-1;
            }
            if(linked==-1) {
                linked=static_cast<s16>(load_le16(dg(m,0x2193Eu+actor_off)));
                if(linked!=-1) {
                    if(app47_env_qualifies(m,env_base,linked))selected=1;
                    else linked=-1;
                }
            }
            if(linked!=-1) {
                const s32 chosen=static_cast<s16>(load_le16(dg(m,0x2193Cu+actor_off+static_cast<u32>(selected)*2u)));
                const u32 chosen_off=static_cast<u32>(chosen)*65u;
                const s32 subtype=static_cast<s8>(*dg(m,env_base+chosen_off+0x31u));
                inserted=app47_insert_list(m,b,subtype);
            }
        }
        if(inserted)return;
        for(u32 i=0;i<12u;++i) {
            const u32 p=0x21920u+actor_off+i*2u;
            const s32 old=static_cast<s16>(load_le16(dg(m,p)));
            if(old==-1) {
                store_le16(dg(m,p),load_le16(dg(m,0x8600u)));
                if(b.call_1a344)b.call_1a344(old,0u);
                return;
            }
        }
        if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0xD678u)));
        return;
    }
    if((flags&1u)!=0u) {
        const u16 actor_flags=load_le16(dg(m,0x218F0u+actor_off));
        if((actor_flags&0x8000u)==0u&&b.call_13118)b.call_13118();
    }
}
static_assert(0x214B8u-0x211D8u==736u);

void function_214b8(Machine& m,Application47Boundary& b,s32 actor,u32 slot,u32 flags) noexcept {
    const u32 actor_off=static_cast<u32>(actor)*0x248u;
    const u16 actor_flags=load_le16(dg(m,0x218F0u+actor_off));
    if((actor_flags&0x8000u)!=0u)return;
    if((flags&8u)!=0u) {
        const s32 remainder=actor%2;
        const s32 quotient=actor/2;
        const u32 position=static_cast<u32>(remainder*0x35+4)+static_cast<u32>(quotient)*0xD3u;
        const s32 linked=static_cast<s16>(load_le16(dg(m,0x2193Cu+actor_off+slot*2u)));
        const u32 linked_bits=static_cast<u32>(linked);
        if(linked_bits>=0x7Eu&&linked_bits<=0x86u)return;
        const s32 current=static_cast<s16>(load_le16(dg(m,0x8600u)));
        const u32 current_byte=static_cast<u32>(*dg(m,0x8602u));
        const u32 env_base=load_le32(dg(m,0x255D8u));
        const u32 env_off=static_cast<u32>(current)*65u;
        const u32 type=static_cast<u32>(*dg(m,env_base+env_off+0x2Fu));
        if(type==3u||type==4u||type==8u||type==6u||type==7u||type==5u)return;
        if(b.call_291b0)b.call_291b0(static_cast<u32>(*dg(m,0xC050u)));
        const u32 target_slot=slot+0x0Eu;
        const u32 hit=(b.call_f218?b.call_f218(static_cast<u32>(actor),target_slot):0u)&0xFFu;
        if(b.call_1a344)b.call_1a344(linked,hit);
        store_le16(dg(m,0x2193Cu+actor_off+slot*2u),load_le16(dg(m,0x8600u)));
        if(b.call_f238)b.call_f238(static_cast<u32>(actor),target_slot,current_byte);
        store_le32(dg(m,0x21A7Eu+actor_off+slot*4u),0u);
        if(b.call_1ea90) {
            b.call_1ea90(static_cast<u32>(actor),slot,0xFFFFFFFFu);
            b.call_1ea90(static_cast<u32>(actor),slot^1u,0xFFFFFFFFu);
        }
        if(b.call_1a8c8) {
            b.call_1a8c8(load_le32(dg(m,0xC050u)),0x113u,position,0x7Du,0x35u);
            b.call_1a8c8(load_le32(dg(m,0xC050u)),0x113u,position+0x21u,0x7Du,0x35u);
        }
        if(slot!=1u)return;
        const u32 track=0x6FA0u+static_cast<u32>(actor)*4u;
        if(static_cast<s8>(*dg(m,track))!=-1) {
            *dg(m,track)=0xFFu; *dg(m,track+1u)=0xFFu;
            *dg(m,track+2u)=0xFFu; *dg(m,track+3u)=0xFFu;
        }
        if(type!=1u)return;
        const s8 subtype=static_cast<s8>(*dg(m,env_base+env_off+0x31u));
        *dg(m,track)=*dg(m,static_cast<u32>(0x7334+static_cast<s32>(subtype)*10));
        *dg(m,track+1u)=static_cast<u8>(subtype);
        *dg(m,track+2u)=3u;
        *dg(m,track+3u)=static_cast<u8>(current);
        return;
    }
    if((flags&1u)==0u)return;
    if((actor_flags&0xF240u)!=0u)return;
    const u32 ok=b.call_24478?b.call_24478(static_cast<u32>(actor),slot):0u;
    if(ok!=0u&&b.call_26018)b.call_26018(static_cast<u32>(actor),slot,0xFFFFFFFFu);
}
static_assert(0x21702u-0x214B8u==586u);
static_assert((0x210AFu-0x20E08u)+(0x211D8u-0x210B0u)+
              (0x214B8u-0x211D8u)+(0x21702u-0x214B8u)==2297u);
