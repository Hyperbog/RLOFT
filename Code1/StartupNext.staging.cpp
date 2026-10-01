/* CODE.1 startup/runtime continuation staging.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Instruction-bounded candidate code: CD97..CEBF, CEC0..CEC7,
 * CEC8..CEE8, CEF0..D08B = 750 bytes. NOT COUNTED until reconciled.
 * CEE9..CEEF = seven zero bytes / alignment-data candidate, excluded here.
 */
#include <cstdint>
#include <cstddef>
namespace code1_startup_next {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t; using s32=std::int32_t;

/* The boundary deliberately exposes DOS/selector/runtime operations rather than
 * replacing them with host-library behavior. The next verification pass must
 * reconcile register preservation and every callback contract before fold-in. */
struct Boundary {
    u8* dgroup=nullptr;
    u8 (*read_selector8)(u16,u32)=nullptr;
    u16 (*read_selector16)(u16,u32)=nullptr;
    u32 (*read_selector32)(u16,u32)=nullptr;
    void (*write_selector8)(u16,u32,u8)=nullptr;
    void (*write_selector16)(u16,u32,u16)=nullptr;
    void (*write_selector32)(u16,u32,u32)=nullptr;
    void (*dos_int21)(u32& eax,u32& ebx,u32& ecx,u32& edx,u16& ds,u16& es)=nullptr;
    void (*mouse_int33)(u32& eax,u32& ebx,u32& ecx,u32& edx)=nullptr;
    void (*call_void)(u32 address)=nullptr;
    u32 (*call_u32)(u32 address,u32 a,u32 b,u32 c)=nullptr;
};
static u16 get16(const u8*p) noexcept { return static_cast<u16>(p[0]|(static_cast<u16>(p[1])<<8)); }
static u32 get32(const u8*p) noexcept { return static_cast<u32>(p[0]|(static_cast<u32>(p[1])<<8)|(static_cast<u32>(p[2])<<16)|(static_cast<u32>(p[3])<<24)); }
static void put16(u8*p,u16 v) noexcept { p[0]=static_cast<u8>(v); p[1]=static_cast<u8>(v>>8); }
static void put32(u8*p,u32 v) noexcept { p[0]=static_cast<u8>(v); p[1]=static_cast<u8>(v>>8); p[2]=static_cast<u8>(v>>16); p[3]=static_cast<u8>(v>>24); }

/* CD97..CEBF (297 bytes): startup continuation.
 * CD97 records startup registers/selector state. CDB2..CDE9 copies the PSP
 * command tail after its leading spaces into the aligned DGROUP area at 8E8C.
 * CDED..CE1F walks the environment strings, detects case-insensitive `no87=`,
 * skips the double-NUL trailer metadata, and copies the executable path.
 * CE21..CE79 records resulting pointers and clears 55F8..8E8C (capped at 1000h
 * when startup byte 531Eh == 1). CE7F/CE86 call runtime initializers FCE8/FC98.
 * CE8E..CEBC is the encoded fatal/exit path: optionally open CC92, write the
 * supplied NUL-terminated message, call FD2B, then terminate via INT 21h/4Ch.
 * This staging routine keeps the register-derived inputs explicit; it is not
 * yet counted because the FCE8/FC98/FD2B contracts and PSP selector source are
 * still being reconciled. */
struct StartupRegs { u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0; u16 ds=0,es=0; };
void function_cd97_cebf(Boundary&b,StartupRegs&r) noexcept {
    if(!b.dgroup) return;
    b.dgroup[0x531e]=static_cast<u8>(r.eax);
    r.es=static_cast<u16>(r.ebx);
    if(b.write_selector16) b.write_selector16(r.es,0xcc90,r.ds);
    put32(b.dgroup+0x5320,r.esi); put16(b.dgroup+0x5324,static_cast<u16>(r.ecx));
    const u32 saved_esi=r.esi;
    r.es=get16(b.dgroup+0x52fc);
    r.edx=(0x8e8cu+0x0fu)&~0x0fu;
    r.ecx=b.read_selector8?b.read_selector8(r.es,r.edi-1u):0;
    /* CDBA..CDCF: REPZ SCASB for leading ASCII spaces. */
    while(r.ecx!=0 && b.read_selector8 && b.read_selector8(r.es,r.edi)==0x20u){++r.edi;--r.ecx;}
    r.esi=r.edi-1u; r.edi=r.edx;
    if(r.ecx!=0){++r.ecx; while(r.ecx--!=0){const u8 v=b.read_selector8?b.read_selector8(r.es,r.esi):0; b.dgroup[r.edi++]=v; ++r.esi;}}
    b.dgroup[r.edi++]=0; b.dgroup[r.edi++]=0;
    r.esi=saved_esi; --r.edi;
    /* CDED switches DS to the saved environment selector. */
    const u16 env_sel=get16(b.dgroup+0x5324); r.ebp=0;
    for(;;){
        const u32 first=b.read_selector32?b.read_selector32(env_sel,r.esi):0;
        if((first|0x20202020u)==0x37386f6eu && b.read_selector8 && b.read_selector8(env_sel,r.esi+4u)==static_cast<u8>('=')) ++r.ebp;
        while(b.read_selector8 && b.read_selector8(env_sel,r.esi)!=0) ++r.esi;
        ++r.esi;
        if(!b.read_selector8 || b.read_selector8(env_sel,r.esi)==0) break;
    }
    ++r.esi; ++r.esi; /* CE16 LODSB followed by two INC ESI: net +3 from second NUL. */
    while(b.read_selector8){const u8 v=b.read_selector8(env_sel,r.esi++); b.dgroup[r.edi++]=v; if(v==0) break;}
    put16(b.dgroup+0x531c,static_cast<u16>(r.ebp)); put32(b.dgroup+0x5300,r.edi);
    /* CE21 records current ESP in 52EC; caller supplies it in EBX for staging. */
    put32(b.dgroup+0x52ec,r.ebx);
    u32 clear_count=0x8e8cu-0x55f8u; if(b.dgroup[0x531e]==1u && clear_count>0x1000u) clear_count=0x1000u;
    for(u32 i=0;i<clear_count;++i)b.dgroup[0x55f8u+i]=0;
    put32(b.dgroup+0x52f4,(0x8e8cu+0x0fu)&~0x0fu); put32(b.dgroup+0x52f8,r.esi);
    if(b.call_void){b.call_void(0xfce8); b.call_void(0xfc98);}
}

/* CEC0..CEC7 (8 bytes): restore DS from the word stored in CS:CC90 and return. */
[[nodiscard]] u16 function_cec0(u16 saved_ds) noexcept { return saved_ds; }

/* CEC8..CEE8 (33 bytes): register-adapter wrapper around FEFC.
 * ESI/EDI/EDX/EBX receive the four stack arguments; FEFC is called; EAX is
 * then loaded from the dword pointed to by the third argument. */
[[nodiscard]] u32 function_cec8(Boundary&b,u32 a,u32 c,u32 d,u32 e) noexcept {
    (void)a; (void)c; (void)e; if(b.call_void)b.call_void(0xfefc);
    return b.read_selector32?b.read_selector32(0,d):0;
}

/* CEF0..CF40 (81 bytes): build a 0x300/0x33 descriptor-shaped local block,
 * seed its pointer/selector fields from the caller, and invoke the CEC8 adapter
 * with service 31h. The exact 84A0 and FEFC interfaces remain boundary-owned. */
void function_cef0(Boundary&b,u32 caller_ptr,u16 ds) noexcept {
    u32 local_desc[10]{}; local_desc[0]=0x300; local_desc[1]=0x33; local_desc[2]=0;
    u32 far_cell[3]{}; far_cell[2]=caller_ptr; reinterpret_cast<u16*>(far_cell)[0]=ds;
    if(b.call_u32)(void)b.call_u32(0x84a0,reinterpret_cast<u32>(far_cell),0,12);
    (void)function_cec8(b,0x31,reinterpret_cast<u32>(local_desc),reinterpret_cast<u32>(local_desc),reinterpret_cast<u32>(far_cell));
}

/* CF44..CF97 (84 bytes): initialize the 8A28 structure, derive the high word
 * of 30A8 into 8A4A, invoke CEF0, and return whether 8A44 became 1. */
[[nodiscard]] bool function_cf44(Boundary&b) noexcept {
    if(!b.dgroup)return false;
    if(b.call_u32)(void)b.call_u32(0x84a0,0x8a28,0,0x32);
    put32(b.dgroup+0x8a44,0x53c1); put16(b.dgroup+0x8a4a,static_cast<u16>(get32(b.dgroup+0x30a8)>>16)); put32(b.dgroup+0x8a3c,0);
    function_cef0(b,0x8a28,0); return get32(b.dgroup+0x8a44)==1u;
}

/* CF98..D08B (244 bytes): mouse/runtime startup and fallback shutdown path.
 * INT 33h function 0 stores AX/BX at 8A5A/8A5C and clears 8A5E. If a mouse is
 * present, 6F10 allocates/obtains a block recorded at 30A4 and CF44 may expose
 * its first byte as 8A5E; otherwise 6F74 receives word 30AC. If no mouse is
 * present, the encoded fallback calls 61D0, conditionally C4A0/B430/E6EA,
 * then 8424 and tail-jumps to CBF0 with argument 1. This remains staging until
 * those call contracts and the exact terminal/tail-call representation are
 * reconciled. */
struct MouseInitResult { u16 ax=0,bx=0,flag=0; u32 pointer=0; bool terminal_fallback=false; };
[[nodiscard]] MouseInitResult function_cf98_d08b(Boundary&b) noexcept {
    MouseInitResult out{}; if(!b.dgroup)return out;
    u32 eax=0,ebx=0,ecx=0,edx=0; if(b.mouse_int33)b.mouse_int33(eax,ebx,ecx,edx);
    out.ax=static_cast<u16>(eax); out.bx=static_cast<u16>(ebx); put16(b.dgroup+0x8a5a,out.ax); put16(b.dgroup+0x8a5c,out.bx); put16(b.dgroup+0x8a5e,0);
    if(out.ax!=0){
        if(b.call_u32)out.pointer=b.call_u32(0x6f10,0x40,get32(b.dgroup+0x30a8),get32(b.dgroup+0x30ac));
        put32(b.dgroup+0x30a4,out.pointer);
        if(out.pointer==0)return out;
        if(function_cf44(b)){out.flag=b.dgroup[out.pointer];put16(b.dgroup+0x8a5e,out.flag);}
        if(get16(b.dgroup+0x8a5e)==0 && b.call_u32)(void)b.call_u32(0x6f74,get16(b.dgroup+0x30ac),0,0);
    } else {
        if(b.call_u32)(void)b.call_u32(0x61d0,0x350,0,0);
        const u32 p=get32(b.dgroup+0x2aa0); const u32 pending=get32(b.dgroup+0x2b84)|get32(b.dgroup+0x2f48);
        if(p!=0 && get16(b.dgroup+p+8u)!=0x71u){if(get16(b.dgroup+p+0x14u)!=0 && b.call_void)b.call_void(0xc4a0); if(b.call_void)b.call_void(0xb430); if(pending!=0 && b.call_u32)(void)b.call_u32(0xe6ea,0,0,0);}
        if(b.call_void)b.call_void(0x8424); out.terminal_fallback=true;
    }
    out.flag=get16(b.dgroup+0x8a5e); return out;
}

static_assert(0xCEC0u-0xCD97u==297u);
static_assert(0xCEC8u-0xCEC0u==8u);
static_assert(0xCEE9u-0xCEC8u==33u);
static_assert(0xD08Cu-0xCEF0u==412u);
constexpr u32 kInstructionBoundedStagedBytes=750u;
} // namespace code1_startup_next
