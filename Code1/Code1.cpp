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

// Verified container/application layout.
constexpr u32 kOuterMzFileOffset=0x00000000u;
constexpr u32 kBoundBwFileOffset=0x0000F444u;
constexpr u32 kApplicationMzFileOffset=0x00025D24u;
constexpr u32 kApplicationLeFileOffset=0x00027BDCu;
constexpr u32 kApplicationDataFileOffset=0x0002C724u;
constexpr u16 kLeCpuType=0x0002u;
constexpr u16 kLeOsType=0x0001u;
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

// Address spaces are deliberately distinct. DGROUP offsets are not silently
// interchangeable with relocated/linear addresses recovered from registers.
struct DgroupOffset { u32 value; };
struct LinearAddress { u32 value; };
[[nodiscard]] constexpr DgroupOffset dgroup(u32 value) noexcept { return {value}; }
[[nodiscard]] constexpr LinearAddress linear(u32 value) noexcept { return {value}; }

struct CpuState {
    u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0,esp=0;
    u16 ds=0,es=0,gs=0; bool interrupt_enable=false;
};
struct Machine;
using ProtectedModeInterrupt = void (*)(Machine&,u8);
using LinearRead16 = u16 (*)(Machine&,LinearAddress);
using LinearRead32 = u32 (*)(Machine&,LinearAddress);
struct Machine {
    CpuState cpu{};
    u8* data=nullptr; // logical object-2/DGROUP byte zero
    ProtectedModeInterrupt protected_interrupt=nullptr;
    LinearRead16 read_linear16=nullptr;
    LinearRead32 read_linear32=nullptr;
};
[[nodiscard]] u8* dgroup_ptr(Machine& m,DgroupOffset o) noexcept { return m.data+o.value; }
[[nodiscard]] const u8* dgroup_ptr(const Machine& m,DgroupOffset o) noexcept { return m.data+o.value; }
void interrupt(Machine& m,u8 vector) noexcept { if(m.protected_interrupt) m.protected_interrupt(m,vector); }
[[nodiscard]] u16 read16(Machine& m,LinearAddress a) noexcept { return m.read_linear16?m.read_linear16(m,a):0u; }
[[nodiscard]] u32 read32(Machine& m,LinearAddress a) noexcept { return m.read_linear32?m.read_linear32(m,a):0u; }
[[nodiscard]] constexpr u8 low8(u32 v) noexcept { return static_cast<u8>(v); }
[[nodiscard]] constexpr u8 high8(u32 v) noexcept { return static_cast<u8>(v>>8u); }
[[nodiscard]] constexpr u16 low16(u32 v) noexcept { return static_cast<u16>(v); }
constexpr void set_low8(u32& v,u8 x) noexcept { v=(v&0xFFFFFF00u)|x; }
constexpr void set_low16(u32& v,u16 x) noexcept { v=(v&0xFFFF0000u)|x; }
constexpr void set_high8(u32& v,u8 x) noexcept { v=(v&0xFFFF00FFu)|(static_cast<u32>(x)<<8u); }

// --------------------------------------------------------------------------
// CC1C..CCDD: entry jump and first live startup block (74 bytes).
// --------------------------------------------------------------------------
enum class StartupPath:u32 { FallThroughCcde=0xCCDEu, BranchCd17=0xCD17u };
[[nodiscard]] StartupPath translate_cc1c_ccdd(Machine& m) noexcept {
    m.cpu.interrupt_enable=true;                       // CC96 sti
    m.cpu.esp&=0xFFFFFFFCu;                            // CC97 and esp,-4
    m.cpu.ebx=m.cpu.esp;                               // CC9A mov ebx,esp
    store_le32(dgroup_ptr(m,dgroup(0x5304u)),m.cpu.ebx); // CC9C
    store_le32(dgroup_ptr(m,dgroup(0x52F0u)),m.cpu.ebx); // CCA2
    set_low16(m.cpu.eax,0x24u);                        // CCA8 mov ax,24h
    store_le16(dgroup_ptr(m,dgroup(0x52FCu)),low16(m.cpu.eax)); // CCAC
    m.cpu.ebx=0x50484152u;                             // CCB2
    m.cpu.eax=0u; set_high8(m.cpu.eax,0x30u);          // CCB7..CCB9
    interrupt(m,0x21u);                                // CCBB int 21h
    *dgroup_ptr(m,dgroup(0x5326u))=low8(m.cpu.eax);    // CCBD
    *dgroup_ptr(m,dgroup(0x5327u))=high8(m.cpu.eax);   // CCC2
    m.cpu.ecx=m.cpu.eax; m.cpu.esi=0u; m.cpu.edi=0x81u; // CCC8..CCCC
    m.cpu.eax>>=16u;                                   // CCD1
    return low16(m.cpu.eax)!=0x4458u?StartupPath::BranchCd17:StartupPath::FallThroughCcde; // CCD4..CCD8
}

// --------------------------------------------------------------------------
// CCDE..CD16: verified fall-through startup path (57 bytes).
// --------------------------------------------------------------------------
[[nodiscard]] u32 translate_ccde_cd16(Machine& m,u32 es_005c) noexcept {
    const u8 bl=static_cast<u8>(low8(m.cpu.ebx)-0x30u); // CCDE sub bl,30h
    set_low8(m.cpu.ebx,bl); set_low8(m.cpu.eax,bl);    // CCE1 mov al,bl
    const u32 pushed_eax=m.cpu.eax;                    // CCE3 push eax
    m.cpu.es=load_le16(dgroup_ptr(m,dgroup(0x52FCu))); // CCE4 mov es,[52FC]
    m.cpu.ebx=es_005c;                                 // CCEA mov ebx,es:[5C]
    m.cpu.ebx=(m.cpu.ebx+0x0FFFu)&0xFFFFF000u;         // CCF1..CCF7
    store_le32(dgroup_ptr(m,dgroup(0x52F0u)),m.cpu.ebx); // CCFD
    m.cpu.ebx>>=12u;                                   // CD03
    set_low16(m.cpu.eax,m.cpu.ds); m.cpu.es=m.cpu.ds; // CD06..CD08
    set_high8(m.cpu.eax,0x4Au); interrupt(m,0x21u);    // CD0A..CD0C
    m.cpu.eax=pushed_eax;                              // CD0E pop eax
    set_low16(m.cpu.ebx,m.cpu.ds);                     // CD0F mov bx,ds
    set_low16(m.cpu.ecx,0x002Cu);                      // CD11 mov cx,2Ch
    return 0x0000CD4Eu;                                // CD15 jmp CD4E
}

// --------------------------------------------------------------------------
// CD17..CD4F: alternate startup successor and equal-signature path (57 bytes).
// --------------------------------------------------------------------------
// CD17 663D4342           cmp ax,4243h
// CD1B 0F852F000000       jne CD50
// CD21 891594300000       mov [3094],edx
// CD27 8BF2               mov esi,edx
// CD29 8B561C             mov edx,[esi+1Ch]
// CD2C 8BDC               mov ebx,esp
// CD2E 2BDA               sub ebx,edx
// CD30 B44A               mov ah,4Ah
// CD32 CD21               int 21h
// CD34 8CDB               mov ebx,ds
// CD36 8C1DFC520000       mov [52FC],ds
// CD3C 8B4610             mov eax,[esi+10h]
// CD3F 03F8               add edi,eax
// CD41 2BF6               sub esi,esi
// CD43 668B702C           mov si,[eax+2Ch]
// CD47 C1E604             shl esi,4
// CD4A 8CD9               mov ecx,ds
// CD4C B009               mov al,09h
// CD4E EB47               jmp CD97
//
// Register-derived [esi+...] and [eax+...] accesses are explicitly linear;
// they are not coerced into DGROUP offsets. This is the preventative address-
// space distinction established before further startup translation.
enum class Cd17Path:u32 { BranchCd50=0xCD50u, JumpCd97=0xCD97u };
[[nodiscard]] Cd17Path translate_cd17_cd4f(Machine& m) noexcept {
    if(low16(m.cpu.eax)!=0x4243u) return Cd17Path::BranchCd50; // CD17..CD1B
    store_le32(dgroup_ptr(m,dgroup(0x3094u)),m.cpu.edx);       // CD21
    m.cpu.esi=m.cpu.edx;                                      // CD27
    m.cpu.edx=read32(m,linear(m.cpu.esi+0x1Cu));               // CD29
    m.cpu.ebx=m.cpu.esp; m.cpu.ebx-=m.cpu.edx;                 // CD2C..CD2E
    set_high8(m.cpu.eax,0x4Au); interrupt(m,0x21u);            // CD30..CD32
    m.cpu.ebx=static_cast<u32>(m.cpu.ds);                      // CD34 mov ebx,ds
    store_le16(dgroup_ptr(m,dgroup(0x52FCu)),m.cpu.ds);        // CD36
    m.cpu.eax=read32(m,linear(m.cpu.esi+0x10u));               // CD3C
    m.cpu.edi+=m.cpu.eax;                                      // CD3F
    m.cpu.esi=0u;                                              // CD41
    set_low16(m.cpu.esi,read16(m,linear(m.cpu.eax+0x2Cu)));    // CD43
    m.cpu.esi<<=4u;                                            // CD47
    m.cpu.ecx=static_cast<u32>(m.cpu.ds);                      // CD4A mov ecx,ds
    set_low8(m.cpu.eax,0x09u);                                // CD4C
    return Cd17Path::JumpCd97;                                 // CD4E
}

// The earlier note that CD1B branches to CD4E was incorrect; direct decoding
// of the original bytes establishes the target as CD50. The correction is made
// here before any downstream block depends on that edge.
constexpr u32 kVerifiedTranslatedBytes=188u;
constexpr u32 kCoverageDenominator=kCodeObjectVirtualSize;
} // namespace code1
