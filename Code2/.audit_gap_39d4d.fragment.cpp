// Semantic-audit explicit reconstruction of 39D4D..39FEC.
// This interval followed historical Batch 142 but had no explicit C++ ownership.
struct Runtime39d4dBoundary {
    // Exact runtime boundaries already reconstructed elsewhere.
    u32 (*alloc_39c7b)(u32)=nullptr;
    u32 (*init_arena_39bbc)(u32)=nullptr;
    void (*free_37c45)(u32)=nullptr;
    u32 (*probe_39c30)()=nullptr;
    u32 (*limit_3a467)(u32)=nullptr;
    // DOS AH=48h path: must preserve CF separately from EAX.
    struct Dos48Result { bool carry=false; u32 eax=0; };
    Dos48Result (*dos_alloc_48)(u32)=nullptr;
    u32 (*lsl_ds)()=nullptr;
};

[[nodiscard]] u32 function_39f77(Machine& m,Runtime39d4dBoundary& b,u32& size) noexcept {
    u32 ebx=size+3u;
    ebx=(ebx&0xFFFFFF00u)|((ebx&0xFFu)&0xFCu); // AND BL,FC
    if(ebx==0u)return 0u;
    const u8 mode=*dg(m,0x12662u);
    if((mode==1u && *dg(m,0x12663u)==0u) || mode==9u) ebx+=8u;
    else ebx-=b.probe_39c30?b.probe_39c30():0u;
    size=ebx;
    const u32 old=ebx;
    ebx+=0x3Cu;
    if(ebx<old)return 0u;
    const u32 floor=load_le32(dg(m,0x1270Cu));
    if(ebx<floor){ ebx=floor; ebx=(ebx&0xFFFFFF00u)|((ebx&0xFFu)&0xFEu); }
    size=ebx;
    const u32 prepage=ebx;
    ebx+=0xFFFu;
    if(ebx<prepage)return 0u;
    // AND BH,F0 / XOR BL,BL: preserve bits 16..31, clear low 12 bits.
    ebx&=0xFFFFF000u;
    size=ebx;
    return ebx!=0u;
}
static_assert(0x39FEDu-0x39F77u==118u);

[[nodiscard]] u32 function_39d4d(Machine& m,Runtime39d4dBoundary& b,u32 requested) noexcept {
    if(load_le32(dg(m,0x12708u))==0u || load_le32(dg(m,0x12634u))==0xFFFFFFFEu)return 0u;
    u32 size=requested;
    if(function_39f77(m,b,size)==0u)return 0u;
    u32 owner=0u;
    if(*dg(m,0x12662u)==1u){
        owner=b.alloc_39c7b?b.alloc_39c7b(size):0u;
        if(owner!=0u)size=load_le32(dg(m,owner));
    }else{
        const auto r=b.dos_alloc_48?b.dos_alloc_48(size):Runtime39d4dBoundary::Dos48Result{true,0u};
        owner=r.carry?0u:r.eax; // SBB/NOT/AND folds CF, not EAX value.
        size-=4u;
    }
    if(owner==0u)return 0u;
    const u32 old=size;
    size-=4u;
    if(size>old || size<0x38u)return 0u;
    store_le32(dg(m,owner),size);
    u32 block=b.init_arena_39bbc?b.init_arena_39bbc(owner):0u;
    u32 block_size=load_le32(dg(m,block));
    size=block_size;
    block_size=(block_size&0xFFFFFF00u)|((block_size&0xFFu)|1u); // OR AL,1
    store_le32(dg(m,block),block_size);
    const u32 payload=block+4u;
    store_le32(dg(m,owner+0x14u),0xFFFFFFFFu);
    store_le32(dg(m,owner+0x18u),load_le32(dg(m,owner+0x18u))+1u);
    if(b.free_37c45)b.free_37c45(payload);
    return 1u;
}
static_assert(0x39E1Cu-0x39D4Du==207u);

[[nodiscard]] u32 function_39e1c(Machine& m,Runtime39d4dBoundary& b,u32 requested) noexcept {
    const u8 mode=*dg(m,0x12662u);
    if((mode==1u && *dg(m,0x12663u)==0u) || mode==9u)
        return function_39d4d(m,b,requested);
    if(load_le32(dg(m,0x12708u))==0u || load_le32(dg(m,0x12634u))==0xFFFFFFFEu)return 0u;
    u32 size=requested;
    if(function_39f77(m,b,size)==0u)return 0u;
    if(mode>=2u && mode<=8u && *dg(m,0x12664u)==0u){
        const u32 lim=b.lsl_ds?b.lsl_ds():0u;
        store_le32(dg(m,0x12634u),lim+1u);
    }
    const u32 ceiling=load_le32(dg(m,0x12634u));
    u32 target=size+ceiling;
    if(target<ceiling)target=0xFFFFFFFEu;
    const u32 base=b.limit_3a467?b.limit_3a467(target):0xFFFFFFFFu;
    if(base==0xFFFFFFFFu || base>0xFFFFFFF8u || target<=base)return 0u;
    u32 extent=target-base;
    const u32 old_extent=extent;
    extent-=4u;
    if(extent>old_extent)return 0u;

    u32 owner=load_le32(dg(m,0x12624u));
    while(owner!=0u){
        const u32 next=load_le32(dg(m,owner+8u));
        if(next==0u)break;
        if(owner>base || base<next)break;
        owner=next;
    }
    u32 block=0u;
    if(owner!=0u && owner+load_le32(dg(m,owner))==base-4u){
        extent+=4u;
        store_le32(dg(m,owner),load_le32(dg(m,owner))+extent);
        block=owner+load_le32(dg(m,owner))-extent;
        store_le32(dg(m,block+extent),0xFFFFFFFFu);
    }else{
        if(extent<0x38u)return 0u;
        owner=base;
        store_le32(dg(m,owner),extent);
        block=b.init_arena_39bbc?b.init_arena_39bbc(owner):0u;
        extent=load_le32(dg(m,block));
    }
    u32 marked=extent;
    marked=(marked&0xFFFFFF00u)|((marked&0xFFu)|1u); // OR AL,1
    store_le32(dg(m,block),marked);
    const u32 payload=block+4u;
    store_le32(dg(m,owner+0x14u),0xFFFFFFFFu);
    store_le32(dg(m,owner+0x18u),load_le32(dg(m,owner+0x18u))+1u);
    if(b.free_37c45)b.free_37c45(payload);
    return 1u;
}
static_assert(0x39F77u-0x39E1Cu==347u);
static_assert(0x39FEDu-0x39D4Du==672u);
