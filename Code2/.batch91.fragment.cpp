// Batch 91: 335DC..337C6 two-pass record-run coalescing and flush traversal.
struct Application91Boundary {
    u32 (*call_2f29c)(u32,u32)=nullptr;
    void (*call_3263c)()=nullptr;
};

void function_335dc(Machine& m,Application91Boundary& b) noexcept {
    const u32 record_base=0x15A18u;
    const u32 record_size=0x1Cu;
    const auto rec=[&](u32 i) noexcept { return record_base+i*record_size; };
    const auto flush=[&]() noexcept { if(b.call_3263c)b.call_3263c(); };
    const auto initial_width=[&](u32 record) noexcept {
        store_le32(dg(m,0x255E4u),load_le32(dg(m,record+0x10u))<<1);
        const u32 extra=(b.call_2f29c?b.call_2f29c(load_le32(dg(m,0xC700u)),4u):0u)+2u;
        store_le32(dg(m,0x255E4u),load_le32(dg(m,0x255E4u))+extra);
    };
    const auto reset_width=[&](u32 record,u32 value_field) noexcept {
        store_le32(dg(m,0xC700u),load_le32(dg(m,record+value_field)));
        const u32 base=(b.call_2f29c?b.call_2f29c(load_le32(dg(m,0xC700u)),4u):0u)+2u;
        store_le32(dg(m,0x255E4u),base);
        store_le32(dg(m,0x255E4u),load_le32(dg(m,0x255E4u))+(load_le32(dg(m,record+0x10u))<<1));
    };

    u32 index=0u;
    store_le32(dg(m,0xFCD0u),1u);
    store_le32(dg(m,0xC6F4u),1u);
    store_le32(dg(m,0xC6F8u),0u);
    store_le32(dg(m,0xFCF2u),0u);
    const u32 total=load_le32(dg(m,0xC6D8u));

    for(;;){
        while(load_le32(dg(m,rec(index)+0x0Cu))!=1u){
            ++index;
            if(index==total)break;
        }
        if(static_cast<s32>(index)>=static_cast<s32>(total))break;

        u32 current=rec(index);
        store_le32(dg(m,0xC6F8u),index);
        store_le32(dg(m,0xFCF2u),load_le32(dg(m,current+0x18u)));
        store_le32(dg(m,0xC700u),load_le32(dg(m,current+0x04u)));
        initial_width(current);
        ++index;

        for(;;){
            if(index==total){
                flush();
                goto first_pass_done;
            }
            current=rec(index);
            if(load_le32(dg(m,current+0x0Cu))!=1u){
                flush();
                store_le32(dg(m,0xC6F4u),1u);
                break;
            }

            const u32 previous=current-record_size;
            if(load_le32(dg(m,current))!=load_le32(dg(m,previous)) ||
               load_le32(dg(m,current+0x10u))!=load_le32(dg(m,previous+0x10u))){
                flush();
                store_le32(dg(m,0xC6F4u),1u);
                store_le32(dg(m,0xC6F8u),index);
                store_le32(dg(m,0xFCF2u),load_le32(dg(m,current+0x18u)));
                reset_width(current,0x04u);
            }else{
                store_le32(dg(m,0xC6F4u),load_le32(dg(m,0xC6F4u))+1u);
            }
            ++index;
        }
    }
first_pass_done:

    index=0u;
    store_le32(dg(m,0xFCD0u),0u);
    store_le32(dg(m,0xC6F4u),1u);
    store_le32(dg(m,0xC6F8u),0u);
    u32 current=rec(index);
    store_le32(dg(m,0xC700u),load_le32(dg(m,current+0x08u)));
    store_le32(dg(m,0xFCF2u),load_le32(dg(m,current+0x18u)));
    initial_width(current);

    for(;;){
        ++index;
        if(index==total)break;
        current=rec(index);
        const u32 previous=current-record_size;
        if(load_le32(dg(m,current))!=load_le32(dg(m,previous)) ||
           load_le32(dg(m,current+0x10u))!=load_le32(dg(m,previous+0x10u))){
            flush();
            store_le32(dg(m,0xC6F4u),1u);
            store_le32(dg(m,0xC6F8u),index);
            store_le32(dg(m,0xFCF2u),load_le32(dg(m,current+0x18u)));
            reset_width(current,0x08u);
        }else{
            store_le32(dg(m,0xC6F4u),load_le32(dg(m,0xC6F4u))+1u);
        }
    }
    flush();
}
static_assert(0x337C7u-0x335DCu==491u);
// 337C7 is an unreferenced NOP alignment byte.
