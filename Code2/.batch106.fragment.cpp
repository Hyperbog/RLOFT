// Batch 106: 342E4..34994 stack-sharing planar renderers with shared 64-pixel sampler.
struct Runtime106Boundary {
    void (*out16)(u16,u16)=nullptr;
    // Preserve ES-qualified STOSB rather than flattening VGA memory into dgroup.
    void (*store_es8)(u32,u8)=nullptr;
};
struct Raster106Pair { u32 eax=0; u32 edx=0; };

static void app106_sample64(Machine& m,Runtime106Boundary& b,u32& eax,u32& edx,u32 ebp_step,u32 ebx_step,u32& edi) noexcept {
    for(u32 i=0;i<64u;++i){
        edx+=ebx_step;
        eax+=ebp_step;
        const u32 esi=((edx>>26)<<6)|(eax>>26);
        const u32 texture_base=load_le32(dg(m,0xFF54u));
        const u8 texture=*dg(m,texture_base+esi);
        const u8 ah=static_cast<u8>(eax>>8);
        const u8 al=static_cast<u8>(texture-ah);
        eax=(eax&0xFFFFFF00u)|static_cast<u32>(al); // LODS/SUB rewrite AL before the next unrolled pixel.
        if(b.store_es8)b.store_es8(edi,al);
        ++edi; // STOSB advances EDI even when the host boundary is absent.
    }
}

void function_342e4(Machine& m,Runtime106Boundary& b,u32 saved_return,u32 ebp,u32 ebx,
                     Raster106Pair plane8,Raster106Pair plane4,Raster106Pair plane2,Raster106Pair plane1) noexcept {
    store_le32(dg(m,0x10258u),saved_return);
    const u32 ebp_step=ebp<<2;
    const u32 ebx_step=ebx<<2;
    Raster106Pair planes[4]={plane8,plane4,plane2,plane1};
    const u16 masks[4]={0x0802u,0x0402u,0x0202u,0x0102u};
    for(u32 i=0;i<4u;++i){
        u32 edi=load_le32(dg(m,0xFF40u));
        if(b.out16)b.out16(0x03C4u,masks[i]);
        app106_sample64(m,b,planes[i].eax,planes[i].edx,ebp_step,ebx_step,edi);
    }
}

void function_34954(Machine& m,Runtime106Boundary& b,u32 saved_return,u32 ebp,u32 ebx,
                     Raster106Pair planes0c,Raster106Pair planes03) noexcept {
    store_le32(dg(m,0x10258u),saved_return);
    const u32 ebp_step=ebp<<2;
    const u32 ebx_step=ebx<<2;
    Raster106Pair planes[2]={planes0c,planes03};
    const u16 masks[2]={0x0C02u,0x0302u};
    for(u32 i=0;i<2u;++i){
        u32 edi=load_le32(dg(m,0xFF40u));
        if(b.out16)b.out16(0x03C4u,masks[i]);
        app106_sample64(m,b,planes[i].eax,planes[i].edx,ebp_step,ebx_step,edi);
    }
}

static_assert(0x34951u-0x342E4u==1645u);
// 34951..34953 is unreferenced XCHG EBX,EBX + NOP alignment between renderer entries.
static_assert(0x34995u-0x34954u==65u);
// 34995..34997 is unreferenced XCHG EBX,EBX + NOP alignment; 34998 has an independent caller.
