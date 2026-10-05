// Batch 115: 35878..358D0 selector-aware copy/file-I/O helper.
struct Runtime115Boundary {
    // Encoded REP MOVS uses DS:source -> ES:destination.
    void (*copy_ds_to_es)(u16,u32,u16,u32,u32)=nullptr;
    u32 (*call_38407)(u32,u32,u32)=nullptr;
    u32 (*call_38106)(u32,u32,u32)=nullptr;
    u32 (*call_3967c)(u32)=nullptr;
};

[[nodiscard]] u32 function_35878(Machine& m,Runtime115Boundary& b,
                                  u32 source,u32 offset,u32 flags,
                                  u32 destination,u32 length) noexcept {
    if((static_cast<u8>(flags)&1u)!=0u) {
        // REPNE MOVSD/MOVSB is equivalent to REP for MOVS: ZF is ignored.
        // The encoded dword-then-byte split still copies exactly `length` bytes.
        if(b.copy_ds_to_es)
            b.copy_ds_to_es(m.cpu.ds,source+offset,m.cpu.es,destination,length);
        return offset+length;                            // LEA EAX,[EDX+EBX]
    }

    if(b.call_38407)(void)b.call_38407(source,offset,0u);
    if(b.call_38106)(void)b.call_38106(source,destination,length);
    return b.call_3967c?b.call_3967c(source):0u;
}
static_assert(0x358D1u-0x35878u==89u);
// 358D1..358D3 is LEA EAX,[EAX+0] alignment before independent 358D4.
static_assert(0x358D4u-0x358D1u==3u);
