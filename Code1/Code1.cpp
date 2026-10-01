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

struct CpuState {
    u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0,esp=0;
    u16 ds=0,es=0; bool interrupt_enable=false;
};
struct Machine { CpuState cpu{}; u8* data=nullptr; void (*int21)(Machine&)=nullptr; };
[[nodiscard]] constexpr u8 low8(u32 v) noexcept { return static_cast<u8>(v); }
[[nodiscard]] constexpr u8 high8(u32 v) noexcept { return static_cast<u8>(v>>8u); }
[[nodiscard]] constexpr u16 low16(u32 v) noexcept { return static_cast<u16>(v); }
constexpr void set_low8(u32& v,u8 x) noexcept { v=(v&0xFFFFFF00u)|x; }
constexpr void set_low16(u32& v,u16 x) noexcept { v=(v&0xFFFF0000u)|x; }
constexpr void set_high8(u32& v,u8 x) noexcept { v=(v&0xFFFF00FFu)|(static_cast<u32>(x)<<8u); }

// --------------------------------------------------------------------------
// CC1C..CCDD: entry jump and first live startup block (74 bytes).
// --------------------------------------------------------------------------
// CC1C EB78 jmp CC96. CC1E..CC95 is skipped runtime text/data.
// CC96 FB sti
// CC97 83E4FC and esp,-4
// CC9A 8BDC mov ebx,esp
// CC9C 891D04530000 mov [5304],ebx
// CCA2 891DF0520000 mov [52F0],ebx
// CCA8 66B82400 mov ax,24h
// CCAC 66A3FC520000 mov [52FC],ax
// CCB2 BB52414850 mov ebx,50484152h
// CCB7 2BC0 sub eax,eax
// CCB9 B430 mov ah,30h
// CCBB CD21 int 21h
// CCBD A226530000 mov [5326],al
// CCC2 882527530000 mov [5327],ah
// CCC8 8BC8 mov ecx,eax
// CCCA 2BF6 sub esi,esi
// CCCC BF81000000 mov edi,81h
// CCD1 C1E810 shr eax,10h
// CCD4 663D5844 cmp ax,4458h
// CCD8 0F8539000000 jne CD17
enum class StartupPath:u32 { FallThroughCcde=0xCCDEu, BranchCd17=0xCD17u };
[[nodiscard]] StartupPath translate_cc1c_ccdd(Machine& m) noexcept {
    m.cpu.interrupt_enable=true;
    m.cpu.esp&=0xFFFFFFFCu;
    m.cpu.ebx=m.cpu.esp;
    store_le32(m.data+0x5304u,m.cpu.ebx);
    store_le32(m.data+0x52F0u,m.cpu.ebx);
    set_low16(m.cpu.eax,0x24u);
    store_le16(m.data+0x52FCu,low16(m.cpu.eax));
    m.cpu.ebx=0x50484152u;
    m.cpu.eax=0u; set_high8(m.cpu.eax,0x30u);
    if(m.int21) m.int21(m);
    m.data[0x5326u]=low8(m.cpu.eax);
    m.data[0x5327u]=high8(m.cpu.eax);
    m.cpu.ecx=m.cpu.eax; m.cpu.esi=0u; m.cpu.edi=0x81u;
    m.cpu.eax>>=16u;
    return low16(m.cpu.eax)!=0x4458u?StartupPath::BranchCd17:StartupPath::FallThroughCcde;
}

// --------------------------------------------------------------------------
// CCDE..CD16: verified fall-through startup path (57 bytes).
// --------------------------------------------------------------------------
// CCDE 80EB30             sub bl,30h
// CCE1 8AC3               mov al,bl
// CCE3 50                 push eax
// CCE4 8E05FC520000       mov es,[52FCh]
// CCEA 268B1D5C000000     mov ebx,es:[5Ch]
// CCF1 81C3FF0F0000       add ebx,0FFFh
// CCF7 81E300F0FFFF       and ebx,0FFFFF000h
// CCFD 891DF0520000       mov [52F0h],ebx
// CD03 C1EB0C             shr ebx,0Ch
// CD06 8CD8               mov ax,ds
// CD08 8EC0               mov es,ax
// CD0A B44A               mov ah,4Ah
// CD0C CD21               int 21h
// CD0E 58                 pop eax
// CD0F 8CDB               mov bx,ds
// CD11 66B92C00           mov cx,002Ch
// CD15 EB37               jmp CD4Eh
//
// DOS/4GW documentation states that INT 21h is translated by the extender.
// AH=4Ah is therefore retained as an explicit DOS boundary rather than being
// replaced with guessed host allocation behavior.
[[nodiscard]] u32 translate_ccde_cd16(Machine& m,u32 es_005c) noexcept {
    const u8 bl=static_cast<u8>(low8(m.cpu.ebx)-0x30u); // sub bl,30h
    set_low8(m.cpu.ebx,bl);
    set_low8(m.cpu.eax,bl);                            // mov al,bl
    const u32 pushed_eax=m.cpu.eax;                    // push eax
    m.cpu.es=load_le16(m.data+0x52FCu);                // mov es,[52FC]
    m.cpu.ebx=es_005c;                                 // mov ebx,es:[5C]
    m.cpu.ebx=(m.cpu.ebx+0x0FFFu)&0xFFFFF000u;         // add/and
    store_le32(m.data+0x52F0u,m.cpu.ebx);              // mov [52F0],ebx
    m.cpu.ebx>>=12u;                                   // shr ebx,0Ch
    set_low16(m.cpu.eax,m.cpu.ds);                     // mov ax,ds
    m.cpu.es=m.cpu.ds;                                 // mov es,ax
    set_high8(m.cpu.eax,0x4Au);                        // mov ah,4Ah
    if(m.int21) m.int21(m);                            // int 21h
    m.cpu.eax=pushed_eax;                              // pop eax
    set_low16(m.cpu.ebx,m.cpu.ds);                     // mov bx,ds
    set_low16(m.cpu.ecx,0x002Cu);                      // mov cx,002Ch
    return 0x0000CD4Eu;                                // jmp CD4E
}

// Alternate successor begins at CD17:
//   66 3D 43 42             cmp ax,4243h
//   0F 85 2F 00 00 00       jne CD4Eh
// This path is the next unresolved block and is not counted yet.
constexpr u32 kVerifiedTranslatedBytes=131u;
constexpr u32 kCoverageDenominator=kCodeObjectVirtualSize;
} // namespace code1
