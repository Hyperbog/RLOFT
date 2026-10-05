// Batch 109: 34BB0..34DE6 indexed quad construction and conditional raster dispatch.
struct Runtime109Boundary {
    // Existing resource-address helper: first argument is [258F8h] + record byte 9, second is 4.
    u32 (*call_2f29c)(u32,u32)=nullptr;
    // Represents LEA of the local 8-dword stack quad followed by CALL 33E6C.
    // Keeping this boundary explicit preserves the decoded DS:ESI/ES selector contract of 33E6C.
    void (*call_33e6c_local_quad)(Machine&,Runtime103Boundary&,const u32*)=nullptr;
};

static u32 app109_sar3(u32 v) noexcept {
    return (v>>3)|((v&0x80000000u)?0xE0000000u:0u);
}
static u32 app109_sar1(u32 v) noexcept {
    return (v>>1)|(v&0x80000000u);
}
static u32 app109_div8_trunc(u32 v) noexcept {
    return app109_sar3(v+((v&0x80000000u)?7u:0u));
}
static u32 app109_div2_trunc(u32 v) noexcept {
    return app109_sar1(v+((v&0x80000000u)?1u:0u));
}

void function_34bb0(Machine& m,Runtime109Boundary& b,Runtime103Boundary& raster,
                     const u32* coords,u32* indices,u32 record) noexcept {
    u32 quad_a[8]{};
    u32 quad_b[8]{};

    for(u32 i=0;i<4u;++i){
        // 34BE3 is a raw arithmetic SAR by 3, distinct from the bias-corrected divisions below.
        indices[i]=app109_sar3(indices[i]);
        const u32 limit=load_le32(dg(m,0xC738u));
        if(static_cast<s32>(indices[i])>static_cast<s32>(limit))indices[i]=limit;
        if(indices[i]==0u)return;

        const u32 width=load_le32(dg(m,0x15618u+indices[i]*4u));
        const u32 center=app109_div8_trunc(width)+0x80u;
        const u32 j=(i&1u)?(i^2u):i; // odd vertices 1 and 3 exchange places in the mirrored quad.

        quad_a[i*2u+1u]=coords[i];
        quad_b[j*2u+1u]=coords[i];
        const u32 half=app109_div2_trunc(width);
        quad_a[i*2u]=center+half;
        quad_b[j*2u]=center-half;
    }

    const bool alternate=((*dg(m,record+1u))&0x20u)!=0u;
    const u8 resource_index=*dg(m,record+9u);
    const u32 resource_id=load_le32(dg(m,0x258F8u))+static_cast<u32>(resource_index);
    if(!alternate){
        if(*dg(m,0xC026u)==0u)return;
        const u32 resource=b.call_2f29c?b.call_2f29c(resource_id,4u):0u;
        store_le32(dg(m,0xFDC0u),resource);
        store_le32(dg(m,0xFDC0u),load_le32(dg(m,0xFDC0u))+2u);
        store_le32(dg(m,0xFDB8u),0x84u);
        store_le32(dg(m,0xFDB4u),0x103u);
        if(b.call_33e6c_local_quad)b.call_33e6c_local_quad(m,raster,quad_a);
        return;
    }

    if(*dg(m,0xC027u)==0u)return;
    const u32 resource=b.call_2f29c?b.call_2f29c(resource_id,4u):0u;
    store_le32(dg(m,0xFDC0u),resource);
    store_le32(dg(m,0xFDC0u),load_le32(dg(m,0xFDC0u))+2u);
    store_le32(dg(m,0xFDB8u),0x04u);
    store_le32(dg(m,0xFDB4u),0x83u);
    if(b.call_33e6c_local_quad)b.call_33e6c_local_quad(m,raster,quad_b);
}

static_assert(0x34DE7u-0x34BB0u==567u);
// 34DE7 is an unreferenced NOP separator; 34DE8 has an independent direct caller.
