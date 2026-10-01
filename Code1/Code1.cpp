/* Ravenloft: Strahd's Possession - CODE.1 faithful reconstruction
 * Immutable source: CODE.1, 277787 bytes
 * SHA-256 fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Instruction-accountable reconstruction; unknown semantics are not guessed. */
#include <cstddef>
#include <cstdint>
#include <cstring>
namespace code1 {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t; using s16=std::int16_t;
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

struct DgroupOffset { u32 value; }; struct LinearAddress { u32 value; }; struct SelectorOffset { u16 selector; u32 offset; };
[[nodiscard]] constexpr DgroupOffset dgroup(u32 v) noexcept{return{v};} [[nodiscard]] constexpr LinearAddress linear(u32 v) noexcept{return{v};}
[[nodiscard]] constexpr SelectorOffset selector_offset(u16 s,u32 o) noexcept{return{s,o};}
struct CpuState { u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0,esp=0; u16 ds=0,es=0,gs=0; bool interrupt_enable=false; };
struct Machine; using ProtectedModeInterrupt=void(*)(Machine&,u8); using LinearRead16=u16(*)(Machine&,LinearAddress); using LinearRead32=u32(*)(Machine&,LinearAddress); using SelectorRead16=u16(*)(Machine&,SelectorOffset);
struct Machine { CpuState cpu{}; u8* data=nullptr; ProtectedModeInterrupt protected_interrupt=nullptr; LinearRead16 read_linear16=nullptr; LinearRead32 read_linear32=nullptr; SelectorRead16 read_selector16=nullptr; };
[[nodiscard]] u8* dgroup_ptr(Machine& m,DgroupOffset o) noexcept{return m.data+o.value;} void interrupt(Machine&m,u8 v)noexcept{if(m.protected_interrupt)m.protected_interrupt(m,v);}
[[nodiscard]] u16 read16(Machine&m,LinearAddress a)noexcept{return m.read_linear16?m.read_linear16(m,a):0u;} [[nodiscard]] u32 read32(Machine&m,LinearAddress a)noexcept{return m.read_linear32?m.read_linear32(m,a):0u;} [[nodiscard]] u16 read16(Machine&m,SelectorOffset a)noexcept{return m.read_selector16?m.read_selector16(m,a):0u;}
[[nodiscard]] constexpr u8 low8(u32 v)noexcept{return static_cast<u8>(v);} [[nodiscard]] constexpr u8 high8(u32 v)noexcept{return static_cast<u8>(v>>8u);} [[nodiscard]] constexpr u16 low16(u32 v)noexcept{return static_cast<u16>(v);}
constexpr void set_low8(u32&v,u8 x)noexcept{v=(v&0xFFFFFF00u)|x;} constexpr void set_low16(u32&v,u16 x)noexcept{v=(v&0xFFFF0000u)|x;} constexpr void set_high8(u32&v,u8 x)noexcept{v=(v&0xFFFF00FFu)|(static_cast<u32>(x)<<8u);}

enum class StartupPath:u32{FallThroughCcde=0xCCDEu,BranchCd17=0xCD17u};
[[nodiscard]] StartupPath translate_cc1c_ccdd(Machine&m)noexcept{m.cpu.interrupt_enable=true;m.cpu.esp&=0xFFFFFFFCu;m.cpu.ebx=m.cpu.esp;store_le32(dgroup_ptr(m,dgroup(0x5304u)),m.cpu.ebx);store_le32(dgroup_ptr(m,dgroup(0x52F0u)),m.cpu.ebx);set_low16(m.cpu.eax,0x24u);store_le16(dgroup_ptr(m,dgroup(0x52FCu)),low16(m.cpu.eax));m.cpu.ebx=0x50484152u;m.cpu.eax=0u;set_high8(m.cpu.eax,0x30u);interrupt(m,0x21u);*dgroup_ptr(m,dgroup(0x5326u))=low8(m.cpu.eax);*dgroup_ptr(m,dgroup(0x5327u))=high8(m.cpu.eax);m.cpu.ecx=m.cpu.eax;m.cpu.esi=0u;m.cpu.edi=0x81u;m.cpu.eax>>=16u;return low16(m.cpu.eax)!=0x4458u?StartupPath::BranchCd17:StartupPath::FallThroughCcde;}
[[nodiscard]] u32 translate_ccde_cd16(Machine&m,u32 es_005c)noexcept{const u8 bl=static_cast<u8>(low8(m.cpu.ebx)-0x30u);set_low8(m.cpu.ebx,bl);set_low8(m.cpu.eax,bl);const u32 saved=m.cpu.eax;m.cpu.es=load_le16(dgroup_ptr(m,dgroup(0x52FCu)));m.cpu.ebx=es_005c;m.cpu.ebx=(m.cpu.ebx+0xFFFu)&0xFFFFF000u;store_le32(dgroup_ptr(m,dgroup(0x52F0u)),m.cpu.ebx);m.cpu.ebx>>=12u;set_low16(m.cpu.eax,m.cpu.ds);m.cpu.es=m.cpu.ds;set_high8(m.cpu.eax,0x4Au);interrupt(m,0x21u);m.cpu.eax=saved;set_low16(m.cpu.ebx,m.cpu.ds);set_low16(m.cpu.ecx,0x2Cu);return 0xCD4Eu;}
enum class Cd17Path:u32{BranchCd50=0xCD50u,JumpCd97=0xCD97u};
[[nodiscard]] Cd17Path translate_cd17_cd4f(Machine&m)noexcept{if(low16(m.cpu.eax)!=0x4243u)return Cd17Path::BranchCd50;store_le32(dgroup_ptr(m,dgroup(0x3094u)),m.cpu.edx);m.cpu.esi=m.cpu.edx;m.cpu.edx=read32(m,linear(m.cpu.esi+0x1Cu));m.cpu.ebx=m.cpu.esp-m.cpu.edx;set_high8(m.cpu.eax,0x4Au);interrupt(m,0x21u);set_low16(m.cpu.ebx,m.cpu.ds);store_le16(dgroup_ptr(m,dgroup(0x52FCu)),m.cpu.ds);m.cpu.eax=read32(m,linear(m.cpu.esi+0x10u));m.cpu.edi+=m.cpu.eax;m.cpu.esi=0;set_low16(m.cpu.esi,read16(m,linear(m.cpu.eax+0x2Cu)));m.cpu.esi<<=4u;set_low16(m.cpu.ecx,m.cpu.ds);set_low8(m.cpu.eax,9u);return Cd17Path::JumpCd97;}
[[nodiscard]] u32 translate_cd50_cd96(Machine&m)noexcept{set_low16(m.cpu.edx,0x78u);set_low16(m.cpu.eax,0xFF00u);interrupt(m,0x21u);if(low8(m.cpu.eax)!=0u){set_low16(m.cpu.eax,m.cpu.gs);if(low16(m.cpu.eax)!=0u)store_le16(dgroup_ptr(m,dgroup(0x3098u)),low16(m.cpu.eax));set_low8(m.cpu.eax,1u);set_low16(m.cpu.ebx,m.cpu.ds);store_le16(dgroup_ptr(m,dgroup(0x52FCu)),m.cpu.es);set_low16(m.cpu.ecx,read16(m,selector_offset(m.cpu.es,0x2Cu)));return 0xCD97u;}set_low16(m.cpu.edx,m.cpu.ds);set_low16(m.cpu.ecx,0x24u);m.cpu.ds=low16(m.cpu.ecx);set_low16(m.cpu.ecx,read16(m,selector_offset(m.cpu.ds,0x2Cu)));m.cpu.ds=low16(m.cpu.edx);set_low16(m.cpu.ebx,0x17u);set_low8(m.cpu.eax,0u);return 0xCD97u;}

// --------------------------------------------------------------------------
// Structured lift: mouse-facing routines. These are ordinary C++ functions,
// not CPU-state blocks. Open Watcom's DOS/4GW guide explicitly documents
// protected-mode support for Microsoft-standard INT 33h mouse calls.
// The still-unidentified predicate at 84D0 remains an address-named callback.
// --------------------------------------------------------------------------
struct MouseRegs { u16 ax=0,bx=0,cx=0,dx=0; };
struct NativeBoundaries { bool (*call_84d0)()=nullptr; void (*mouse_int33)(MouseRegs&)=nullptr; };
struct MouseSnapshot { u16 buttons; u16 reserved1; s16 x_half; u16 y; };
static_assert(sizeof(MouseSnapshot)==8);

// D08C..D0B0 (37 bytes): if 84D0 permits it, INT33 AX=4 with
// DX=arg0 and CX=(arg1<<1). No guessed coordinate names are assigned yet.
void function_d08c(NativeBoundaries& b,u32 arg0,u32 arg1) noexcept {
    if(!b.call_84d0 || !b.call_84d0()) return;
    MouseRegs r{}; r.ax=4u; r.dx=static_cast<u16>(arg0); r.cx=static_cast<u16>(arg1<<1u);
    if(b.mouse_int33) b.mouse_int33(r);
}

// D0B4..D108 (85 bytes): INT33 AX=3. Original stores BX at 8A60,
// DX at 8A66, and arithmetic-shifted signed CX/2 at 8A64, then returns 8A60.
[[nodiscard]] MouseSnapshot* function_d0b4(NativeBoundaries& b,u8* dgroup_bytes) noexcept {
    auto* out=reinterpret_cast<MouseSnapshot*>(dgroup_bytes+0x8A60u);
    if(!b.call_84d0 || !b.call_84d0()) return out;
    MouseRegs r{}; r.ax=3u; if(b.mouse_int33) b.mouse_int33(r);
    out->buttons=r.bx; out->y=r.dx; out->x_half=static_cast<s16>(static_cast<s16>(r.cx)>>1);
    return out;
}

// D10C..D12E (35 bytes): gated INT33 AX=7; ECX/EDX receive the two arguments.
void function_d10c(NativeBoundaries& b,u32 arg0,u32 arg1) noexcept {
    if(!b.call_84d0 || !b.call_84d0()) return;
    MouseRegs r{}; r.ax=7u; r.cx=static_cast<u16>(arg0); r.dx=static_cast<u16>(arg1);
    if(b.mouse_int33) b.mouse_int33(r);
}

// D130..D152 (35 bytes): gated INT33 AX=8; same argument placement as D10C.
void function_d130(NativeBoundaries& b,u32 arg0,u32 arg1) noexcept {
    if(!b.call_84d0 || !b.call_84d0()) return;
    MouseRegs r{}; r.ax=8u; r.cx=static_cast<u16>(arg0); r.dx=static_cast<u16>(arg1);
    if(b.mouse_int33) b.mouse_int33(r);
}

// D1E8..D239 (82 bytes): overlap-safe byte copy. The original chooses a
// backward copy only when source < destination < source+count; otherwise it
// copies forward. This is exactly the semantic contract of memmove.
void* function_d1e8(void* destination,const void* source,u32 count) noexcept {
    return std::memmove(destination,source,static_cast<std::size_t>(count));
}

// D154 is intentionally the current stop point for this batch. It calls 84D0,
// D0B4 and 8960 in a loop. 84D0/8960 semantics have not yet been established;
// assigning names to them now would compromise the evidence-first rule.
constexpr u32 kVerifiedTranslatedBytes=533u;
constexpr u32 kCoverageDenominator=kCodeObjectVirtualSize;
} // namespace code1
