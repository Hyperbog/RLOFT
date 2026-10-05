// Batch 108: 34A74..34BAC renderer driver with self-patched byte immediates.
struct Runtime108Boundary {
    void (*patch_code8)(u32,u8)=nullptr;
    // Translate an object-relative continuation into the guest return token stored by the stack-sharing renderer.
    u32 (*encode_return)(u32)=nullptr;
};
static u32 app108_return(Runtime108Boundary& b,u32 object_offset) noexcept {
    return b.encode_return?b.encode_return(object_offset):object_offset;
}

void function_34a74(Machine& m,Runtime108Boundary& b,Runtime106Boundary& raster) noexcept {
    const u8 add_dl=*dg(m,0xC044u);
    const u8 add_al=*dg(m,0xC048u);
    if(b.patch_code8){
        b.patch_code8(0x34AF9u,add_dl);
        b.patch_code8(0x34B69u,add_dl);
        b.patch_code8(0x34AF6u,add_al);
        b.patch_code8(0x34B66u,add_al);
    }

    store_le32(dg(m,0xFF40u),load_le32(dg(m,0xFF48u))+load_le32(dg(m,0xFDBCu)));
    store_le32(dg(m,0xFF54u),load_le32(dg(m,0xC040u))+2u);
    u32 esi=0xFF58u;
    u32 ecx=load_le32(dg(m,0xFF50u));
    if(static_cast<s32>(ecx)>=0x50)ecx=0x50u;
    u32 edi=0xFE94u+0x7Fu*load_le32(dg(m,0xFF4Cu));
    u8 shade=*dg(m,edi);
    if(b.patch_code8)b.patch_code8(0x34B07u,shade);

    do {
        const u32 saved_ecx=ecx;
        const u32 saved_edi=edi;
        const u32 saved_esi=esi;
        const u32 ebp=static_cast<u32>(load_le16(dg(m,esi)))<<16;
        const u32 ebx=static_cast<u32>(load_le16(dg(m,esi+2u)))<<16;
        const u8 al=static_cast<u8>(*dg(m,esi+4u)+add_al);
        const u8 dl=static_cast<u8>(*dg(m,esi+5u)+add_dl);
        u32 eax=static_cast<u32>(al)<<24;
        u32 edx=static_cast<u32>(dl)<<24;
        eax|=static_cast<u32>(shade)<<8;
        Raster106Pair p0{eax,edx};
        eax+=ebp;edx+=ebx;Raster106Pair p1{eax,edx};
        eax+=ebp;edx+=ebx;Raster106Pair p2{eax,edx};
        eax+=ebp;edx+=ebx;Raster106Pair p3{eax,edx};
        function_342e4(m,raster,app108_return(b,0x34B21u),ebp,ebx,p3,p2,p1,p0);
        ecx=saved_ecx;edi=saved_edi;esi=saved_esi;
        edi-=load_le32(dg(m,0xFF4Cu));
        esi+=6u;
        store_le32(dg(m,0xFF40u),load_le32(dg(m,0xFF40u))+load_le32(dg(m,0xFF44u)));
        shade=*dg(m,edi);
        if(b.patch_code8)b.patch_code8(0x34B07u,shade);
        --ecx;
    } while(ecx!=0u);

    if(b.patch_code8)b.patch_code8(0x34B77u,shade);
    ecx=load_le32(dg(m,0xFF50u))-0x50u;
    if(static_cast<s32>(ecx)>0){
        do {
            const u32 saved_ecx=ecx;
            const u32 saved_edi=edi;
            const u32 saved_esi=esi;
            const u32 ebp=static_cast<u32>(load_le16(dg(m,esi)))<<16;
            const u32 ebx=static_cast<u32>(load_le16(dg(m,esi+2u)))<<16;
            const u8 al=static_cast<u8>(*dg(m,esi+4u)+add_al);
            const u8 dl=static_cast<u8>(*dg(m,esi+5u)+add_dl);
            u32 eax=static_cast<u32>(al)<<24;
            u32 edx=static_cast<u32>(dl)<<24;
            eax|=static_cast<u32>(shade)<<8;
            const Raster106Pair p0{eax,edx};
            eax+=ebp;edx+=ebx;
            eax+=ebp;edx+=ebx;
            const Raster106Pair p2{eax,edx};
            function_34954(m,raster,app108_return(b,0x34B89u),ebp,ebx,p2,p0);
            ecx=saved_ecx;edi=saved_edi;esi=saved_esi;
            edi-=load_le32(dg(m,0xFF4Cu));
            esi+=6u;
            store_le32(dg(m,0xFF40u),load_le32(dg(m,0xFF40u))+load_le32(dg(m,0xFF44u)));
            shade=*dg(m,edi);
            if(b.patch_code8)b.patch_code8(0x34B77u,shade);
            --ecx;
        } while(ecx!=0u);
    }
}

static_assert(0x34BADu-0x34A74u==313u);
// 34BAD..34BAF is three unreferenced zero padding bytes; 34BB0 has an independent caller.
