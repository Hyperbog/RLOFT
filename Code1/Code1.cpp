/* Ravenloft: Strahd's Possession - CODE.1 faithful reconstruction
 * Immutable source: CODE.1, 277787 bytes
 * SHA-256 fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Instruction-accountable reconstruction; unknown semantics are not guessed. */
#include <cstddef>
#include <cstdint>
#include <limits>
namespace code1 {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
static_assert(sizeof(u8)==1 && sizeof(u16)==2 && sizeof(u32)==4);
constexpr std::size_t kSourceFileSize=277787u;
constexpr char kSourceSha256[]="fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed";

constexpr u32 kOuterMzFileOffset=0x00000000u, kBoundBwFileOffset=0x0000F444u;
constexpr u32 kApplicationMzFileOffset=0x00025D24u, kApplicationLeFileOffset=0x00027BDCu;
constexpr u32 kApplicationDataFileOffset=0x0002C724u;
constexpr u16 kLeCpuType=0x0002u, kLeOsType=0x0001u;
constexpr u32 kLeModuleFlags=0x00000200u, kLeModulePages=24u, kLePageSize=0x1000u, kLeObjectCount=2u;
constexpr u32 kEntryObject=1u, kEntryOffset=0xCC1Cu, kStackObject=2u, kInitialEsp=0x9E90u, kAutoDataObject=2u;
constexpr u32 kCodeObjectVirtualSize=0x11302u, kCodeObjectBase=0x10000u, kCodeObjectFlags=0x2005u;
constexpr u32 kCodeObjectFirstPage=1u, kCodeObjectPageCount=18u;
constexpr u32 kDataObjectVirtualSize=0x9E90u, kDataObjectBase=0x30000u, kDataObjectFlags=0x2003u;
constexpr u32 kDataObjectFirstPage=19u, kDataObjectPageCount=6u;
constexpr u32 kLeDataPagesRelativeToAppMz=0x6A00u, kFinalPageBytes=0x5F7u;
constexpr u32 kApplicationPageDataBytes=(kLeModulePages-1u)*kLePageSize+kFinalPageBytes;
static_assert(kApplicationMzFileOffset+kLeDataPagesRelativeToAppMz==kApplicationDataFileOffset);
static_assert(kApplicationDataFileOffset+kApplicationPageDataBytes==kSourceFileSize);
[[nodiscard]] constexpr u32 code_offset_to_file_offset(u32 o) noexcept { return kApplicationDataFileOffset+o; }
static_assert(code_offset_to_file_offset(kEntryOffset)==0x39340u);
constexpr u32 kEntryFileOffset=0x39340u, kEntryJumpTarget=0xCC96u;
constexpr u32 kWatcomRuntimeTextFileOffset=0x39342u, kRavenloftTextFileOffset=0x3E7ACu;

[[nodiscard]] constexpr u16 load_le16(const u8* p) noexcept { return static_cast<u16>(p[0]|(static_cast<u16>(p[1])<<8u)); }
[[nodiscard]] constexpr u32 load_le32(const u8* p) noexcept { return static_cast<u32>(p[0]|(static_cast<u32>(p[1])<<8u)|(static_cast<u32>(p[2])<<16u)|(static_cast<u32>(p[3])<<24u)); }
constexpr void store_le16(u8* p,u16 v) noexcept { p[0]=static_cast<u8>(v); p[1]=static_cast<u8>(v>>8u); }
constexpr void store_le32(u8* p,u32 v) noexcept { p[0]=static_cast<u8>(v); p[1]=static_cast<u8>(v>>8u); p[2]=static_cast<u8>(v>>16u); p[3]=static_cast<u8>(v>>24u); }

// Explicit address-space types prevent DGROUP offsets from being confused with
// register-derived/relocated linear addresses or selector-relative addresses.
struct DgroupOffset { u32 value; };
struct LinearAddress { u32 value; };
struct SelectorOffset { u16 selector; u32 offset; };
[[nodiscard]] constexpr DgroupOffset dgroup(u32 value) noexcept { return {value}; }
[[nodiscard]] constexpr LinearAddress linear(u32 value) noexcept { return {value}; }
[[nodiscard]] constexpr SelectorOffset selector_offset(u16 selector,u32 offset) noexcept { return {selector,offset}; }

struct CpuState { u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0,esp=0; u16 ds=0,es=0,gs=0; bool interrupt_enable=false; };
struct Machine;
using ProtectedModeInterrupt=void (*)(Machine&,u8);
using LinearRead16=u16 (*)(Machine&,LinearAddress);
using LinearRead32=u32 (*)(Machine&,LinearAddress);
using SelectorRead16=u16 (*)(Machine&,SelectorOffset);
struct Machine { CpuState cpu{}; u8* data=nullptr; ProtectedModeInterrupt protected_interrupt=nullptr; LinearRead16 read_linear16=nullptr; LinearRead32 read_linear32=nullptr; SelectorRead16 read_selector16=nullptr; };
[[nodiscard]] u8* dgroup_ptr(Machine& m,DgroupOffset o) noexcept { return m.data+o.value; }
void interrupt(Machine& m,u8 vector) noexcept { if(m.protected_interrupt) m.protected_interrupt(m,vector); }
[[nodiscard]] u16 read16(Machine& m,LinearAddress a) noexcept { return m.read_linear16?m.read_linear16(m,a):0u; }
[[nodiscard]] u32 read32(Machine& m,LinearAddress a) noexcept { return m.read_linear32?m.read_linear32(m,a):0u; }
[[nodiscard]] u16 read16(Machine& m,SelectorOffset a) noexcept { return m.read_selector16?m.read_selector16(m,a):0u; }
[[nodiscard]] constexpr u8 low8(u32 v) noexcept { return static_cast<u8>(v); }
[[nodiscard]] constexpr u8 high8(u32 v) noexcept { return static_cast<u8>(v>>8u); }
[[nodiscard]] constexpr u16 low16(u32 v) noexcept { return static_cast<u16>(v); }
constexpr void set_low8(u32& v,u8 x) noexcept { v=(v&0xFFFFFF00u)|x; }
constexpr void set_low16(u32& v,u16 x) noexcept { v=(v&0xFFFF0000u)|x; }
constexpr void set_high8(u32& v,u8 x) noexcept { v=(v&0xFFFF00FFu)|(static_cast<u32>(x)<<8u); }

// CC1C..CCDD: entry jump plus first live startup block (74 bytes).
enum class StartupPath:u32 { FallThroughCcde=0xCCDEu, BranchCd17=0xCD17u };
[[nodiscard]] StartupPath translate_cc1c_ccdd(Machine& m) noexcept {
    m.cpu.interrupt_enable=true; m.cpu.esp&=0xFFFFFFFCu; m.cpu.ebx=m.cpu.esp;
    store_le32(dgroup_ptr(m,dgroup(0x5304u)),m.cpu.ebx); store_le32(dgroup_ptr(m,dgroup(0x52F0u)),m.cpu.ebx);
    set_low16(m.cpu.eax,0x24u); store_le16(dgroup_ptr(m,dgroup(0x52FCu)),low16(m.cpu.eax));
    m.cpu.ebx=0x50484152u; m.cpu.eax=0u; set_high8(m.cpu.eax,0x30u); interrupt(m,0x21u);
    *dgroup_ptr(m,dgroup(0x5326u))=low8(m.cpu.eax); *dgroup_ptr(m,dgroup(0x5327u))=high8(m.cpu.eax);
    m.cpu.ecx=m.cpu.eax; m.cpu.esi=0u; m.cpu.edi=0x81u; m.cpu.eax>>=16u;
    return low16(m.cpu.eax)!=0x4458u?StartupPath::BranchCd17:StartupPath::FallThroughCcde;
}

// CCDE..CD16: verified fall-through startup path (57 bytes).
[[nodiscard]] u32 translate_ccde_cd16(Machine& m,u32 es_005c) noexcept {
    const u8 bl=static_cast<u8>(low8(m.cpu.ebx)-0x30u); set_low8(m.cpu.ebx,bl); set_low8(m.cpu.eax,bl);
    const u32 pushed_eax=m.cpu.eax; m.cpu.es=load_le16(dgroup_ptr(m,dgroup(0x52FCu))); m.cpu.ebx=es_005c;
    m.cpu.ebx=(m.cpu.ebx+0x0FFFu)&0xFFFFF000u; store_le32(dgroup_ptr(m,dgroup(0x52F0u)),m.cpu.ebx); m.cpu.ebx>>=12u;
    set_low16(m.cpu.eax,m.cpu.ds); m.cpu.es=m.cpu.ds; set_high8(m.cpu.eax,0x4Au); interrupt(m,0x21u);
    m.cpu.eax=pushed_eax; set_low16(m.cpu.ebx,m.cpu.ds); set_low16(m.cpu.ecx,0x002Cu); return 0x0000CD4Eu;
}

// CD17..CD4F: alternate successor and equal-signature path (57 bytes).
// CD17 663D4342 cmp ax,4243h; CD1B 0F852F000000 jne CD50.
// Equal path: [3094]=EDX; ESI=EDX; EDX=[ESI+1C]; EBX=ESP-EDX;
// AH=4A/int21; EBX<-DS; [52FC]=DS; EAX=[ESI+10]; EDI+=EAX;
// ESI=0; SI=[EAX+2C]; ESI<<=4; ECX<-DS; AL=09; jmp CD97.
// Register-derived dereferences use the linear-address boundary, never DGROUP.
enum class Cd17Path:u32 { BranchCd50=0xCD50u, JumpCd97=0xCD97u };
[[nodiscard]] Cd17Path translate_cd17_cd4f(Machine& m) noexcept {
    if(low16(m.cpu.eax)!=0x4243u) return Cd17Path::BranchCd50;
    store_le32(dgroup_ptr(m,dgroup(0x3094u)),m.cpu.edx); m.cpu.esi=m.cpu.edx;
    m.cpu.edx=read32(m,linear(m.cpu.esi+0x1Cu)); m.cpu.ebx=m.cpu.esp; m.cpu.ebx-=m.cpu.edx;
    set_high8(m.cpu.eax,0x4Au); interrupt(m,0x21u);
    // 8C DB / 8C D9 target 32-bit GPRs. On the declared 80386 target the
    // upper half is not treated as a proven zero-extension; retain it.
    set_low16(m.cpu.ebx,m.cpu.ds); store_le16(dgroup_ptr(m,dgroup(0x52FCu)),m.cpu.ds);
    m.cpu.eax=read32(m,linear(m.cpu.esi+0x10u)); m.cpu.edi+=m.cpu.eax; m.cpu.esi=0u;
    set_low16(m.cpu.esi,read16(m,linear(m.cpu.eax+0x2Cu))); m.cpu.esi<<=4u;
    set_low16(m.cpu.ecx,m.cpu.ds); set_low8(m.cpu.eax,0x09u); return Cd17Path::JumpCd97;
}

// CD50..CD96: non-equal-signature path to the common CD97 successor (71 bytes).
// CD50 66BA7800 mov dx,0078h; CD54 66B800FF mov ax,FF00h; CD58 int 21h.
// AL!=0 path obtains GS, conditionally saves nonzero GS at [3098], sets AL=1,
// copies DS to BX, stores ES at [52FC], and loads CX from ES:[2C].
// AL==0 path saves DS in DX, temporarily loads DS=0024h, loads CX from DS:[2C],
// restores DS, sets BX=0017h and AL=0. Both paths arrive at CD97.
[[nodiscard]] u32 translate_cd50_cd96(Machine& m) noexcept {
    set_low16(m.cpu.edx,0x0078u);                      // CD50 mov dx,78h
    set_low16(m.cpu.eax,0xFF00u);                      // CD54 mov ax,FF00h
    interrupt(m,0x21u);                                // CD58 int 21h
    if(low8(m.cpu.eax)!=0u) {                          // CD5A cmp al,0 / CD5C je CD80
        set_low16(m.cpu.eax,m.cpu.gs);                 // CD5E mov eax,gs (selector word)
        if(low16(m.cpu.eax)!=0u)                       // CD60..CD64
            store_le16(dgroup_ptr(m,dgroup(0x3098u)),low16(m.cpu.eax)); // CD66
        set_low8(m.cpu.eax,0x01u);                     // CD6C mov al,1
        set_low16(m.cpu.ebx,m.cpu.ds);                 // CD6E mov ebx,ds
        store_le16(dgroup_ptr(m,dgroup(0x52FCu)),m.cpu.es); // CD70 mov [52FC],es
        set_low16(m.cpu.ecx,read16(m,selector_offset(m.cpu.es,0x2Cu))); // CD76
        return 0x0000CD97u;                            // CD7E jmp CD97
    }
    set_low16(m.cpu.edx,m.cpu.ds);                     // CD80 mov edx,ds
    set_low16(m.cpu.ecx,0x0024u);                      // CD82 mov cx,24h
    m.cpu.ds=low16(m.cpu.ecx);                         // CD86 mov ds,cx
    set_low16(m.cpu.ecx,read16(m,selector_offset(m.cpu.ds,0x2Cu))); // CD88
    m.cpu.ds=low16(m.cpu.edx);                         // CD8F mov ds,dx
    set_low16(m.cpu.ebx,0x0017u);                      // CD91 mov bx,17h
    set_low8(m.cpu.eax,0x00u);                         // CD95 mov al,0
    return 0x0000CD97u;
}

// An earlier checkpoint note incorrectly named CD4E as CD1B's branch target.
// Direct decoding of CODE.1 proves the displacement targets CD50; corrected
// before translation of the target block.
constexpr u32 kVerifiedTranslatedBytes=259u;
constexpr u32 kCoverageDenominator=kCodeObjectVirtualSize;
} // namespace code1
