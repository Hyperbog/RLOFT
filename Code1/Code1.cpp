/* Ravenloft: Strahd's Possession - CODE.1 faithful reconstruction
 * Immutable source: CODE.1, 277787 bytes
 * SHA-256 fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Instruction-accountable reconstruction; unknown semantics are not guessed. */
#include <cstddef>
#include <cstdint>
#include <cstring>
namespace code1 {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t; using s16=std::int16_t; using s32=std::int32_t;
static_assert(sizeof(u8)==1&&sizeof(u16)==2&&sizeof(u32)==4);
constexpr std::size_t kSourceFileSize=277787u;
constexpr char kSourceSha256[]="fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed";
constexpr u32 kOuterMzFileOffset=0,kBoundBwFileOffset=0xF444u,kApplicationMzFileOffset=0x25D24u,kApplicationLeFileOffset=0x27BDCu,kApplicationDataFileOffset=0x2C724u;
constexpr u32 kEntryOffset=0xCC1Cu,kCodeObjectVirtualSize=0x11302u,kCodeObjectBase=0x10000u,kDataObjectVirtualSize=0x9E90u,kDataObjectBase=0x30000u;
constexpr u32 kEntryFileOffset=0x39340u,kWatcomRuntimeTextFileOffset=0x39342u,kRavenloftTextFileOffset=0x3E7ACu;
[[nodiscard]] constexpr u16 load_le16(const u8*p)noexcept{return static_cast<u16>(p[0]|(static_cast<u16>(p[1])<<8));}
[[nodiscard]] constexpr u32 load_le32(const u8*p)noexcept{return static_cast<u32>(p[0]|(static_cast<u32>(p[1])<<8)|(static_cast<u32>(p[2])<<16)|(static_cast<u32>(p[3])<<24));}
constexpr void store_le16(u8*p,u16 v)noexcept{p[0]=static_cast<u8>(v);p[1]=static_cast<u8>(v>>8);} constexpr void store_le32(u8*p,u32 v)noexcept{p[0]=static_cast<u8>(v);p[1]=static_cast<u8>(v>>8);p[2]=static_cast<u8>(v>>16);p[3]=static_cast<u8>(v>>24);}
struct DgroupOffset{u32 value;}; struct LinearAddress{u32 value;}; struct SelectorOffset{u16 selector;u32 offset;};
[[nodiscard]] constexpr DgroupOffset dgroup(u32 v)noexcept{return{v};} [[nodiscard]] constexpr LinearAddress linear(u32 v)noexcept{return{v};} [[nodiscard]] constexpr SelectorOffset selector_offset(u16 s,u32 o)noexcept{return{s,o};}
struct CpuState{u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0,esp=0;u16 ds=0,es=0,gs=0;bool interrupt_enable=false;};
struct Machine; using ProtectedModeInterrupt=void(*)(Machine&,u8); using LinearRead16=u16(*)(Machine&,LinearAddress); using LinearRead32=u32(*)(Machine&,LinearAddress); using SelectorRead16=u16(*)(Machine&,SelectorOffset);
struct Machine{CpuState cpu{};u8*data=nullptr;ProtectedModeInterrupt protected_interrupt=nullptr;LinearRead16 read_linear16=nullptr;LinearRead32 read_linear32=nullptr;SelectorRead16 read_selector16=nullptr;};
[[nodiscard]] u8*dgroup_ptr(Machine&m,DgroupOffset o)noexcept{return m.data+o.value;} void interrupt(Machine&m,u8 v)noexcept{if(m.protected_interrupt)m.protected_interrupt(m,v);} [[nodiscard]]u16 read16(Machine&m,LinearAddress a)noexcept{return m.read_linear16?m.read_linear16(m,a):0;} [[nodiscard]]u32 read32(Machine&m,LinearAddress a)noexcept{return m.read_linear32?m.read_linear32(m,a):0;} [[nodiscard]]u16 read16(Machine&m,SelectorOffset a)noexcept{return m.read_selector16?m.read_selector16(m,a):0;}
[[nodiscard]]constexpr u8 low8(u32 v)noexcept{return static_cast<u8>(v);} [[nodiscard]]constexpr u8 high8(u32 v)noexcept{return static_cast<u8>(v>>8);} [[nodiscard]]constexpr u16 low16(u32 v)noexcept{return static_cast<u16>(v);} constexpr void set_low8(u32&v,u8 x)noexcept{v=(v&0xffffff00u)|x;} constexpr void set_low16(u32&v,u16 x)noexcept{v=(v&0xffff0000u)|x;} constexpr void set_high8(u32&v,u8 x)noexcept{v=(v&0xffff00ffu)|(static_cast<u32>(x)<<8);}

// CC1C..CD96: startup recovery scaffolding. Kept low-level until the CRT-to-application handoff is proven.
enum class StartupPath:u32{FallThroughCcde=0xCCDEu,BranchCd17=0xCD17u};
[[nodiscard]] StartupPath translate_cc1c_ccdd(Machine&m)noexcept{m.cpu.interrupt_enable=true;m.cpu.esp&=0xfffffffcu;m.cpu.ebx=m.cpu.esp;store_le32(dgroup_ptr(m,dgroup(0x5304)),m.cpu.ebx);store_le32(dgroup_ptr(m,dgroup(0x52f0)),m.cpu.ebx);set_low16(m.cpu.eax,0x24);store_le16(dgroup_ptr(m,dgroup(0x52fc)),low16(m.cpu.eax));m.cpu.ebx=0x50484152;m.cpu.eax=0;set_high8(m.cpu.eax,0x30);interrupt(m,0x21);*dgroup_ptr(m,dgroup(0x5326))=low8(m.cpu.eax);*dgroup_ptr(m,dgroup(0x5327))=high8(m.cpu.eax);m.cpu.ecx=m.cpu.eax;m.cpu.esi=0;m.cpu.edi=0x81;m.cpu.eax>>=16;return low16(m.cpu.eax)!=0x4458?StartupPath::BranchCd17:StartupPath::FallThroughCcde;}
[[nodiscard]]u32 translate_ccde_cd16(Machine&m,u32 es005c)noexcept{const u8 bl=static_cast<u8>(low8(m.cpu.ebx)-0x30);set_low8(m.cpu.ebx,bl);set_low8(m.cpu.eax,bl);const u32 saved=m.cpu.eax;m.cpu.es=load_le16(dgroup_ptr(m,dgroup(0x52fc)));m.cpu.ebx=es005c;m.cpu.ebx=(m.cpu.ebx+0xfff)&0xfffff000;store_le32(dgroup_ptr(m,dgroup(0x52f0)),m.cpu.ebx);m.cpu.ebx>>=12;set_low16(m.cpu.eax,m.cpu.ds);m.cpu.es=m.cpu.ds;set_high8(m.cpu.eax,0x4a);interrupt(m,0x21);m.cpu.eax=saved;set_low16(m.cpu.ebx,m.cpu.ds);set_low16(m.cpu.ecx,0x2c);return 0xcd4e;}
enum class Cd17Path:u32{BranchCd50=0xcd50,JumpCd97=0xcd97};
[[nodiscard]]Cd17Path translate_cd17_cd4f(Machine&m)noexcept{if(low16(m.cpu.eax)!=0x4243)return Cd17Path::BranchCd50;store_le32(dgroup_ptr(m,dgroup(0x3094)),m.cpu.edx);m.cpu.esi=m.cpu.edx;m.cpu.edx=read32(m,linear(m.cpu.esi+0x1c));m.cpu.ebx=m.cpu.esp-m.cpu.edx;set_high8(m.cpu.eax,0x4a);interrupt(m,0x21);set_low16(m.cpu.ebx,m.cpu.ds);store_le16(dgroup_ptr(m,dgroup(0x52fc)),m.cpu.ds);m.cpu.eax=read32(m,linear(m.cpu.esi+0x10));m.cpu.edi+=m.cpu.eax;m.cpu.esi=0;set_low16(m.cpu.esi,read16(m,linear(m.cpu.eax+0x2c)));m.cpu.esi<<=4;set_low16(m.cpu.ecx,m.cpu.ds);set_low8(m.cpu.eax,9);return Cd17Path::JumpCd97;}
[[nodiscard]]u32 translate_cd50_cd96(Machine&m)noexcept{set_low16(m.cpu.edx,0x78);set_low16(m.cpu.eax,0xff00);interrupt(m,0x21);if(low8(m.cpu.eax)){set_low16(m.cpu.eax,m.cpu.gs);if(low16(m.cpu.eax))store_le16(dgroup_ptr(m,dgroup(0x3098)),low16(m.cpu.eax));set_low8(m.cpu.eax,1);set_low16(m.cpu.ebx,m.cpu.ds);store_le16(dgroup_ptr(m,dgroup(0x52fc)),m.cpu.es);set_low16(m.cpu.ecx,read16(m,selector_offset(m.cpu.es,0x2c)));return 0xcd97;}set_low16(m.cpu.edx,m.cpu.ds);set_low16(m.cpu.ecx,0x24);m.cpu.ds=low16(m.cpu.ecx);set_low16(m.cpu.ecx,read16(m,selector_offset(m.cpu.ds,0x2c)));m.cpu.ds=low16(m.cpu.edx);set_low16(m.cpu.ebx,0x17);set_low8(m.cpu.eax,0);return 0xcd97;}

// D08C..D153 and D1E8..D239: proven native lifts.
struct MouseRegs{u16 ax=0,bx=0,cx=0,dx=0;}; struct MouseSnapshot{u16 buttons;u16 reserved1;s16 x_half;u16 y;}; static_assert(sizeof(MouseSnapshot)==8);
struct NativeBoundaries{bool(*call_84d0)()=nullptr;void(*mouse_int33)(MouseRegs&)=nullptr;};
void function_d08c(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=4;r.dx=static_cast<u16>(a);r.cx=static_cast<u16>(c<<1);if(b.mouse_int33)b.mouse_int33(r);}
[[nodiscard]]MouseSnapshot*function_d0b4(NativeBoundaries&b,u8*d)noexcept{auto*out=reinterpret_cast<MouseSnapshot*>(d+0x8a60);if(!b.call_84d0||!b.call_84d0())return out;MouseRegs r{};r.ax=3;if(b.mouse_int33)b.mouse_int33(r);out->buttons=r.bx;out->y=r.dx;out->x_half=static_cast<s16>(static_cast<s16>(r.cx)>>1);return out;}
void function_d10c(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=7;r.cx=static_cast<u16>(a);r.dx=static_cast<u16>(c);if(b.mouse_int33)b.mouse_int33(r);} void function_d130(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=8;r.cx=static_cast<u16>(a);r.dx=static_cast<u16>(c);if(b.mouse_int33)b.mouse_int33(r);}
void*function_d1e8(void*dst,const void*src,u32 n)noexcept{return std::memmove(dst,src,static_cast<std::size_t>(n));}

// D5AD..D8CD: structured Watcom/DOS file-I/O lifts. The address-named runtime
// callbacks are intentionally not renamed until their exact CRT contracts are independently proven.
struct FileBoundaries{
 u32(*call_103c3)(u32)=nullptr; u32*(*call_103b7)()=nullptr; s32(*call_10314)(u16)=nullptr;
 u32(*call_1042d)()=nullptr; void(*call_1046f)()=nullptr;
 s32(*dos_read)(u16,void*,u32)=nullptr; s32(*dos_write)(u16,const void*,u32)=nullptr;
 s32(*dos_seek)(u16,s32,u8)=nullptr; s32(*dos_delete)(const char*)=nullptr;
};
static void set_runtime_error(FileBoundaries&b,u32 v)noexcept{if(b.call_103b7){if(u32*p=b.call_103b7())*p=v;}}
static s32 map_dos_error(FileBoundaries&b,s32 v)noexcept{return b.call_10314?b.call_10314(static_cast<u16>(v)):v;}

// D5AD..D685 (217 bytes): read. Bit 40h is binary/direct mode; otherwise CR
// bytes are discarded and 1Ah terminates text input, exactly as the loop encodes.
[[nodiscard]]s32 function_d5ad(FileBoundaries&b,u16 handle,u8*dst,u32 count)noexcept{
 const u32 flags=b.call_103c3?b.call_103c3(handle):0;
 if(flags==0){set_runtime_error(b,4);return -1;} if((flags&1u)==0){set_runtime_error(b,6);return -1;}
 if(flags&0x40u){const s32 n=b.dos_read?b.dos_read(handle,dst,count):-1;return n<0?map_dos_error(b,n):n;}
 u32 total=0,remaining=count;u8*writep=dst;
 while(remaining){const s32 n=b.dos_read?b.dos_read(handle,writep,remaining):-1;if(n<0)return map_dos_error(b,n);if(n==0)break;
  const u8*src=writep;const u8*end=writep+static_cast<u32>(n);u32 kept=0;
  while(src<end){const u8 ch=*src++;if(ch==0x1a){total+=kept;return static_cast<s32>(total);}if(ch==0x0d)continue;writep[kept++]=ch;}
  total+=kept;remaining-=kept;writep+=kept;if(flags&0x2000u)break;
 }
 return static_cast<s32>(total);
}

// D686..D85E (473 bytes): write. Binary mode writes directly. Text mode
// expands LF to CR/LF and flushes a 0x80/0x200-byte temporary buffer; short
// writes set runtime error 12 while preserving the original routine's return accounting.
[[nodiscard]]s32 function_d686(FileBoundaries&b,u16 handle,const u8*src,u32 count)noexcept{
 const u32 flags=b.call_103c3?b.call_103c3(handle):0;
 if(flags==0){set_runtime_error(b,4);return -1;} if((flags&2u)==0){set_runtime_error(b,6);return -1;}
 if(flags&0x80u){const s32 pos=b.dos_seek?b.dos_seek(handle,0,2):-1;if(pos<0)return map_dos_error(b,pos);}
 if(flags&0x40u){const s32 n=b.dos_write?b.dos_write(handle,src,count):-1;if(n<0)return map_dos_error(b,n);if(static_cast<u32>(n)!=count)set_runtime_error(b,12);return n;}
 const u32 version=b.call_1042d?b.call_1042d():0;if(version<0xb0u&&b.call_1046f)b.call_1046f();const u32 cap=version<0x230u?0x80u:0x200u;
 u8 temp[0x200]{};u32 consumed=0,last_flushed_input=0,fill=0;
 auto flush=[&](u32 amount)->s32{const s32 n=b.dos_write?b.dos_write(handle,temp,amount):-1;if(n<0)return map_dos_error(b,n);if(static_cast<u32>(n)!=amount){set_runtime_error(b,12);return static_cast<s32>(last_flushed_input+static_cast<u32>(n));}last_flushed_input=consumed;fill=0;return -0x40000000;};
 while(consumed<count){const u8 ch=src[consumed];if(ch==0x0a){temp[fill++]=0x0d;if(fill==cap){const s32 r=flush(cap);if(r!=-0x40000000)return r;}}
  temp[fill++]=ch;++consumed;if(fill==cap){const s32 r=flush(cap);if(r!=-0x40000000)return r;}}
 if(fill){const s32 n=b.dos_write?b.dos_write(handle,temp,fill):-1;if(n<0)return map_dos_error(b,n);if(static_cast<u32>(n)!=fill){set_runtime_error(b,12);return static_cast<s32>(last_flushed_input+static_cast<u32>(n));}}
 return static_cast<s32>(count);
}

// D85F..D8A9 (75 bytes): DOS seek (AH=42h). D8AA..D8CD (36 bytes): delete (AH=41h).
[[nodiscard]]s32 function_d85f(FileBoundaries&b,u16 handle,s32 offset,u8 origin)noexcept{const s32 r=b.dos_seek?b.dos_seek(handle,offset,origin):-1;return r<0?map_dos_error(b,r):r;}
[[nodiscard]]s32 function_d8aa(FileBoundaries&b,const char*path)noexcept{const s32 r=b.dos_delete?b.dos_delete(path):-1;return r<0?map_dos_error(b,r):0;}

// Newly counted in this batch: D5AD..D685=217, D686..D85E=473,
// D85F..D8A9=75, D8AA..D8CD=36: 801 additional verified bytes.
constexpr u32 kVerifiedTranslatedBytes=1334u;
constexpr u32 kCoverageDenominator=kCodeObjectVirtualSize;
} // namespace code1
