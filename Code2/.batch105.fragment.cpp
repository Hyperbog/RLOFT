// Batch 105: 34270/34275 angle normalization and signed table lookup.
static u32 app105_34275(Machine& m,u32 eax,u32 ebx) noexcept {
    while((eax&0x80000000u)!=0u)eax+=0x168u;
    if(static_cast<s32>(eax)>=0x168)eax-=0x168u;

    u32 edi=eax;
    u32 quadrant=0u;
    if(static_cast<s32>(eax)>=0x10E){
        quadrant=3u;
        edi-=0x168u;
        edi=0u-edi;
    }else if(static_cast<s32>(eax)>=0xB4){
        quadrant=2u;
        edi-=0xB4u;
    }else if(static_cast<s32>(eax)>=0x5A){
        quadrant=1u;
        edi-=0xB4u;
        edi=0u-edi;
    }

    u32 value=static_cast<u32>(load_le16(dg(m,ebx+edi*4u)));
    ++value;
    value>>=2;
    if((quadrant&2u)!=0u)value=0u-value;
    return value;
}

u32 function_34270(Machine& m,u32 eax,u32 ebx) noexcept {
    return app105_34275(m,eax-0x10Eu,ebx);
}

u32 function_34275(Machine& m,u32 eax,u32 ebx) noexcept {
    return app105_34275(m,eax,ebx);
}

static_assert(0x342E2u-0x34270u==114u);
// 342E2..342E3 is unreferenced XCHG EBX,EBX alignment; 342E4 is independently called.
