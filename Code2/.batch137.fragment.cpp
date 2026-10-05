
// Batch 137: 3908C..39588. DOS error normalization plus generic interrupt-register bridges.
struct Runtime137Boundary {
    void (*translate_dos_error)(u32)=nullptr;
    void (*set_errno)(u32)=nullptr;
    // Execute interrupt `vector` with the supplied register image.  The boundary
    // owns host/extender mechanics; the image preserves the encoded register set.
    void (*interrupt)(u8 vector,u32* regs)=nullptr;
};

[[nodiscard]] s32 function_3908c(Runtime137Boundary& b,bool carry,u32 ax) noexcept {
    if(!carry)return 0;
    if(b.translate_dos_error)b.translate_dos_error(ax&0xFFFFu);
    return -1;
}
void function_3909f() noexcept {}
[[nodiscard]] u32 function_390a0(Runtime137Boundary& b,u32 value,u32 error) noexcept {
    if(error==0u)return 0u;
    if(b.translate_dos_error)b.translate_dos_error(value);
    return 0xFFFFFFFFu;
}
[[nodiscard]] u32 function_390ba(Runtime137Boundary& b,u32 value,u32 error) noexcept {
    if(error!=0u){if(b.translate_dos_error)b.translate_dos_error(value);return 0xFFFFFFFFu;}
    return value;
}
[[nodiscard]] s32 function_390d7(Machine& m,Runtime137Boundary& b,u32 dos_error) noexcept {
    u8 e=static_cast<u8>(dos_error);
    // The helper first publishes the raw DOS error through 37CCE.
    if(b.translate_dos_error)b.translate_dos_error(e);
    u32 mapped;
    if(dos_error<0x100u){
        if(*dg(m,0x1266Bu)>=3u){if(e==0x50u)e=0x0Eu;else if(e>=0x22u)e=0x13u;else if(e>=0x20u)e=0x05u;}
        if(e>0x13u)e=0x13u;
        // 12671h is the historical signed-byte DOS->errno table. Keep table access exact.
        mapped=static_cast<u32>(static_cast<s32>(static_cast<s8>(*dg(m,0x12671u+e*4u+3u))));
    } else mapped=(dos_error>>8u)&0xFFu;
    if(b.set_errno)b.set_errno(mapped);
    return -1;
}

struct Interrupt137Regs {u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0,flags=0;u16 ds=0,es=0,fs=0,gs=0;};
void function_3914a(Runtime137Boundary& b,u8 vector,Interrupt137Regs& r) noexcept {
    u32 image[12]={r.eax,r.ebx,r.ecx,r.edx,r.esi,r.edi,r.ebp,r.flags,r.ds,r.es,r.fs,r.gs};
    if(b.interrupt)b.interrupt(vector,image);
    r.eax=image[0];r.ebx=image[1];r.ecx=image[2];r.edx=image[3];r.esi=image[4];r.edi=image[5];r.ebp=image[6];r.flags=image[7];
}
void function_391a5(Runtime137Boundary& b,u8 vector,Interrupt137Regs& r) noexcept {function_3914a(b,vector,r);}

// 39226..39525 is a dense 256-entry executable dispatch table: each entry is
// exactly `INT imm8; RET` (3 bytes), indexed by vector*3 by both bridge paths.
void function_39226(Runtime137Boundary& b,u8 vector,Interrupt137Regs& r) noexcept {function_3914a(b,vector,r);}

[[nodiscard]] s32 function_39526(const u8* lhs,const u8* rhs,u32 count) noexcept {
    if(count==0u)return 0;
    while(count--){u8 a=*lhs++,c=*rhs++;if(a>='A'&&a<='Z')a=static_cast<u8>(a+0x20u);if(c>='A'&&c<='Z')c=static_cast<u8>(c+0x20u);if(a!=c)return static_cast<s32>(a)-static_cast<s32>(c);if(c==0u)return 0;}
    return 0;
}
static_assert(0x39147u-0x3908Cu==187u);
static_assert(0x3914Au-0x39147u==3u);
static_assert(0x39226u-0x3914Au==220u);
static_assert(0x39526u-0x39226u==768u);
static_assert(0x39589u-0x39526u==99u);
