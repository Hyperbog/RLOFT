// Batch 95: 3395C..339A1 VGA sequencer map-mask and bounded string-copy helpers.
struct Runtime95Boundary {
    void (*out16)(u16,u16)=nullptr;
};

void function_3395c(Runtime95Boundary& b,u32 mask) noexcept {
    const u16 ax=static_cast<u16>((static_cast<u16>(mask&0xFFu)<<8)|0x02u);
    if(b.out16)b.out16(0x03C4u,ax);
}

u32 function_33971(Machine& m,u32 destination,u32 source,u32 count) noexcept {
    const u32 original_destination=destination;
    while(count!=0u){
        const u8 value=*dg(m,source);
        if(value==0u)break;
        ++source;
        --count;
        *dg(m,destination)=value;
        ++destination;
    }
    while(count!=0u){
        --count;
        *dg(m,destination)=0u;
        ++destination;
    }
    return original_destination;
}
static_assert(0x339A2u-0x3395Cu==70u);
