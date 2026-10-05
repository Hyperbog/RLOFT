// Batch 103: 33E6C..33FB1 polygon/raster edge setup driver.
struct Runtime103Boundary {
    // Preserve the encoded DS:ESI -> ES:EDI REP MOVSD copy; source is supplied as an offset, not flattened to host stack memory.
    void (*rep_movsd_ds_es)(u32,u32,u32)=nullptr;
    // Forward local helpers remain address-specific until their own instruction ranges are folded.
    void (*call_3402c)(u32,u32)=nullptr; // ESI vertex index, EDI edge-state address.
    void (*call_34110)()=nullptr;
    bool (*call_33fb4)()=nullptr;        // true iff encoded helper returns CF=1.
};

void function_33e6c(Machine& m,Runtime103Boundary& b,u32 source) noexcept {
    const u32 polygon=0xFD2Cu;
    if(b.rep_movsd_ds_es)b.rep_movsd_ds_es(source,polygon,8u);

    s32 min_y=0x7FFFFFFF;
    s32 max_y=static_cast<s32>(0x80000001u);
    s32 min_x=0x7FFFFFFF;
    s32 max_x=static_cast<s32>(0x80000001u);
    u32 vertex=0u;
    u32 offset=0u;
    u32 count=4u;
    do {
        const s32 y=static_cast<s32>(load_le32(dg(m,0xFD30u+offset)));
        if(y<min_y){min_y=y;store_le32(dg(m,0xFD28u),vertex);}
        if(y>max_y){max_y=y;store_le32(dg(m,0xFD24u),vertex);}
        const s32 x=static_cast<s32>(load_le32(dg(m,0xFD2Cu+offset)));
        if(x<min_x)min_x=x;
        if(x>max_x)max_x=x;
        ++vertex;
        offset+=8u;
        --count;
    } while(count!=0u);

    if(max_x<=static_cast<s32>(load_le32(dg(m,0xFDB8u))))return;
    if(min_x>=static_cast<s32>(load_le32(dg(m,0xFDB4u))))return;
    if(max_y<=static_cast<s32>(load_le32(dg(m,0xFDB0u))))return;
    if(min_y>=static_cast<s32>(load_le32(dg(m,0xFDACu))))return;
    if(min_y>=max_y)return;

    store_le32(dg(m,0xFD20u),static_cast<u32>(min_y));
    store_le32(dg(m,0xFDC4u),1u);
    const u32 start_vertex=load_le32(dg(m,0xFD28u));
    if(b.call_3402c)b.call_3402c(start_vertex,0xFDC4u);
    store_le32(dg(m,0xFDE8u),3u);
    if(b.call_3402c)b.call_3402c(start_vertex,0xFDE8u);

    const s32 clip_top=static_cast<s32>(load_le32(dg(m,0xFDB0u)));
    const s32 current=static_cast<s32>(load_le32(dg(m,0xFD20u)));
    if(clip_top>current){
        u32 ecx=static_cast<u32>(load_le32(dg(m,0xFDB0u))-load_le32(dg(m,0xFD20u)));
        do {
            if(b.call_33fb4)(void)b.call_33fb4();
            store_le32(dg(m,0xFD20u),load_le32(dg(m,0xFD20u))+1u);
            --ecx;
        } while(ecx!=0u);
    }

    u32 ecx=load_le32(dg(m,0xFDACu))-load_le32(dg(m,0xFD20u));
    do {
        if(b.call_34110)b.call_34110();
        const bool carry=b.call_33fb4?b.call_33fb4():false;
        if(!carry)return;
        store_le32(dg(m,0xFD20u),load_le32(dg(m,0xFD20u))+1u);
        --ecx;
    } while(ecx!=0u);
}

static_assert(0x33FB2u-0x33E6Cu==326u);
// 33FB2..33FB3 is unreferenced XCHG EBX,EBX alignment; 33FB4 is a direct call target.
