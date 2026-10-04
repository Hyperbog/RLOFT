// -----------------------------------------------------------------------------
// Batch 38: 1BD9C..1C0B8 world/record animation update dispatcher.
// 1BD9C..1BDAB is the exact four-entry CS table consumed at 1BE11;
// 1C0B9..1C0BF is bounded zero padding. The next routine begins at 1C0C0.
// -----------------------------------------------------------------------------

static constexpr u32 kFunction1bdacDispatch[4]={
    0x1BE70u,0x1BE19u,0x1BE4Au,0x1BE6Cu
};
static_assert(sizeof(kFunction1bdacDispatch)/sizeof(kFunction1bdacDispatch[0])==4u);

struct Application38Boundary {
    Application36Boundary* app36=nullptr;
    void (*call_0d550)(u32,u32,u32)=nullptr;
    u32 (*call_2388c)(u32,u32)=nullptr;
    void (*call_30cd4)(u32,u32,u32)=nullptr;
    void (*call_22e90)(u32,u32)=nullptr;
    void (*call_08c88)(s32)=nullptr;
    void (*call_09070)(u32,u32)=nullptr;
    void (*call_08cf4)(u32,u32)=nullptr;
};

static void app38_rotate_orient(Machine& m,u32 rec,u32 divisor) noexcept {
    const u32 orient=(app36_orient3(load_le16(dg(m,rec+0x14u)))+1u)%divisor;
    store_le16(dg(m,rec+0x14u),app36_set_orient3(load_le16(dg(m,rec+0x14u)),orient&7u));
}

void function_1bdac(Machine& m,Application38Boundary& b) noexcept {
    Application36Boundary* const a=b.app36;

    if(*dg(m,0xBF38u)!=0u) {
        u32 rec=0x14D80u;
        for(s32 index=0;;++index,rec+=0x16u) {
            const s32 count=static_cast<s16>(load_le16(dg(m,0x258A2u)));
            if(index>=count)break;

            const u32 type=load_le16(dg(m,rec+0x0Eu));
            const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
            if((*dg(m,rec+0x0Bu)&2u)!=0u)continue;

            u16 timer=load_le16(dg(m,rec+0x10u));
            if(timer!=0u) {
                store_le16(dg(m,rec+0x10u),static_cast<u16>(timer-1u));
                continue;
            }

            if((*dg(m,rec+8u)&8u)!=0u)
                *dg(m,rec+8u)=static_cast<u8>(*dg(m,rec+8u)&0xF7u);

            const u32 state=(static_cast<u32>(load_le16(dg(m,rec+0x14u)))>>1u)&3u;
            switch(state) {
                case 1u: {
                    *dg(m,rec+0x14u)=static_cast<u8>((*dg(m,rec+0x14u)&0xF9u)|4u);
                    if(b.call_0d550)b.call_0d550(rec,desc,static_cast<u32>(index));
                    timer=load_le16(dg(m,desc+0x25u));
                    if((*dg(m,rec+8u)&0x80u)!=0u)
                        timer=static_cast<u16>(timer*3u);
                    store_le16(dg(m,rec+0x10u),timer);
                    break;
                }
                case 2u:
                    timer=7u;
                    store_le16(dg(m,rec+0x10u),timer);
                    *dg(m,rec+0x14u)=static_cast<u8>(*dg(m,rec+0x14u)&0xF9u);
                    if((*dg(m,rec+8u)&0x80u)!=0u)
                        store_le16(dg(m,rec+0x10u),static_cast<u16>(timer*3u));
                    break;
                case 3u:
                    *dg(m,rec+0x14u)=static_cast<u8>(*dg(m,rec+0x14u)&0xF9u);
                    [[fallthrough]];
                case 0u:
                    if((*dg(m,rec+0x0Bu)&0x70u)!=0u) {
                        if(a)function_1bbd8(m,*a,rec,desc,static_cast<u32>(index));
                    } else if((*dg(m,rec+0x0Bu)&4u)!=0u) {
                        if(a)function_1b4f8(m,*a,rec,desc,static_cast<u32>(index));
                    } else {
                        const u32 divisor=*dg(m,desc+0x19u);
                        if(divisor!=0u)app38_rotate_orient(m,rec,divisor);
                    }
                    timer=load_le16(dg(m,desc+0x25u));
                    if((*dg(m,rec+8u)&0x80u)!=0u)
                        timer=static_cast<u16>(timer*3u);
                    store_le16(dg(m,rec+0x10u),timer);
                    break;
            }
        }
    }

    {
        u32 rec=0x22238u;
        for(s32 index=0;;++index,rec+=0x16u) {
            const s32 count=static_cast<s16>(load_le16(dg(m,0x258A6u)));
            if(index>=count)break;
            const u32 type=load_le16(dg(m,rec+0x0Eu));
            if(type==0x54u)continue;
            const u32 meta=load_le32(dg(m,0x255DCu))+type*7u;
            const u32 divisor=*dg(m,meta+5u);
            if(divisor==0u) {
                if(a&&a->divide_error)a->divide_error();
                return;
            }
            const u32 orient=(app36_orient3(load_le16(dg(m,rec+0x14u)))+1u)%divisor;
            store_le16(dg(m,rec+0x14u),app36_set_orient3(load_le16(dg(m,rec+0x14u)),orient&7u));
        }
    }

    for(s32 index=0;;++index) {
        const s32 count=static_cast<s16>(load_le16(dg(m,0x258A4u)));
        if(index>=count)break;
        const u32 rec=0x17618u+static_cast<u32>(index)*0x16u;
        if((*dg(m,rec+0x15u)&2u)==0u)continue;
        const u32 remove=b.call_2388c?b.call_2388c(rec,1u):0u;
        if(remove==0u)continue;

        const s32 live_count=static_cast<s16>(load_le16(dg(m,0x258A4u)));
        const u32 bytes=static_cast<u32>(live_count-1-index)*0x16u;
        if(b.call_30cd4)b.call_30cd4(rec,rec+0x16u,bytes);
        store_le16(dg(m,0x258A4u),static_cast<u16>(load_le16(dg(m,0x258A4u))-1u));
        --index; // encoded DEC ESI before the common increment/retest.
    }

    for(s32 index=0;;++index) {
        const s32 count=static_cast<s32>(load_le32(dg(m,0xC038u)));
        if(index>=count)break;
        const u32 rec=0x248ECu+static_cast<u32>(index)*0x16u;
        const u32 type=load_le16(dg(m,rec+0x0Eu));
        const u32 meta=load_le32(dg(m,0x22234u))+type*7u;

        if(type<8u) {
            if(*dg(m,0xBF38u)==0u)continue;
            if(b.call_22e90)b.call_22e90(rec,1u);
            if((*dg(m,rec+0x14u)&6u)!=0u) {
                if(b.call_08c88)b.call_08c88(index);
                --index;
            }
            continue;
        }

        if(*dg(m,0xBF38u)!=0u) {
            const u32 ok=a&&a->call_2d5d0?
                a->call_2d5d0(load_le32(dg(m,rec)),load_le32(dg(m,rec+4u)),0x190u):0u;
            if(static_cast<u16>(ok)==0u) {
                const bool type8_blocked=type==8u&&
                    (load_le16(dg(m,rec+0x14u))&0x01C0u)!=0u;
                // The binary contains CMP EDI,EDI here; it is always equal.
                if(!type8_blocked) {
                    if(b.call_09070)b.call_09070(rec,1u);
                    if(b.call_08cf4)b.call_08cf4(rec,1u);
                }
            }

            if(type==8u) {
                const u16 timer=load_le16(dg(m,rec+0x12u));
                if(timer==0u) {
                    if(b.call_08c88)b.call_08c88(index);
                    --index;
                    continue;
                }
                store_le16(dg(m,rec+0x12u),static_cast<u16>(timer-1u));
            }
        }

        const u32 divisor=*dg(m,meta+5u);
        if(divisor==0u) {
            if(a&&a->divide_error)a->divide_error();
            return;
        }
        const u32 orient=(app36_orient3(load_le16(dg(m,rec+0x14u)))+1u)%divisor;
        store_le16(dg(m,rec+0x14u),app36_set_orient3(load_le16(dg(m,rec+0x14u)),orient&7u));
    }
}
static_assert(0x1C0B9u-0x1BDACu==781u);
