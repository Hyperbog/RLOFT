// -----------------------------------------------------------------------------
// Batch 65: 2C294..2D001 render ordering/orchestration, overlap predicates,
// secondary interaction routing, party transition, and timed presentation loop.
// All addresses are CODE.2 object-relative.
// -----------------------------------------------------------------------------

struct Application65Boundary {
    Application64Boundary* app64=nullptr;
    void (*call_30d30)(u32,u32,u32)=nullptr;
    void (*call_335dc)()=nullptr;
    void (*call_29738_stack4)(u32*)=nullptr;
    void (*call_296c0)(u32,u32,u32,u32)=nullptr;
    void (*call_36cec)(u32,u32,u32,u32,u32,u32)=nullptr;
    void (*call_2978c)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_2acdc)(u32)=nullptr;
    void (*call_181b4)(u32)=nullptr;
    u32 (*call_14a60)(u32)=nullptr;
    void (*call_14c40)(u32)=nullptr;
    void (*call_1c160)(u32)=nullptr;
    void (*call_1c124)(u32)=nullptr;
    u32 (*call_1c1dc)(u32)=nullptr;
    void (*call_0ccf4)()=nullptr;
    void (*call_1c0c0)(u32)=nullptr;
    void (*call_2e608)()=nullptr;
    void (*call_291b0)(u32)=nullptr;
    void (*call_1f1c8)()=nullptr;
    void (*call_2e510)(u32,u32,u32,u32)=nullptr;
    void (*call_2efd0)(u32,u32)=nullptr;
    void (*call_1ea04)()=nullptr;
    void (*call_1ea78)()=nullptr;
    void (*call_2e614)(u32,u32)=nullptr;
    u32 (*call_30b40)()=nullptr;
};

static u32 app65_record_base(Machine& m) noexcept {
    return 0x27148u+load_le32(dg(m,0xC050u))*0x2260u;
}
static u32 app65_count_slot(Machine& m) noexcept {
    return 0xF038u+load_le32(dg(m,0xC050u))*2u;
}
static bool app65_clip_active(Machine& m) noexcept {
    return load_le16(dg(m,0xF03Cu))!=0u || load_le16(dg(m,0xF03Eu))!=0u;
}
static u32 app65_inline_layout(u32 raw) noexcept {
    const s32 half=static_cast<s32>(raw)/2;
    const s32 quarter=half/2;
    s32 result=static_cast<s32>(0x84)-quarter;
    result+=half/8;
    result+=half/2;
    return static_cast<u32>(result);
}
static void app65_emit(Application65Boundary& b,u32 a0,u32 a1,u32 a2,u32 a3,u32 a4,u32 a5) noexcept {
    if(b.call_36cec)b.call_36cec(a0,a1,a2,a3,a4,a5);
}

void function_2c294(Machine& m,Application65Boundary& b) noexcept {
    u32 saved_rect[4]={0u,0u,0u,0u};
    if(b.call_30d30)b.call_30d30(app65_record_base(m),0u,0x1130u);

    store_le16(dg(m,app65_count_slot(m)),0u);
    u32 cursor=load_le16(dg(m,app65_count_slot(m)));
    if(b.app64)cursor=function_2b6d0(m,*b.app64,cursor);
    store_le16(dg(m,app65_count_slot(m)),static_cast<u16>(cursor));

    cursor=load_le16(dg(m,app65_count_slot(m)));
    if(b.app64)cursor=function_2bba0(m,*b.app64,cursor);
    store_le16(dg(m,app65_count_slot(m)),static_cast<u16>(cursor));

    cursor=load_le16(dg(m,app65_count_slot(m)));
    if(b.app64)cursor=function_2bdf4(m,*b.app64,cursor);
    store_le16(dg(m,app65_count_slot(m)),static_cast<u16>(cursor));

    cursor=load_le16(dg(m,app65_count_slot(m)));
    if(b.app64)cursor=function_2bfec(m,*b.app64,cursor);
    store_le16(dg(m,app65_count_slot(m)),static_cast<u16>(cursor));

    if(b.call_335dc)b.call_335dc();
    if(load_le16(dg(m,app65_count_slot(m)))==0u)return;

    if(app65_clip_active(m)){
        if(b.call_29738_stack4)b.call_29738_stack4(saved_rect);
        if(b.call_296c0)b.call_296c0(4u,0x20u,0x103u,0x11Fu);
    }

    for(u32 ordinal=0;;++ordinal){
        if(load_le16(dg(m,app65_count_slot(m)))<=ordinal)break;

        u32 min_y=0x7D00u;
        u32 best=0u;
        u32 scan=0u;
        u32 p=app65_record_base(m);
        while(load_le16(dg(m,app65_count_slot(m)))>scan){
            const u32 y=load_le16(dg(m,p+2u));
            if(y<min_y){min_y=y;best=scan;}
            ++scan;
            p=app65_record_base(m)+scan*0x16u;
        }

        const u32 target=app65_record_base(m)+best*0x16u;
        bool skip_generic=false;
        if(load_le16(dg(m,target+8u))==0u){
            const u32 source_index=load_le16(dg(m,target+6u));
            const u32 source=0x14D80u+source_index*0x16u;
            const u32 type=load_le16(dg(m,source+0x0Eu));
            if(type==8u){
                const u32 y=load_le16(dg(m,target+2u));
                const u32 scaled=(y*0x78u+0x6Cu)/0xD8u;
                app65_emit(b,load_le16(dg(m,target)),load_le16(dg(m,target+4u))+0x12u,scaled,
                           load_le16(dg(m,target+0x0Cu)),0x58u,y);
                app65_emit(b,load_le16(dg(m,target)),load_le16(dg(m,target+4u)),scaled,
                           load_le16(dg(m,target+0x0Cu)),0u,y);
                skip_generic=true;
            }
        }
        if(!skip_generic && load_le16(dg(m,target+8u))==1u){
            const u32 source_index=load_le16(dg(m,target+6u));
            const u32 type=load_le16(dg(m,0x22246u+source_index*0x16u));
            if(static_cast<s32>(type)>=0x3C && static_cast<s32>(type)<=0x43){
                const u32 y=load_le16(dg(m,target+2u));
                app65_emit(b,load_le16(dg(m,target)),load_le16(dg(m,target+4u))+8u,y,
                           load_le16(dg(m,target+0x0Cu)),0x58u,y);
            }
        }
        if(!skip_generic){
            if(load_le16(dg(m,target+8u))==2u){
                const u32 source_index=load_le16(dg(m,target+6u));
                const u32 y=load_le16(dg(m,target+2u));
                const u32 color=(*dg(m,0x1762Du+source_index*0x16u)&0x02u)!=0u?0x3Cu:0u;
                app65_emit(b,load_le16(dg(m,target)),load_le16(dg(m,target+4u)),y,
                           load_le16(dg(m,target+0x0Cu)),color,y);
            } else {
                const u32 y=load_le16(dg(m,target+2u));
                app65_emit(b,load_le16(dg(m,target)),load_le16(dg(m,target+4u)),y,
                           load_le16(dg(m,target+0x0Cu)),0u,y);
            }
        }

        if(load_le16(dg(m,target+8u))==0u){
            const u32 source_index=load_le16(dg(m,target+6u));
            if((*dg(m,0x14D88u+source_index*0x16u)&0x08u)!=0u){
                const u32 y=load_le16(dg(m,target+2u));
                const s32 row=static_cast<s16>(load_le16(dg(m,0x27106u)))+0x41;
                app65_emit(b,load_le16(dg(m,target)),static_cast<u32>(row),y,
                           load_le16(dg(m,target+0x0Cu)),0u,y);
            }
        }

        if(app65_clip_active(m) && load_le16(dg(m,target+8u))==0u){
            const u32 source_index=load_le16(dg(m,target+6u));
            const u32 type=load_le16(dg(m,0x14D8Eu+source_index*0x16u));
            if(static_cast<s32>(type)<0x22){
                bool allow=true;
                if(load_le16(dg(m,0xF03Eu))==0u){
                    const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
                    allow=(*dg(m,desc+0x31u)&0x01u)==0u;
                }
                if(allow && b.call_2978c){
                    const u32 y=load_le16(dg(m,target+2u));
                    b.call_2978c(0u,app65_inline_layout(y),load_le16(dg(m,target))+0x18u,
                                  load_le32(dg(m,0x258DCu))+0x48u,0u);
                }
            }
        }

        store_le16(dg(m,target+0x0Au),static_cast<u16>(ordinal));
        *dg(m,target+3u)=static_cast<u8>(*dg(m,target+3u)+0x7Du);
    }

    if(app65_clip_active(m) && b.call_296c0)
        b.call_296c0(saved_rect[0],saved_rect[2],saved_rect[1],saved_rect[3]);
}
static_assert(0x2C7F7u-0x2C294u==1379u);

[[nodiscard]] u32 function_2c800(Machine& m,u32 a0,u32 a1,u32 exclude) noexcept {
    for(s32 index=0;;++index){
        const s32 count=static_cast<s16>(load_le16(dg(m,0x258A2u)));
        if(index>=count)return 0u;
        if(static_cast<u32>(index)==exclude)continue;
        const u32 off=static_cast<u32>(index)*0x16u;
        if((*dg(m,0x14D8Bu+off)&0x02u)!=0u)continue;
        const u32 x=load_le32(dg(m,0x14D84u+off));
        const u32 y=load_le32(dg(m,0x14D80u+off));
        const u32 type=load_le16(dg(m,0x14D8Eu+off));
        const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
        const u32 radius=load_le16(dg(m,desc+0x16u))+0x3E8u;
        const u32 x0=x-radius,x1=x+radius,y0=y-radius,y1=y+radius;
        if(a1>=x0 && a1<=x1 && a0>=y0 && a0<=y1)return 1u;
    }
}
static_assert(0x2C8E2u-0x2C800u==226u);

[[nodiscard]] u32 function_2c8e4(Machine& m,Application65Boundary& b,u32 a0,u32 a1,u32 exclude) noexcept {
    for(s32 index=0;;++index){
        const s32 count=static_cast<s16>(load_le16(dg(m,0x258A2u)));
        if(index>=count)return 0u;
        if(static_cast<u32>(index)==exclude)continue;
        const u32 off=static_cast<u32>(index)*0x16u;
        const u32 type0=load_le16(dg(m,0x14D8Eu+off));
        if((b.call_2acdc?b.call_2acdc(type0):0u)==0u)continue;
        if((*dg(m,0x14D8Bu+off)&0x02u)!=0u)continue;
        const u32 x=load_le32(dg(m,0x14D84u+off));
        const u32 y=load_le32(dg(m,0x14D80u+off));
        const u32 type=load_le16(dg(m,0x14D8Eu+off));
        const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
        const u32 radius=load_le16(dg(m,desc+0x16u));
        const u32 x0=x-radius,x1=x+radius,y0=y-radius,y1=y+radius;
        if(a1>=x0 && a1<=x1 && a0>=y0 && a0<=y1)return 1u;
    }
}
static_assert(0x2C9D9u-0x2C8E4u==245u);

[[nodiscard]] u32 function_2c9dc(Machine& m,u32 a0,u32 a1) noexcept {
    const u32 x=load_le32(dg(m,0x255D0u));
    const u32 y=load_le32(dg(m,0x255D4u));
    const u32 x0=x-0x8FA0u,x1=x+0x8FA0u,y0=y-0x8FA0u,y1=y+0x8FA0u;
    return (a1>=x0 && a1<=x1 && a0>=y0 && a0<=y1)?1u:0u;
}
static_assert(0x2CA5Cu-0x2C9DCu==128u);

[[nodiscard]] u32 function_2ca5c(Machine& m,u32 a0,u32 a1) noexcept {
    for(s32 index=0;;++index){
        const s32 count=static_cast<s16>(load_le16(dg(m,0x258A6u)));
        if(index>=count)return 0u;
        const u32 off=static_cast<u32>(index)*0x16u;
        if(load_le16(dg(m,0x22246u+off))!=0xADu)continue;
        if(static_cast<s16>(load_le16(dg(m,0x2223Eu+off)))!=static_cast<s32>(a0))continue;
        if(static_cast<s16>(load_le16(dg(m,0x2223Au+off)))!=static_cast<s32>(a1))continue;
        return 1u;
    }
}
static_assert(0x2CAD3u-0x2CA5Cu==119u);

static void app65_mark_party_slot(Machine& m,Application65Boundary& b,u32 slot,u32 snd0,u32 snd1) noexcept {
    if(b.call_14c40)b.call_14c40(slot);
    if(b.call_1c160)b.call_1c160(snd0);
    if(b.call_1c124)b.call_1c124(snd1);
    store_le16(dg(m,0x218F0u+slot*0x248u),0x8000u);
    *dg(m,0xCF5Au+slot)=1u;
}

void function_2cad4(Machine& m,Application65Boundary& b) noexcept {
    store_le16(dg(m,0x255D2u),0x26u);
    store_le16(dg(m,0x255D0u),0x8000u);
    store_le16(dg(m,0x255D6u),0x37u);
    store_le16(dg(m,0x255D4u),0x8000u);
    store_le32(dg(m,0xC658u),0x5Au);
    if(b.call_181b4)b.call_181b4(0x18u);
    store_le32(dg(m,0xBF44u),0x12u);

    const u32 first=b.call_14a60?b.call_14a60(0x1338u):0u;
    const u32 second=b.call_14a60?b.call_14a60(0x1344u):0u;
    const u32 third=b.call_14a60?b.call_14a60(0x1350u):0u;
    for(u32 slot=0;slot<4u;++slot){
        if(slot==first){
            app65_mark_party_slot(m,b,slot,0x0Cu,0x17u);
            if((b.call_1c1dc?b.call_1c1dc(0x47u):0u)!=0u){
                if(b.call_1c160)b.call_1c160(8u);
                if(b.call_1c124)b.call_1c124(0x13u);
            }
        }
        if(slot==second && load_le16(dg(m,0x218F0u+slot*0x248u))<0x4000u)
            app65_mark_party_slot(m,b,slot,9u,0x14u);
        if(slot==third){
            store_le32(dg(m,0xBF44u),0x11u);
            app65_mark_party_slot(m,b,slot,8u,0x13u);
        }
    }
    if(b.call_0ccf4)b.call_0ccf4();
}
static_assert(0x2CC85u-0x2CAD4u==433u);

[[nodiscard]] u32 function_2cc88(Machine& m,Application65Boundary& b,u32 a0,u32 a1,u32 mode) noexcept {
    for(s32 index=0;;++index){
        const s32 count=static_cast<s16>(load_le16(dg(m,0x258A6u)));
        if(index>=count)return 0u;
        const u32 off=static_cast<u32>(index)*0x16u;
        const u32 x=load_le32(dg(m,0x2223Cu+off));
        const u32 y=load_le32(dg(m,0x22238u+off));
        const u32 type=load_le16(dg(m,0x22246u+off));
        const u32 meta=load_le32(dg(m,0x255DCu))+type*7u;
        const u32 radius=load_le16(dg(m,meta+2u));
        if(radius<=1u)continue;
        const u32 x0=x-radius,x1=x+radius,y0=y-radius,y1=y+radius;
        if(!(a1>=x0 && a1<=x1 && a0>=y0 && a0<=y1))continue;

        if(mode==0xFFFFFFFFu){
            if(type==0x54u){
                const u16 flags=load_le16(dg(m,0x2224Cu+off));
                if((flags&0x01C0u)!=0x0040u)continue;
                const u32 state=load_le32(dg(m,0xC6C8u));
                if(state==0x17u){
                    store_le16(dg(m,0x255D2u),3u);
                    store_le16(dg(m,0x255D0u),0x8000u);
                    store_le16(dg(m,0x255D6u),0x0Au);
                    store_le16(dg(m,0x255D4u),0x8000u);
                    if(b.call_1c0c0)b.call_1c0c0(0xB4u);
                    if(b.call_181b4)b.call_181b4(0x13u);
                    return 2u;
                }
                if(state==0x0Au){
                    store_le16(dg(m,0x255D2u),6u);
                    store_le16(dg(m,0x255D0u),0x8000u);
                    store_le16(dg(m,0x255D6u),0x3Eu);
                    store_le16(dg(m,0x255D4u),0x8000u);
                    if(b.call_1c0c0)b.call_1c0c0(0xB4u);
                    if(b.call_181b4)b.call_181b4(6u);
                    return 2u;
                }
                continue;
            }
            if(type==0x7Au){function_2cad4(m,b);return 2u;}
            return 1u;
        }

        if(static_cast<s32>(mode)>=0 && static_cast<s32>(mode)<0x190){
            const u32 ptype=load_le16(dg(m,0x14D8Eu+mode*0x16u));
            const u32 desc=load_le32(dg(m,0x22230u))+ptype*0x45u;
            if((*dg(m,desc+0x39u)&0x80u)!=0u)return 0u;
        }
        return 1u;
    }
}
static_assert(0x2CEB6u-0x2CC88u==558u);

static void app65_transition_frame(Machine& m,Application65Boundary& b) noexcept {
    store_le16(dg(m,0x255D2u),0x0Du);
    store_le16(dg(m,0x255D6u),0x23u);
    store_le32(dg(m,0xC658u),0xB4u);
    store_le16(dg(m,0x255D4u),0x8000u);
    store_le16(dg(m,0x255D0u),load_le16(dg(m,0x255D4u)));
    if(b.call_181b4)b.call_181b4(0x11u);
}

void function_2ceb8(Machine& m,Application65Boundary& b) noexcept {
    store_le32(dg(m,0xBF30u),2u);
    store_le32(dg(m,0x255D0u),load_le32(dg(m,0x2B634u)));
    store_le32(dg(m,0x255D4u),load_le32(dg(m,0x2B630u)));

    s32 frame=0;
    for(;frame<0x47;++frame){
        if(b.call_2e608)b.call_2e608();
        if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
        if(frame<0x0B || frame>0x44){if(b.call_1f1c8)b.call_1f1c8();}
        const u32 cell0=load_le32(dg(m,0x258ECu))+static_cast<u32>(frame);
        if(b.call_2e510)b.call_2e510(4u,0x20u,cell0,9u);
        const u32 cell1=load_le32(dg(m,0x258ECu))+static_cast<u32>(frame);
        if(b.call_2efd0)b.call_2efd0(cell1,9u);
        if(b.call_1ea04)b.call_1ea04();
        if(b.call_1ea78)b.call_1ea78();
        if(b.call_2e614)b.call_2e614(6u,0u);
        const u32 key=b.call_30b40?b.call_30b40():0u;
        if(key==0x1Bu)break;
        if(frame==0x0E)app65_transition_frame(m,b);
    }

    store_le32(dg(m,0xBF30u),0u);
    if(frame<=0x0E)app65_transition_frame(m,b);
}
static_assert(0x2D002u-0x2CEB8u==330u);

static_assert((0x2C7F7u-0x2C294u)+(0x2C8E2u-0x2C800u)+(0x2C9D9u-0x2C8E4u)+
              (0x2CA5Cu-0x2C9DCu)+(0x2CAD3u-0x2CA5Cu)+(0x2CC85u-0x2CAD4u)+
              (0x2CEB6u-0x2CC88u)+(0x2D002u-0x2CEB8u)==3418u);
