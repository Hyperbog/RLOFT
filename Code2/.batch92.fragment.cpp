// Batch 92: 337C8..33806 weighted-word lookup with signed starting slot.
u32 function_337c8(Machine& m,u32 table,u32 target) noexcept {
    u32 cx=0u;
    u32 slot=static_cast<u32>(static_cast<s32>(static_cast<signed char>(*dg(m,0x25621u))));
    slot<<=1;
    for(;;){
        cx=static_cast<u16>(static_cast<u16>(cx)+load_le16(dg(m,table+slot+0x102u)));
        if(static_cast<s32>(cx)>=static_cast<s32>(target))break;
        slot-=2u;
        if(static_cast<s32>(slot)>=0)continue;
        break;
    }
    const bool no_borrow=slot>=2u;
    slot-=2u;
    if(!no_borrow)slot=0u;
    return static_cast<u32>(load_le16(dg(m,table+slot)));
}
static_assert(0x33807u-0x337C8u==63u);
// 33807 is an unreferenced NOP alignment byte.
