
// Batch 126: INT 24h bridge plus 4 KiB dictionary decompressor.
struct Runtime126Boundary {
    u8 (*read8)(u32)=nullptr; void (*write8)(u32,u8)=nullptr;
    void (*install_interrupt)(u8,u16,u32)=nullptr; // vector, selector, offset
    void (*save_extender_state)()=nullptr;
    u32 (*invoke_saved_far)(u32,u32,u32,u32)=nullptr; // eax, edx, linear frame, selector/context
    u16 (*get_input)()=nullptr; void (*put_output)(u8)=nullptr;
};
static inline u8 runtime126_read8(Runtime126Boundary& b,u32 p) noexcept { return b.read8?b.read8(p):0u; }
static inline void runtime126_write8(Runtime126Boundary& b,u32 p,u8 v) noexcept { if(b.write8)b.write8(p,v); }

// 375FE..37641. Stores the previous-handler far pointer in 105F2:105F6 and installs
// the CS-local 37642 bridge. Extender modes 2..6 use selector 24h / vector 06h;
// other modes install ordinary INT 24h.
[[nodiscard]] u32 function_375fe(Runtime126Boundary& b,u32 old_offset,u16 old_selector,u8 extender_mode) noexcept {
    (void)old_offset; (void)old_selector;
    if(b.install_interrupt){
        if(extender_mode>=2u && extender_mode<=6u)b.install_interrupt(0x06u,0x24u,0x37642u);
        else b.install_interrupt(0x24u,0u,0x37642u);
    }
    return 0u;
}
static_assert(0x37642u-0x375FEu==68u);

// 37642..376E8. The binary saves the complete interrupt-visible register set,
// snapshots SS:ESP and 12644, switches to its private 2B8F0/2BCF0 stack arena,
// converts BP:SI to a linear address, then far-calls the saved handler. LSS restores
// the original stack before the exact pop sequence and IRET.
[[nodiscard]] u32 interrupt_bridge_37642(Runtime126Boundary& b,u32 eax,u32 edx,u16 bp,u16 si,u16 di,u8 extender_mode) noexcept {
    if(b.save_extender_state)b.save_extender_state();
    const u32 linear=(static_cast<u32>(bp)<<4)+static_cast<u32>(si);
    u32 context=0x34u;
    if(extender_mode==1u)context=0u; // encoded path uses current DS selector; boundary supplies it externally when needed.
    if(extender_mode>=2u && extender_mode<=6u)context=0x34u;
    return b.invoke_saved_far?b.invoke_saved_far(eax,static_cast<u32>(di),linear,context):edx;
}
static_assert(0x376E9u-0x37642u==167u);

// 376F0 is an address-taken one-byte RET stub.
void function_376f0() noexcept {}
static_assert(0x376F1u-0x376F0u==1u);

// 37700..3780C. 4 KiB ring-dictionary decoder. The original intentionally uses
// 16-bit SI/BX/DI arithmetic and masks dictionary positions with 0FFFh.
void function_37700(Runtime126Boundary& b,u8 stop_flag) noexcept {
    u8 dict[4096];
    for(u32 i=0;i<4096u;++i)dict[i]=0xFEu;
    u16 write_pos=0x0FEEu;
    u16 flags=0u;
    for(;;){
        flags=static_cast<u16>(flags>>1);
        if((flags&0x0100u)==0u){
            if(stop_flag==1u)return;
            const u16 v=b.get_input?b.get_input():0u;
            flags=static_cast<u16>((v&0x00FFu)|0xFF00u);
        }
        if((flags&1u)!=0u){
            if(stop_flag==1u)return;
            const u8 c=static_cast<u8>(b.get_input?b.get_input():0u);
            if(b.put_output)b.put_output(c);
            dict[write_pos]=c; write_pos=static_cast<u16>((write_pos+1u)&0x0FFFu);
        } else {
            if(stop_flag==1u)return;
            const u16 lo=b.get_input?b.get_input():0u;
            if(stop_flag==1u)return;
            const u16 hi=b.get_input?b.get_input():0u;
            const u16 source=static_cast<u16>((lo&0x00FFu)|((hi&0x00F0u)<<4));
            const u16 last=static_cast<u16>((hi&0x000Fu)+2u);
            for(u16 i=0u;i<=last;++i){
                const u8 c=dict[static_cast<u16>((source+i)&0x0FFFu)];
                if(b.put_output)b.put_output(c);
                dict[write_pos]=c; write_pos=static_cast<u16>((write_pos+1u)&0x0FFFu);
            }
        }
    }
}
static_assert(0x3780Du-0x37700u==269u);
