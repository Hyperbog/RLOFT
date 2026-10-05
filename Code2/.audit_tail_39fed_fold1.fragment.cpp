// Semantic-audit reconstruction of final-tail fold 1: 39FED..3A1B2 (454 bytes).
// Immutable object-1 controls semantics. DOS results keep carry/error folding explicit.
struct RuntimeTail1Boundary {
    struct DosResult { bool carry=false; u32 eax=0, edx=0; };
    DosResult (*dos_seek_end)(u32 handle)=nullptr;
    DosResult (*dos_write)(u32 handle,u32 buffer,u32 count)=nullptr;
    DosResult (*dos_ioctl_4400)(u32 handle)=nullptr;
    DosResult (*dos_close)(u32 handle)=nullptr;
    u32 (*normalize_dos_error_390d7)(u16)=nullptr;
    void (*set_runtime_error_37cc2)(u32)=nullptr;
    void (*fatal_3a536)(u32,u32)=nullptr;
};

[[nodiscard]] u32 function_39fed() noexcept { return 0u; }
[[nodiscard]] u32 function_39ff0() noexcept { return 0x2BCFCu; }
[[nodiscard]] u32 function_39ff6() noexcept { return 0x2BCF8u; }

[[nodiscard]] u32 function_3a0a8(RuntimeTail1Boundary& b,u32 handle) noexcept {
    const auto r=b.dos_ioctl_4400?b.dos_ioctl_4400(handle):RuntimeTail1Boundary::DosResult{};
    // Binary tests DL bit 7 returned by IOCTL 4400h. Carry is folded into EDX bit31
    // by RCL/ROR but does not replace the device bit test.
    return ((r.edx&0x80u)!=0u)?1u:0u;
}

void function_39ffc(Machine& m,RuntimeTail1Boundary& b,u32 stream) noexcept {
    u8 flags=*dg(m,stream+0x0Du);
    if((flags&0x20u)!=0u)return;
    if(function_3a0a8(b,load_le32(dg(m,stream+0x10u)))==0u)return;
    flags=static_cast<u8>(flags|0x20u);
    *dg(m,stream+0x0Du)=flags;
    if((flags&0x07u)==0u)*dg(m,stream+0x0Du)=static_cast<u8>(flags|0x02u);
}

[[nodiscard]] u32 function_3a0c4(Machine& m,RuntimeTail1Boundary& b,u32 handle) noexcept {
    if(handle>=load_le32(dg(m,0x12694u)))return 0u;
    if(static_cast<s32>(handle)<=5){
        const u32 table=load_le32(dg(m,0x126E8u));
        const u32 slot=table+handle*4u;
        u8 f=*dg(m,slot+1u);
        if((f&0x40u)==0u){
            *dg(m,slot+1u)=static_cast<u8>(f|0x40u);
            if(function_3a0a8(b,handle)!=0u){
                const u32 table2=load_le32(dg(m,0x126E8u)); // reload across call
                *dg(m,table2+handle*4u+1u)=static_cast<u8>(*dg(m,table2+handle*4u+1u)|0x20u);
            }
        }
    }
    const u32 table=load_le32(dg(m,0x126E8u));
    return load_le32(dg(m,table+handle*4u));
}

void function_3a11a(Machine& m,u32 handle,u32 value) noexcept {
    // OR DH,40h changes byte 1 of EDX before the complete dword is published.
    value|=0x00004000u;
    const u32 table=load_le32(dg(m,0x126E8u));
    store_le32(dg(m,table+handle*4u),value);
}

[[nodiscard]] u32 function_3a02f(Machine& m,RuntimeTail1Boundary& b,
                                 u32 handle,u32 buffer,u32 count) noexcept {
    const u32 descriptor=function_3a0c4(m,b,handle);
    if((descriptor&0x80u)!=0u){
        const auto s=b.dos_seek_end?b.dos_seek_end(handle):RuntimeTail1Boundary::DosResult{true,0u,0u};
        if(s.carry){
            const u16 err=static_cast<u16>(s.eax);
            return b.normalize_dos_error_390d7?b.normalize_dos_error_390d7(err):0u;
        }
    }
    const auto w=b.dos_write?b.dos_write(handle,buffer,count):RuntimeTail1Boundary::DosResult{true,0u,0u};
    if(w.carry){
        const u16 err=static_cast<u16>(w.eax);
        return b.normalize_dos_error_390d7?b.normalize_dos_error_390d7(err):0u;
    }
    const u32 wrote=w.eax&0x7FFFFFFFu; // RCL/ROR carries CF into sign bit; success has CF clear.
    if(wrote!=count && b.set_runtime_error_37cc2)b.set_runtime_error_37cc2(0x0Cu);
    return wrote;
}

[[nodiscard]] u32 function_3a130(Machine& m,RuntimeTail1Boundary& b,u32 handle) noexcept {
    const auto r=b.dos_close?b.dos_close(handle):RuntimeTail1Boundary::DosResult{true,0u,0u};
    if(!r.carry){ function_3a11a(m,handle,0u); return 0u; }
    if(b.set_runtime_error_37cc2)b.set_runtime_error_37cc2(4u);
    return 0xFFFFFFFFu;
}

[[nodiscard]] u32 function_3a165(Machine& m,u32 current_esp) noexcept {
    return current_esp-load_le32(dg(m,0x12644u));
}
void function_3a16e(Machine& m,u16 ss) noexcept { store_le16(dg(m,0x126ECu),ss); }

// 3A176 and 3A186 are address-distinct RET 4 entry points and must not be merged.
// 3A176 swaps EAX with caller slot +4, calls 3A189 using the swapped value, restores
// EAX from that slot, then RET 4. 3A186 is a bare RET 4.
struct StackGuard3a176Result { u32 eax=0; bool ret4=true; };
[[nodiscard]] StackGuard3a176Result function_3a176(Machine& m,RuntimeTail1Boundary& b,
                                                   u32 eax,u32 caller_slot,u32 esp,u16 ss) noexcept {
    const u32 probe=eax;
    eax=caller_slot;
    bool fail=false;
    if(probe>=esp)fail=true;
    else {
        const u32 distance=esp-probe;
        if(distance>load_le32(dg(m,0x12644u)))fail=true;
    }
    if(!fail && ss!=load_le16(dg(m,0x126ECu)))fail=true;
    if(fail && b.fatal_3a536)b.fatal_3a536(1u,0x126EEu);
    // Encoded 3A1AE call has no local epilogue; if the fatal boundary returns,
    // execution falls through into 3A1B3. That odd control-flow edge is intentional.
    return {eax,true};
}
[[nodiscard]] StackGuard3a176Result function_3a186(u32 eax) noexcept { return {eax,true}; }

static_assert(0x3A1B3u-0x39FEDu==454u);