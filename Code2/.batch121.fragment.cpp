// Batch 121: runtime renderer and multi-pass wrapper, 369D8..36B98.
struct Runtime121Boundary {
    u8 (*read_u8)(u32)=nullptr;
    u16 (*read_u16)(u32)=nullptr;
    u32 (*read_u32)(u32)=nullptr;
    void (*write_u8)(u32,u8)=nullptr;
    void (*write_u32)(u32,u32)=nullptr;
    void (*set_map_mask)(u8)=nullptr;
    void (*call_register_guest)(u32,u32,u32,u32)=nullptr; // target, ECX, ESI, EDI
};

static u32 batch121_swap_al_ah(u32 value) noexcept {
    return (value & 0xFFFF0000u) | ((value & 0xFFu) << 8) | ((value >> 8) & 0xFFu);
}

void function_369d8(Runtime121Boundary& b,u32 coordinate,u32 callback_ecx) noexcept {
    const auto r8=[&](u32 a)->u8{return b.read_u8?b.read_u8(a):0u;};
    const auto r16=[&](u32 a)->u16{return b.read_u16?b.read_u16(a):0u;};
    const auto r32=[&](u32 a)->u32{return b.read_u32?b.read_u32(a):0u;};
    const auto w8=[&](u32 a,u8 v){if(b.write_u8)b.write_u8(a,v);};
    u32 destination=((0x800u*static_cast<u32>(r8(0x0EF4Du)))+0xA000u)<<4;
    destination += r32(0x10450u)*0x50u + r32(0x0C6E0u) + static_cast<u32>(static_cast<s32>(r32(0x105E3u))>>2);
    const u32 phase_index=((r32(0x105E3u)&3u)<<3)+r32(0x105DFu);
    const u8 mask0=r8(0x0C668u+phase_index);
    const u8 mask1=r8(0x0C688u+phase_index);
    const u8 mask2=r8(0x0C6A8u+phase_index);
    u32 record=r32(0x105DBu);
    const u32 callback_image=r32(0x105D7u);
    for(;;){
        const s32 patch_index=static_cast<s16>(r16(record));
        if(patch_index==0)break;
        const u32 patch_offset=static_cast<u32>(r16(callback_image+static_cast<u32>(patch_index)));
        const u32 patch_address=callback_image+patch_offset;
        const u8 saved=r8(patch_address);
        w8(patch_address,0xC3u);
        const s32 callback_index=static_cast<s16>(r16(record+4u));
        const u32 target=callback_image+static_cast<u32>(r16(callback_image+static_cast<u32>(callback_index)));
        const u32 callback_esi=coordinate+static_cast<u32>(static_cast<s32>(static_cast<s16>(r16(record+2u))));
        record+=6u;
        if(b.set_map_mask)b.set_map_mask(mask0);
        if(b.call_register_guest)b.call_register_guest(target,callback_ecx,callback_esi,destination);
        if(mask1!=0u){
            if(b.set_map_mask)b.set_map_mask(mask1);
            if(b.call_register_guest)b.call_register_guest(target,callback_ecx,callback_esi,destination+1u);
        }
        if(mask2!=0u){
            if(b.set_map_mask)b.set_map_mask(mask2);
            if(b.call_register_guest)b.call_register_guest(target,callback_ecx,callback_esi,destination+2u);
        }
        w8(patch_address,saved);
    }
}
static_assert(0x36B13u-0x369D8u==315u);

// 36B13 is an unreferenced NOP separator.
void function_36b14(Runtime121Boundary& b,u32 coordinate) noexcept {
    const auto r8=[&](u32 a)->u8{return b.read_u8?b.read_u8(a):0u;};
    const auto r32=[&](u32 a)->u32{return b.read_u32?b.read_u32(a):0u;};
    const auto w32=[&](u32 a,u32 v){if(b.write_u32)b.write_u32(a,v);};
    if(r8(0x0C029u)==0u){
        function_369d8(b,coordinate,r32(0x105E7u));
        w32(0x105E7u,batch121_swap_al_ah(r32(0x105E7u)));
        return;
    }
    const u32 saved_phase=r32(0x105E3u);
    const u32 saved_count=r32(0x105DFu);
    const u32 packed=r32(0x105E7u);
    u32 remaining=saved_count;
    w32(0x105DFu,1u);
    do {
        u32 callback_ecx=packed;
        if((r32(0x105E3u)&1u)!=0u)callback_ecx=batch121_swap_al_ah(callback_ecx);
        function_369d8(b,coordinate,callback_ecx);
        w32(0x105E3u,r32(0x105E3u)+1u);
        --remaining;
    } while(remaining!=0u);
    w32(0x105E3u,saved_phase);
    w32(0x105DFu,saved_count);
}
static_assert(0x36B99u-0x36B14u==133u);

// 36B99..36B9B is unreferenced XCHG EBX,EBX; NOP alignment.
