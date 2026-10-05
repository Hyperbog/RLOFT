// Batch 107: 34998..34A73 self-modifying lookup-table initializer.
struct Runtime107Boundary {
    // 349C4/349D0/349EC/349F8 patch the immediate words of later MOV AX,imm16 instructions.
    void (*patch_code16)(u32,u16)=nullptr;
};

static u16 app107_imul_shld3(u16 ax,u16 dx) noexcept {
    const int product=static_cast<int>(static_cast<s16>(ax))*static_cast<int>(static_cast<s16>(dx));
    const u32 bits=static_cast<u32>(product);
    return static_cast<u16>(bits>>13); // exact low 16 bits of SHLD(high16,low16,3).
}

void function_34998(Machine& m,Runtime107Boundary& b) noexcept {
    const u32 table=load_le32(dg(m,0xC65Cu));
    const u32 base_angle=0x168u-load_le32(dg(m,0xC658u));

    u32 angle0=base_angle-0x20u;
    if(static_cast<s32>(angle0)<0)angle0+=0x168u;
    const u16 k0=static_cast<u16>(function_34270(m,angle0,table));
    const u16 k1=static_cast<u16>(function_34275(m,angle0,table));

    u32 angle1=base_angle+0x5Au;
    if(static_cast<s32>(angle1)>=0x168)angle1-=0x168u;
    const u16 k2=static_cast<u16>(function_34270(m,angle1,table));
    const u16 k3=static_cast<u16>(function_34275(m,angle1,table));

    if(b.patch_code16){
        b.patch_code16(0x34A11u,k0);
        b.patch_code16(0x34A27u,k1);
        b.patch_code16(0x34A3Du,k2);
        b.patch_code16(0x34A54u,k3);
    }

    u32 src=0u;
    u32 dst=0xFF58u;
    u32 count=0x80u;
    do {
        const u16 a=load_le16(dg(m,0x193E0u+src));
        const u16 c=load_le16(dg(m,0x19660u+src));
        *dg(m,dst+5u)=static_cast<u8>(app107_imul_shld3(k0,a));
        *dg(m,dst+4u)=static_cast<u8>(app107_imul_shld3(k1,a));
        store_le16(dg(m,dst+2u),app107_imul_shld3(k2,c));
        store_le16(dg(m,dst),app107_imul_shld3(k3,c));
        src+=2u;
        dst+=6u;
        --count;
    } while(count!=0u);
}

static_assert(0x34A74u-0x34998u==220u);
