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
struct CpuState{u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0,esp=0;u16 cs=0,ds=0,es=0,ss=0,fs=0,gs=0;bool interrupt_enable=false;};
struct Machine; using ProtectedModeInterrupt=void(*)(Machine&,u8); using LinearRead16=u16(*)(Machine&,LinearAddress); using LinearRead32=u32(*)(Machine&,LinearAddress); using SelectorRead16=u16(*)(Machine&,SelectorOffset);
struct Machine{CpuState cpu{};u8*data=nullptr;ProtectedModeInterrupt protected_interrupt=nullptr;LinearRead16 read_linear16=nullptr;LinearRead32 read_linear32=nullptr;SelectorRead16 read_selector16=nullptr;};
[[nodiscard]] u8*dgroup_ptr(Machine&m,DgroupOffset o)noexcept{return m.data+o.value;} void interrupt(Machine&m,u8 v)noexcept{if(m.protected_interrupt)m.protected_interrupt(m,v);} [[nodiscard]]u16 read16(Machine&m,LinearAddress a)noexcept{return m.read_linear16?m.read_linear16(m,a):0;} [[nodiscard]]u32 read32(Machine&m,LinearAddress a)noexcept{return m.read_linear32?m.read_linear32(m,a):0;} [[nodiscard]]u16 read16(Machine&m,SelectorOffset a)noexcept{return m.read_selector16?m.read_selector16(m,a):0;}
[[nodiscard]]constexpr u8 low8(u32 v)noexcept{return static_cast<u8>(v);} [[nodiscard]]constexpr u8 high8(u32 v)noexcept{return static_cast<u8>(v>>8);} [[nodiscard]]constexpr u16 low16(u32 v)noexcept{return static_cast<u16>(v);} constexpr void set_low8(u32&v,u8 x)noexcept{v=(v&0xffffff00u)|x;} constexpr void set_low16(u32&v,u16 x)noexcept{v=(v&0xffff0000u)|x;} constexpr void set_high8(u32&v,u8 x)noexcept{v=(v&0xffff00ffu)|(static_cast<u32>(x)<<8);}

// CC1C..CD96: startup recovery scaffolding.
enum class StartupPath:u32{FallThroughCcde=0xCCDEu,BranchCd17=0xCD17u};
[[nodiscard]] StartupPath translate_cc1c_ccdd(Machine&m)noexcept{m.cpu.interrupt_enable=true;m.cpu.esp&=0xfffffffcu;m.cpu.ebx=m.cpu.esp;store_le32(dgroup_ptr(m,dgroup(0x5304)),m.cpu.ebx);store_le32(dgroup_ptr(m,dgroup(0x52f0)),m.cpu.ebx);set_low16(m.cpu.eax,0x24);store_le16(dgroup_ptr(m,dgroup(0x52fc)),low16(m.cpu.eax));m.cpu.ebx=0x50484152;m.cpu.eax=0;set_high8(m.cpu.eax,0x30);interrupt(m,0x21);*dgroup_ptr(m,dgroup(0x5326))=low8(m.cpu.eax);*dgroup_ptr(m,dgroup(0x5327))=high8(m.cpu.eax);m.cpu.ecx=m.cpu.eax;m.cpu.esi=0;m.cpu.edi=0x81;m.cpu.eax>>=16;return low16(m.cpu.eax)!=0x4458?StartupPath::BranchCd17:StartupPath::FallThroughCcde;}
[[nodiscard]]u32 translate_ccde_cd16(Machine&m,u32 es005c)noexcept{const u8 bl=static_cast<u8>(low8(m.cpu.ebx)-0x30);set_low8(m.cpu.ebx,bl);set_low8(m.cpu.eax,bl);const u32 saved=m.cpu.eax;m.cpu.es=load_le16(dgroup_ptr(m,dgroup(0x52fc)));m.cpu.ebx=es005c;m.cpu.ebx=(m.cpu.ebx+0xfff)&0xfffff000;store_le32(dgroup_ptr(m,dgroup(0x52f0)),m.cpu.ebx);m.cpu.ebx>>=12;set_low16(m.cpu.eax,m.cpu.ds);m.cpu.es=m.cpu.ds;set_high8(m.cpu.eax,0x4a);interrupt(m,0x21);m.cpu.eax=saved;set_low16(m.cpu.ebx,m.cpu.ds);set_low16(m.cpu.ecx,0x2c);return 0xcd4e;}
enum class Cd17Path:u32{BranchCd50=0xcd50,JumpCd97=0xcd97};
[[nodiscard]]Cd17Path translate_cd17_cd4f(Machine&m)noexcept{if(low16(m.cpu.eax)!=0x4243)return Cd17Path::BranchCd50;store_le32(dgroup_ptr(m,dgroup(0x3094)),m.cpu.edx);m.cpu.esi=m.cpu.edx;m.cpu.edx=read32(m,linear(m.cpu.esi+0x1c));m.cpu.ebx=m.cpu.esp-m.cpu.edx;set_high8(m.cpu.eax,0x4a);interrupt(m,0x21);set_low16(m.cpu.ebx,m.cpu.ds);store_le16(dgroup_ptr(m,dgroup(0x52fc)),m.cpu.ds);m.cpu.eax=read32(m,linear(m.cpu.esi+0x10));m.cpu.edi+=m.cpu.eax;m.cpu.esi=0;set_low16(m.cpu.esi,read16(m,linear(m.cpu.eax+0x2c)));m.cpu.esi<<=4;set_low16(m.cpu.ecx,m.cpu.ds);set_low8(m.cpu.eax,9);return Cd17Path::JumpCd97;}
[[nodiscard]]u32 translate_cd50_cd96(Machine&m)noexcept{set_low16(m.cpu.edx,0x78);set_low16(m.cpu.eax,0xff00);interrupt(m,0x21);if(low8(m.cpu.eax)){set_low16(m.cpu.eax,m.cpu.gs);if(low16(m.cpu.eax))store_le16(dgroup_ptr(m,dgroup(0x3098)),low16(m.cpu.eax));set_low8(m.cpu.eax,1);set_low16(m.cpu.ebx,m.cpu.ds);store_le16(dgroup_ptr(m,dgroup(0x52fc)),m.cpu.es);set_low16(m.cpu.ecx,read16(m,selector_offset(m.cpu.es,0x2c)));return 0xcd97;}set_low16(m.cpu.edx,m.cpu.ds);set_low16(m.cpu.ecx,0x24);m.cpu.ds=low16(m.cpu.ecx);set_low16(m.cpu.ecx,read16(m,selector_offset(m.cpu.ds,0x2c)));m.cpu.ds=low16(m.cpu.edx);set_low16(m.cpu.ebx,0x17);set_low8(m.cpu.eax,0);return 0xcd97;}

struct MouseRegs{u16 ax=0,bx=0,cx=0,dx=0;}; struct MouseSnapshot{u16 buttons;u16 reserved1;s16 x_half;u16 y;}; static_assert(sizeof(MouseSnapshot)==8);
struct NativeBoundaries{bool(*call_84d0)()=nullptr;void(*mouse_int33)(MouseRegs&)=nullptr;void(*call_8960)()=nullptr;u32(*call_82a4)()=nullptr;void(*call_82ec)()=nullptr;void(*call_8304)()=nullptr;void(*call_82c0)()=nullptr;};
void function_d08c(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=4;r.dx=static_cast<u16>(a);r.cx=static_cast<u16>(c<<1);if(b.mouse_int33)b.mouse_int33(r);}
[[nodiscard]]MouseSnapshot*function_d0b4(NativeBoundaries&b,u8*d)noexcept{auto*out=reinterpret_cast<MouseSnapshot*>(d+0x8a60);if(!b.call_84d0||!b.call_84d0())return out;MouseRegs r{};r.ax=3;if(b.mouse_int33)b.mouse_int33(r);out->buttons=r.bx;out->y=r.dx;out->x_half=static_cast<s16>(static_cast<s16>(r.cx)>>1);return out;}
void function_d10c(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=7;r.cx=static_cast<u16>(a);r.dx=static_cast<u16>(c);if(b.mouse_int33)b.mouse_int33(r);} void function_d130(NativeBoundaries&b,u32 a,u32 c)noexcept{if(!b.call_84d0||!b.call_84d0())return;MouseRegs r{};r.ax=8;r.cx=static_cast<u16>(a);r.dx=static_cast<u16>(c);if(b.mouse_int33)b.mouse_int33(r);}
// D154..D178: drain mouse snapshots while the first returned word remains nonzero.
void function_d154(Machine&m,NativeBoundaries&b)noexcept{if(!b.call_84d0||!b.call_84d0())return;for(;;){MouseSnapshot*p=function_d0b4(b,m.data);if(b.call_8960)b.call_8960();if(p->buttons==0)return;}}
// D179..D17B is LEA EAX,[EAX+0] alignment padding and is intentionally not translated.
// D17C..D1E7: exact repeated 82A4/D0B4/8960 polling sequence; D1B6 calls 82A4 again.
void function_d17c(Machine&m,NativeBoundaries&b)noexcept{if(b.call_84d0&&b.call_84d0()){for(;;){MouseSnapshot*p=function_d0b4(b,m.data);if(b.call_8960)b.call_8960();if(p->buttons==0)break;}for(;;){const u32 ready=b.call_82a4?b.call_82a4():0;if(ready!=0)break;MouseSnapshot*p=function_d0b4(b,m.data);if(p->buttons!=0)break;if(b.call_8960)b.call_8960();}const u32 ready=b.call_82a4?b.call_82a4():0;if(ready!=0){if(b.call_82ec)b.call_82ec();if(b.call_8304)b.call_8304();}else{for(;;){if(b.call_8960)b.call_8960();MouseSnapshot*p=function_d0b4(b,m.data);if(p->buttons==0)break;}}}else if(b.call_82c0)b.call_82c0();}
void*function_d1e8(void*dst,const void*src,u32 n)noexcept{return std::memmove(dst,src,static_cast<std::size_t>(n));}

struct DosAttrResult{u16 ax=0;u16 cx=0;bool carry=false;};
struct DosHandleResult{u16 ax=0;bool carry=false;};
struct FileBoundaries{DosAttrResult(*dos_get_attr)(u32)=nullptr;u32(*call_103c3)(u32)=nullptr;u32(*call_10397)(u32)=nullptr;u32*(*call_103b7)()=nullptr;s32(*call_10314)(u16)=nullptr;u32(*call_1042d)()=nullptr;void(*call_1046f)()=nullptr;s32(*dos_read)(u16,void*,u32)=nullptr;s32(*dos_write)(u16,const void*,u32)=nullptr;s32(*dos_seek)(u16,s32,u8)=nullptr;s32(*dos_delete)(const char*)=nullptr;bool(*dos_close)(u16)=nullptr;DosHandleResult(*dos_open_raw)(u32,u8)=nullptr;DosHandleResult(*dos_create_raw)(u32,u16)=nullptr;DosHandleResult(*dos_close_raw)(u16)=nullptr;DosHandleResult(*dos_write_zero_raw)(u16)=nullptr;u8(*read_guest8)(u32)=nullptr;u32(*read_dgroup32)(u32)=nullptr;void(*set_fd_state_10415)(u32,u32)=nullptr;bool(*ioctl_get_mode)(u16,u8&)=nullptr;bool(*ioctl_set_mode)(u16,u8)=nullptr;u32(*open_core_d332)(u32,u32,u32,u32)=nullptr;DosAttrResult(*dos_find_first)(u32,u16,u32)=nullptr;DosAttrResult(*dos_find_next)(u32)=nullptr;void(*dta_pre_find_next)(u32)=nullptr;void(*dta_post_find)(u32)=nullptr;s32(*compare_guest_string)(const char*,u32)=nullptr;};


// 102D4..104DD: directly disassembled CRT error/descriptor/stack/selector helpers.
// 102D4 and 102EC conditionally forward the encoded error word; the second argument is only a gate.
[[nodiscard]]s32 function_102ec(FileBoundaries&b,u32 error,u32 gate)noexcept{return gate?(b.call_10314?b.call_10314(static_cast<u16>(error)):-1):static_cast<s32>(error);}
[[nodiscard]]s32 function_102d4(FileBoundaries&b,u32 error,u32 gate)noexcept{return gate?function_102ec(b,error,gate):static_cast<s32>(error);}
static_assert(0x10303u-0x102D4u==47u);
// 10303..10313: set runtime errno 9 and return -1.
[[nodiscard]]s32 function_10303(FileBoundaries&b)noexcept{if(b.call_103b7)if(u32*p=b.call_103b7())*p=9u;return -1;}
static_assert(0x10314u-0x10303u==17u);
// 10314..10383: DOS/runtime error mapper. 8-bit errors use the encoded 20-entry signed table at 532C after DOS-version remapping; >=100h errors use bits 8..15 directly. The original error byte is also stored at 8E6C.
[[nodiscard]]s32 function_10314(Machine&m,u32 error)noexcept{
 const u8 original=static_cast<u8>(error);store_le32(dgroup_ptr(m,dgroup(0x8e6cu)),static_cast<u32>(original));u32 mapped=0;
 if(error<0x100u){u8 e=original;if(*dgroup_ptr(m,dgroup(0x5326u))>=3u){if(e==0x50u)e=0x0eu;else if(e>=0x22u)e=0x13u;else if(e>=0x20u)e=5u;}if(e>0x13u)e=0x13u;mapped=static_cast<u32>(static_cast<s32>(static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(0x532cu+e)))));}
 else mapped=(error>>8)&0xffu;store_le32(dgroup_ptr(m,dgroup(0x8e68u)),mapped);return -1;
}
static_assert(0x10384u-0x10314u==112u);
// 10384..10396: carry-sensitive DOS result reducer: success returns zero, failure maps low AX.
[[nodiscard]]s32 function_10384(Machine&m,bool carry,u32 eax)noexcept{return carry?function_10314(m,eax&0xffffu):0;}
static_assert(0x10397u-0x10384u==19u);
struct RuntimeIoctlBoundary{struct Result{u32 edx=0;bool carry=false;};Result(*ioctl_get)(u16)=nullptr;};
// 10397..103B6: INT 21h AX=4400h device query; the RCL/ROR pair materializes CF into EDX bit 31, and SETNE tests DL bit 7 exactly as encoded.
[[nodiscard]]u32 function_10397(RuntimeIoctlBoundary&b,u16 handle)noexcept{const auto r=b.ioctl_get?b.ioctl_get(handle):RuntimeIoctlBoundary::Result{};u32 edx=r.edx;edx=(edx<<1)|(r.carry?1u:0u);edx=(edx>>1)|(edx<<31);return (static_cast<u8>(edx)&0x80u)?1u:0u;}
static_assert(0x103B7u-0x10397u==32u);
[[nodiscard]]constexpr u32 function_103b7()noexcept{return 0x8e68u;}[[nodiscard]]constexpr u32 function_103bd()noexcept{return 0x8e6cu;}
static_assert(0x103C3u-0x103B7u==12u);
// 103C3..10414: fetch descriptor state from table 5398, lazily tagging handles 0..5 with 40h and 20h when the encoded device query succeeds.
[[nodiscard]]u32 function_103c3(Machine&m,RuntimeIoctlBoundary&b,u32 h)noexcept{const u32 n=load_le32(dgroup_ptr(m,dgroup(0x5344u)));if(h>=n)return 0u;const u32 table=load_le32(dgroup_ptr(m,dgroup(0x5398u)));if(h<=5u){u8*entry=dgroup_ptr(m,dgroup(table+h*4u));if((entry[1]&0x40u)==0){entry[1]|=0x40u;if(function_10397(b,static_cast<u16>(h)))entry[1]|=0x20u;}}return load_le32(dgroup_ptr(m,dgroup(table+h*4u)));}
static_assert(0x10415u-0x103C3u==82u);
// 10415..1042C: store descriptor state after forcing DH bit 6.
void function_10415(Machine&m,u32 h,u32 state)noexcept{const u32 table=load_le32(dgroup_ptr(m,dgroup(0x5398u)));state|=0x00400000u;store_le32(dgroup_ptr(m,dgroup(table+h*4u)),state);}
static_assert(0x1042Du-0x10415u==24u);
// 1042D..10435: return ESP minus the runtime stack-base global at 5300.
[[nodiscard]]u32 function_1042d(Machine&m)noexcept{return m.cpu.esp-load_le32(dgroup_ptr(m,dgroup(0x5300u)));}
static_assert(0x10436u-0x1042Du==9u);
// 10436..10437 are two zero bytes and are data/alignment, not translated code.
// 10438..1043E: capture SS at 55B0.
void function_10438(Machine&m)noexcept{store_le16(dgroup_ptr(m,dgroup(0x55b0u)),m.cpu.ss);}
static_assert(0x1043Fu-0x10438u==7u);
struct StackGuardBoundary{void(*fatal_ce8e)(u32,u32)=nullptr;};
// 1043F..1044E exchanges EAX with the caller stack argument, calls 10452, restores EAX from that slot, then RET 4. 1044F is the encoded RET 4 alternate entry.
void function_10452(Machine&m,StackGuardBoundary&b,u32 probe)noexcept{bool ok=false;if(probe<m.cpu.esp){const u32 distance=m.cpu.esp-probe;if(distance<=load_le32(dgroup_ptr(m,dgroup(0x5300u))))ok=true;}if(!ok&&m.cpu.ss==load_le16(dgroup_ptr(m,dgroup(0x55b0u)))){if(b.fatal_ce8e)b.fatal_ce8e(0x55b2u,1u);}}
void function_1043f(Machine&m,StackGuardBoundary&b,u32 argument)noexcept{const u32 incoming_eax=m.cpu.eax;m.cpu.eax=argument;function_10452(m,b,incoming_eax);m.cpu.eax=incoming_eax;}
void function_1044f()noexcept{}
static_assert(0x10452u-0x1043Fu==19u);static_assert(0x1047Eu-0x10452u==44u);
// 1047E..104B0: bounded copy followed by zero padding; returns the original destination.
[[nodiscard]]u8*function_1047e(u8*dst,const u8*src,u32 count)noexcept{u8*out=dst;while(count&&*src){--count;*dst++=*src++;}while(count){--count;*dst++=0;}return out;}
static_assert(0x104B1u-0x1047Eu==51u);
// 104B1..104DD: capture ES,CS,SS,DS,FS,GS in the exact encoded field order.
void function_104b1(Machine&m,u8*out)noexcept{store_le16(out+0,m.cpu.es);store_le16(out+2,m.cpu.cs);store_le16(out+4,m.cpu.ss);store_le16(out+6,m.cpu.ds);store_le16(out+8,m.cpu.fs);store_le16(out+10,m.cpu.gs);}
static_assert(0x104DEu-0x104B1u==45u);
// 104DE..104DF are two zero bytes and are data/alignment, not translated code.
// D23A..D27C: INT 21h/AH=43h attribute query. Caller flag bit 1 plus returned CX bit 0 synthesizes DOS error 6; 102D4/102EC/10314 reduce CF to 0 on success or mapped -1 on failure.
[[nodiscard]]s32 function_d23a(FileBoundaries&b,u32 path,u32 flags)noexcept{DosAttrResult r=b.dos_get_attr?b.dos_get_attr(path):DosAttrResult{1,0,true};if(!r.carry&&(flags&2u)&&(r.cx&1u)){r.ax=0x0600u;r.carry=true;}if(!r.carry)return 0;return b.call_10314?b.call_10314(r.ax):-1;}
static_assert(0xD27Du-0xD23Au==67u);
static void set_runtime_error(FileBoundaries&b,u32 v)noexcept{if(b.call_103b7){if(u32*p=b.call_103b7())*p=v;}} static s32 map_dos_error(FileBoundaries&b,s32 v)noexcept{return b.call_10314?b.call_10314(static_cast<u16>(v)):v;}
[[nodiscard]]u32 function_d332(FileBoundaries&,u32,u32,u32,u32)noexcept;
// D280..D2AC: set DOS DTA, execute AH=4Eh find-first, map AX+CF through 10384 semantics, then enter the exact D2CD mode-9 post-find shuttle boundary.
[[nodiscard]]s32 function_d280(FileBoundaries&b,u32 path,u16 attr,u32 dta)noexcept{DosAttrResult r=b.dos_find_first?b.dos_find_first(path,attr,dta):DosAttrResult{1,0,true};const s32 out=r.carry?map_dos_error(b,r.ax):0;if(b.dta_post_find)b.dta_post_find(dta);return out;}
static_assert(0xD2ADu-0xD280u==45u);
// D2AD..D2CC: set DTA, execute the D2F0 mode-9 pre-find shuttle, AH=4Fh find-next, 10384 mapping, then D2CD post-find shuttle. D2CD/D2F0 remain machine/selector boundaries.
[[nodiscard]]s32 function_d2ad(FileBoundaries&b,u32 dta)noexcept{if(b.dta_pre_find_next)b.dta_pre_find_next(dta);DosAttrResult r=b.dos_find_next?b.dos_find_next(dta):DosAttrResult{1,0,true};const s32 out=r.carry?map_dos_error(b,r.ax):0;if(b.dta_post_find)b.dta_post_find(dta);return out;}
static_assert(0xD2CDu-0xD2ADu==32u);
// D313..D331: variadic front wrapper. The first variadic dword is forwarded as D332 arg4; arg3 is exactly zero.
[[nodiscard]]u32 function_d313(FileBoundaries&b,u32 path,u32 flags,u32 first_variadic)noexcept{return function_d332(b,path,flags,0u,first_variadic);}
static_assert(0xD332u-0xD313u==31u);
// D332..D4F1: DOS open/create/truncate state machine. All DOS calls retain AX+CF explicitly; no host fopen semantics are substituted.
[[nodiscard]]u32 function_d332(FileBoundaries&b,u32 path,u32 flags,u32 share,u32 create_mode)noexcept{
 while(b.read_guest8&&b.read_guest8(path)==0x20u)++path;
 u32 mode=flags&0x83u; const u8 open_mode=static_cast<u8>(mode|share);
 DosHandleResult opened=b.dos_open_raw?b.dos_open_raw(path,open_mode):DosHandleResult{0xffffu,true};
 u32 handle=opened.carry?0xffffffffu:static_cast<u32>(opened.ax); u16 error_ax=opened.ax;
 if((flags&3u)!=0&&handle!=0xffffffffu&&(b.call_10397?b.call_10397(handle):0u)==0u){
  if((flags&0x400u)!=0u){if(b.dos_close_raw)(void)b.dos_close_raw(static_cast<u16>(handle));set_runtime_error(b,7u);return 0xffffffffu;}
  if((flags&0x40u)!=0u){const DosHandleResult wr=b.dos_write_zero_raw?b.dos_write_zero_raw(static_cast<u16>(handle)):DosHandleResult{0xffffu,true};if(wr.carry){if(b.dos_close_raw)(void)b.dos_close_raw(static_cast<u16>(handle));return static_cast<u32>(map_dos_error(b,static_cast<s32>(wr.ax)));}}
 }
 if(handle==0xffffffffu){
  if((flags&0x20u)==0u||error_ax!=2u)return static_cast<u32>(map_dos_error(b,static_cast<s32>(error_ax)));
  u32 cm=create_mode?create_mode:0x180u; const u32 mask=b.read_dgroup32?b.read_dgroup32(0x5340u):0u; cm^=mask; const u16 attr=(static_cast<u8>(cm)&0x80u)?0u:1u;
  const DosHandleResult cr=b.dos_create_raw?b.dos_create_raw(path,attr):DosHandleResult{0xffffu,true};if(cr.carry)return static_cast<u32>(map_dos_error(b,static_cast<s32>(cr.ax)));
  const DosHandleResult cl=b.dos_close_raw?b.dos_close_raw(cr.ax):DosHandleResult{0xffffu,true};if(cl.carry)return static_cast<u32>(map_dos_error(b,static_cast<s32>(cl.ax)));
  opened=b.dos_open_raw?b.dos_open_raw(path,open_mode):DosHandleResult{0xffffu,true};if(opened.carry)return static_cast<u32>(map_dos_error(b,static_cast<s32>(opened.ax)));handle=static_cast<u32>(opened.ax);
 }
 u32 state=b.call_103c3?b.call_103c3(handle):0u; state=(state&~0xffu)|static_cast<u8>(state&0x3cu);
 if((b.call_10397?b.call_10397(handle):0u)!=0u)state|=0x2000u; mode&=0x7fu;
 if(mode==2u)state|=3u;if(mode==0u)state|=1u;if(mode==1u)state|=2u;if((flags&0x10u)!=0u)state|=0x80u;
 const u32 text_state=(state&~0xffu)|static_cast<u8>(state|0x40u);
 if((flags&0x300u)!=0u){if((flags&0x200u)!=0u)state=text_state;}else if((b.read_dgroup32?b.read_dgroup32(0x55a5u):0u)==0x200u)state=text_state;
 if(b.set_fd_state_10415)b.set_fd_state_10415(handle,state);return handle;
}
static_assert(0xD4F2u-0xD332u==448u);

// D4F2..D55D: commit descriptor state with AL|40h; for device descriptors perform INT 21h/AH=44h get/set, mapping the encoded DX error word through 10314.
[[nodiscard]]s32 function_d4f2(FileBoundaries&b,u32 h)noexcept{u32 state=b.call_103c3?b.call_103c3(h):0;state=(state&~0xffu)|static_cast<u8>(state|0x40u);if(b.set_fd_state_10415)b.set_fd_state_10415(h,state);if((state&0x2000u)==0)return 0;u8 mode=0;if(!b.ioctl_get_mode||b.ioctl_get_mode(static_cast<u16>(h),mode)){return b.call_10314?b.call_10314(mode):-1;}mode=static_cast<u8>(mode|0x20u);if(!b.ioctl_set_mode||b.ioctl_set_mode(static_cast<u16>(h),mode)){return b.call_10314?b.call_10314(mode):-1;}return 0;}
static_assert(0xD55Eu-0xD4F2u==108u);
// D55E..D572: fixed wrapper calls D313(path,62h,mode), which forwards mode as D332 arg4.
[[nodiscard]]u32 function_d55e(FileBoundaries&b,u32 path,u32 mode)noexcept{return function_d313(b,path,0x62u,mode);}
static_assert(0xD573u-0xD55Eu==21u);
// D573..D5AC: AH=3Eh close. RCL/ROR materializes CF in EAX sign; success clears descriptor state, failure stores runtime errno 4.
[[nodiscard]]u32 function_d573(FileBoundaries&b,u32 h)noexcept{const bool carry=b.dos_close?b.dos_close(static_cast<u16>(h)):true;if(!carry){if(b.set_fd_state_10415)b.set_fd_state_10415(h,0);return 0;}set_runtime_error(b,4);return 0xffffffffu;}
static_assert(0xD5ADu-0xD573u==58u);
[[nodiscard]]s32 function_d5ad(FileBoundaries&b,u16 handle,u8*dst,u32 count)noexcept{const u32 flags=b.call_103c3?b.call_103c3(handle):0;if(flags==0){set_runtime_error(b,4);return -1;}if((flags&1u)==0){set_runtime_error(b,6);return -1;}if(flags&0x40u){const s32 n=b.dos_read?b.dos_read(handle,dst,count):-1;return n<0?map_dos_error(b,n):n;}u32 total=0,remaining=count;u8*writep=dst;while(remaining){const s32 n=b.dos_read?b.dos_read(handle,writep,remaining):-1;if(n<0)return map_dos_error(b,n);if(n==0)break;const u8*src=writep;const u8*end=writep+static_cast<u32>(n);u32 kept=0;while(src<end){const u8 ch=*src++;if(ch==0x1a){total+=kept;return static_cast<s32>(total);}if(ch==0x0d)continue;writep[kept++]=ch;}total+=kept;remaining-=kept;writep+=kept;if(flags&0x2000u)break;}return static_cast<s32>(total);}
[[nodiscard]]s32 function_d686(FileBoundaries&b,u16 handle,const u8*src,u32 count)noexcept{const u32 flags=b.call_103c3?b.call_103c3(handle):0;if(flags==0){set_runtime_error(b,4);return -1;}if((flags&2u)==0){set_runtime_error(b,6);return -1;}if(flags&0x80u){const s32 pos=b.dos_seek?b.dos_seek(handle,0,2):-1;if(pos<0)return map_dos_error(b,pos);}if(flags&0x40u){const s32 n=b.dos_write?b.dos_write(handle,src,count):-1;if(n<0)return map_dos_error(b,n);if(static_cast<u32>(n)!=count)set_runtime_error(b,12);return n;}const u32 stack_distance=b.call_1042d?b.call_1042d():0;if(stack_distance<0xb0u&&b.call_1046f)b.call_1046f();const u32 cap=stack_distance<0x230u?0x80u:0x200u;u8 temp[0x200]{};u32 consumed=0,last_flushed_input=0,fill=0;auto flush=[&](u32 amount)->s32{const s32 n=b.dos_write?b.dos_write(handle,temp,amount):-1;if(n<0)return map_dos_error(b,n);if(static_cast<u32>(n)!=amount){set_runtime_error(b,12);return static_cast<s32>(last_flushed_input+static_cast<u32>(n));}last_flushed_input=consumed;fill=0;return -0x40000000;};while(consumed<count){const u8 ch=src[consumed];if(ch==0x0a){temp[fill++]=0x0d;if(fill==cap){const s32 r=flush(cap);if(r!=-0x40000000)return r;}}temp[fill++]=ch;++consumed;if(fill==cap){const s32 r=flush(cap);if(r!=-0x40000000)return r;}}if(fill){const s32 n=b.dos_write?b.dos_write(handle,temp,fill):-1;if(n<0)return map_dos_error(b,n);if(static_cast<u32>(n)!=fill){set_runtime_error(b,12);return static_cast<s32>(last_flushed_input+static_cast<u32>(n));}}return static_cast<s32>(count);}
[[nodiscard]]s32 function_d85f(FileBoundaries&b,u16 handle,s32 offset,u8 origin)noexcept{const s32 r=b.dos_seek?b.dos_seek(handle,offset,origin):-1;return r<0?map_dos_error(b,r):r;} [[nodiscard]]s32 function_d8aa(FileBoundaries&b,const char*path)noexcept{const s32 r=b.dos_delete?b.dos_delete(path):-1;return r<0?map_dos_error(b,r):0;}

// F750..F7B2: source reader used by the following resource parsers. Bit 0 selects an in-memory source; otherwise seek/read through the DOS wrappers and return the handle's resulting current position.
[[nodiscard]]u32 function_f750(FileBoundaries&b,u32 source,u32 offset,u32 flags,u8*dst,u32 count)noexcept{
 if((flags&1u)!=0u){for(u32 i=0;i<count;++i)dst[i]=b.read_guest8?b.read_guest8(source+offset+i):0u;return offset+count;}
 (void)function_d85f(b,static_cast<u16>(source),static_cast<s32>(offset),0u);
 (void)function_d5ad(b,static_cast<u16>(source),dst,count);
 return static_cast<u32>(function_d85f(b,static_cast<u16>(source),0,1u));
}
static_assert(0xF7B3u-0xF750u==99u);

// F7B8..F8BC: determine required resource image size. The two-byte signature is read at the header-supplied offset and compared with the DGROUP string at 038Ch; each 24-byte record contributes its first dword.
[[nodiscard]]u32 function_f7b8(FileBoundaries&b,u32 source,u32 flags)noexcept{
 u32 h=source;if((flags&1u)==0u){h=function_d313(b,source,0x200u,0u);if(h==0xffffffffu)return 0u;}
 u8 offbuf[4]{}; (void)function_f750(b,h,0x3cu,flags,offbuf,4u); const u32 header=load_le32(offbuf);
 char sig[4]={' ',' ',0,0}; (void)function_f750(b,h,header,flags,reinterpret_cast<u8*>(sig),2u);
 const s32 mismatch=b.compare_guest_string?b.compare_guest_string(sig,0x38cu):1;
 if(mismatch!=0){(void)function_d573(b,h);return 0u;}
 u8 hdr[0xacu]{}; (void)function_f750(b,h,header,flags,hdr,0xacu);
 u32 pos=header+load_le32(hdr+0x40u); const u32 records=load_le32(hdr+0x44u); u32 total=0u;
 for(u32 i=0;i<records;++i){u8 rec[0x18u]{};pos=function_f750(b,h,pos,flags,rec,0x18u);total+=load_le32(rec);}
 if((flags&1u)==0u)(void)function_d573(b,h);return total+0x0fu;
}
static_assert(0xF8BDu-0xF7B8u==261u);


// F8C4..FC96: resource image loader/relocator. Allocated 32-bit guest addresses never become native pointers: the encoded F750 call whose destination is the loaded image and the final relocation dword store remain explicit guest-memory boundaries.
struct ResourceLoadBoundaries{u32(*alloc_71c8)(u32)=nullptr;void(*zero_84a0)(u32,u32,u32)=nullptr;u32(*read_f750_guest)(u32,u32,u32,u32,u32)=nullptr;void(*write_guest32)(u32,u32)=nullptr;};
[[nodiscard]]u32 function_f8c4(FileBoundaries&b,ResourceLoadBoundaries&lb,u32 source,u32 flags,u32 destination)noexcept{
 u32 handle=source,chunk_count=0,cursor=0;u32 segment_base[64]{},chunk_base[144]{};
 if((flags&1u)==0u){handle=function_d313(b,source,0x200u,0u);if(handle==0xffffffffu)return 0u;}
 const u32 image_size=function_f7b8(b,source,flags);
 if((flags&4u)!=0u){destination=lb.alloc_71c8?lb.alloc_71c8(image_size):0u;if(destination==0u){if((flags&1u)==0u)(void)function_d573(b,handle);return 0u;}}
 if(lb.zero_84a0)lb.zero_84a0(destination,0u,image_size);
 u32 header_off=0;cursor=function_f750(b,handle,0x3cu,flags,reinterpret_cast<u8*>(&header_off),4u);
 char sig[4]={' ',' ',0,0};cursor=function_f750(b,handle,header_off,flags,reinterpret_cast<u8*>(sig),2u);
 if((b.compare_guest_string?b.compare_guest_string(sig,0x38cu):1)!=0){if((flags&1u)==0u)(void)function_d573(b,handle);return 0u;}
 u8 h[0xacu]{};cursor=function_f750(b,handle,header_off,flags,h,0xacu);
 const u32 object_table_off=load_le32(h+0x40u),object_count=load_le32(h+0x44u),page_map_off=load_le32(h+0x48u),data_pages_off=load_le32(h+0x80u),fixup_page_off=load_le32(h+0x68u),fixup_record_off=load_le32(h+0x6cu),page_size=load_le32(h+0x28u),module_pages=load_le32(h+0x14u);const u8 page_shift=h[0x2cu];
 cursor=header_off+object_table_off;
 for(u32 object=0;object<object_count;++object){u8 rec[0x18u]{};cursor=function_f750(b,handle,cursor,flags,rec,0x18u);const u32 virtual_size=load_le32(rec),object_flags=load_le32(rec+8u),page_count=load_le32(rec+0x10u);u32 page_cursor=header_off+page_map_off;
  for(u32 page=0;page<page_count;++page){u8 map[8]{};page_cursor=function_f750(b,handle,page_cursor,flags,map,8u);if(page==0u){if((object_flags&2u)!=0u&&(object_flags&1u)!=0u&&(destination&0xfu)!=0u)destination+=0x10u-(destination&0xfu);segment_base[object]=destination;}chunk_base[chunk_count++]=destination;const u32 used=destination-segment_base[object];const u32 remain=virtual_size-used;const u32 encoded=static_cast<u32>(load_le16(map+4u));const u32 bytes=remain<encoded?remain:encoded;const u32 data_off=(load_le32(map)<<page_shift)+data_pages_off;if(lb.read_f750_guest)(void)lb.read_f750_guest(handle,data_off,flags,destination,bytes);destination+=bytes;}cursor=page_cursor;}
 u32 fix_cursor=header_off+fixup_page_off,previous=0;fix_cursor=function_f750(b,handle,fix_cursor,flags,reinterpret_cast<u8*>(&previous),4u);
 for(u32 page=0;page<module_pages;++page){u32 next=0;const u32 after=function_f750(b,handle,fix_cursor,flags,reinterpret_cast<u8*>(&next),4u);fix_cursor=after;if(previous!=next){u32 rec_cursor=header_off+fixup_record_off+previous;do{u8 source_type=0,target_flags=0;rec_cursor=function_f750(b,handle,rec_cursor,flags,&source_type,1u);rec_cursor=function_f750(b,handle,rec_cursor,flags,&target_flags,1u);if((source_type&7u)==0u||((target_flags&4u)!=0u&&(target_flags&0x20u)==0u)||(source_type&0x20u)!=0u){if((flags&1u)==0u)(void)function_d573(b,handle);return 0u;}u8 offbuf[2]{},idx=0,addbuf[2]{};rec_cursor=function_f750(b,handle,rec_cursor,flags,offbuf,2u);rec_cursor=function_f750(b,handle,rec_cursor,flags,&idx,1u);rec_cursor=function_f750(b,handle,rec_cursor,flags,addbuf,2u);const u32 src_off=load_le16(offbuf);if(src_off<=page_size){const u32 patch=chunk_base[page]+src_off,value=segment_base[idx]+load_le16(addbuf);if(lb.write_guest32)lb.write_guest32(patch,value);}}while(rec_cursor<header_off+fixup_record_off+next);fix_cursor=after;}previous=next;}
 if((flags&1u)==0u)(void)function_d573(b,handle);return destination;
}
static_assert(0xFC97u-0xF8C4u==979u);

// FCE9..FD2A: execute unprocessed six-byte runtime records in 55E0..55F1 by ascending priority. Equal priorities select the later record because the encoded JA only rejects strictly larger values.
struct RuntimeTailBoundaries{void(*call_guest_near)(u32)=nullptr;};
void function_fce9(Machine&m,RuntimeTailBoundaries&b)noexcept{
 for(;;){u32 best=0x55f2u;u8 priority=0xffu;for(u32 rec=0x55e0u;rec<0x55f2u;rec+=6u){const u8 state=*dgroup_ptr(m,dgroup(rec));const u8 pr=*dgroup_ptr(m,dgroup(rec+1u));if(state!=2u&&pr<=priority){best=rec;priority=pr;}}if(best==0x55f2u)return;const u32 fn=load_le32(dgroup_ptr(m,dgroup(best+2u)));if(fn&&b.call_guest_near)b.call_guest_near(fn);*dgroup_ptr(m,dgroup(best))=2u;}
}
static_assert(0xFD2Bu-0xFCE9u==66u);
// FD2B..FD6D: same six-byte record dispatcher for 55F2..55F7, selecting the greatest priority (again preferring the later record on equality).
void function_fd2b(Machine&m,RuntimeTailBoundaries&b)noexcept{
 for(;;){u32 best=0x55f8u;u8 priority=0u;for(u32 rec=0x55f2u;rec<0x55f8u;rec+=6u){const u8 state=*dgroup_ptr(m,dgroup(rec));const u8 pr=*dgroup_ptr(m,dgroup(rec+1u));if(state!=2u&&pr>=priority){best=rec;priority=pr;}}if(best==0x55f8u)return;const u32 fn=load_le32(dgroup_ptr(m,dgroup(best+2u)));if(fn&&b.call_guest_near)b.call_guest_near(fn);*dgroup_ptr(m,dgroup(best))=2u;}
}
static_assert(0xFD6Eu-0xFD2Bu==67u);

// FE21..FEFB: command-line token scanner. Arg1 is the mutable source cursor, arg2 is an optional argv array, arg3 receives the stopping cursor. Quoted tokens omit quotes; backslash only escapes quote/backslash in quoted mode and quote in unquoted mode.
[[nodiscard]]u32 function_fe21(u8*text,u8**argv,u8**end)noexcept{
 u32 argc=0;u8*p=text;
 for(;;){while(*p==0x20u||*p==0x09u)++p;if(*p==0){if(end)*end=p;return argc;}u8*start=nullptr;u8*out=nullptr;u8*after=nullptr;
  if(*p==0x22u){++p;start=p;out=p;for(;;){if(*p==0x22u||*p==0)break;if(*p==0x5cu&&(p[1]==0x22u||p[1]==0x5cu))++p;u8*next=p+1u;if(argv)*out++=*p;p=next;}}
  else{start=p;out=p;for(;;){if(*p==0||*p==0x20u||*p==0x09u)break;if(*p==0x5cu&&p[1]==0x22u)++p;u8*next=p+1u;if(argv)*out++=*p;p=next;}}
  after=p+1u;++argc;if(argv){argv[argc-1u]=start;if(*p==0){*out=0;if(end)*end=p;return argc;}after=p+1u;*out=0;p=after;}else{if(*p==0){if(end)*end=p;return argc;}p=after;}
 }
}
static_assert(0xFEFCu-0xFE21u==219u);

// FFD4..102D3: 256 three-byte interrupt veneers. Entry N is at FFD4+3*N and is exactly `INT N; RET`; the generic reconstruction preserves that table formula and dispatches the encoded vector through the machine interrupt boundary.
void function_interrupt_stub_ffd4(Machine&m,u8 vector)noexcept{interrupt(m,vector);}
[[nodiscard]]constexpr u32 interrupt_stub_address_ffd4(u8 vector)noexcept{return 0xffd4u+3u*static_cast<u32>(vector);}
static_assert(0x102D4u-0xFFD4u==256u*3u);

struct DirectoryBoundaries{s32(*map_error)(u16)=nullptr;s32(*get_current_directory)(u8 drive,char*out)=nullptr;u8(*get_current_drive)()=nullptr;s32(*set_current_drive)(u8 drive,u8*count)=nullptr;s32(*change_directory)(const char*)=nullptr;s32(*make_directory)(const char*)=nullptr;};
[[nodiscard]]u16 function_d8d0(DirectoryBoundaries&b,u32*out)noexcept{const u8 drive=b.get_current_drive?b.get_current_drive():0;if(out)*out=static_cast<u32>(drive)+1u;return 0;}
[[nodiscard]]u32 function_d8e8(DirectoryBoundaries&b,u32 drive,u32*out)noexcept{u8 count=0;const s32 r=b.set_current_drive?b.set_current_drive(static_cast<u8>(drive-1u),&count):0;if(out)*out=count;(void)r;return 0;}
struct CurrentDirBoundaries:DirectoryBoundaries{char*(*allocate)(u32)=nullptr;u32(*string_length)(const char*)=nullptr;void(*copy_string)(char*,const char*)=nullptr;};
[[nodiscard]]char* function_d906(CurrentDirBoundaries&b,char*out)noexcept{char tail[112]{};if(b.get_current_directory&&b.get_current_directory(0,tail)<0)return nullptr;char prefix[3]={static_cast<char>((b.get_current_drive?b.get_current_drive():0)+'A'),':','\\'};u32 tail_len=b.string_length?b.string_length(tail):static_cast<u32>(std::strlen(tail));char*dst=out;if(!dst){dst=b.allocate?b.allocate(tail_len+4u):nullptr;if(!dst)return nullptr;}dst[0]=prefix[0];dst[1]=prefix[1];dst[2]=prefix[2];if(b.copy_string)b.copy_string(dst+3,tail);else std::memcpy(dst+3,tail,tail_len+1u);return dst;}
[[nodiscard]]u32 function_d991(u32 ch)noexcept{return(ch>=0x61u&&ch<=0x7au)?ch-0x20u:ch;}
[[nodiscard]]s32 function_d9a6(DirectoryBoundaries&b,const char*path)noexcept{const s32 r=b.change_directory?b.change_directory(path):-1;return r<0?(b.map_error?b.map_error(static_cast<u16>(-r)):r):0;}
[[nodiscard]]s32 function_d9ca(DirectoryBoundaries&b,const char*path)noexcept{const s32 r=b.make_directory?b.make_directory(path):-1;return r<0?(b.map_error?b.map_error(static_cast<u16>(-r)):r):0;}
void* function_d9ee(void*dst,const void*src,u32 count)noexcept{return std::memcpy(dst,src,static_cast<std::size_t>(count));}
void function_da18(Machine&m)noexcept{m.cpu.interrupt_enable=false;} void function_da1a(Machine&m)noexcept{m.cpu.interrupt_enable=true;}

struct DosMemResult{bool carry=false;u16 ax=0;u16 bx=0;};
struct LowLevelBoundaries{DosMemResult(*allocate_paragraphs)(u16)=nullptr;DosMemResult(*free_segment)(u16)=nullptr;DosMemResult(*resize_segment)(u16,u16)=nullptr;u8(*in8)(u16)=nullptr;void(*out8)(u16,u8)=nullptr;void(*set_vector_legacy)(u8,u16,u32)=nullptr;void(*set_vector_2504)(u8,u16,u32)=nullptr;void(*get_vector_legacy)(u8,u16*,u32*)=nullptr;void(*get_vector_2502)(u8,u16*,u32*)=nullptr;s32(*map_dos_error)(u16)=nullptr;};
[[nodiscard]]s32 function_10384(LowLevelBoundaries&b,const DosMemResult&r)noexcept{return r.carry?(b.map_dos_error?b.map_dos_error(r.ax):-1):0;}
[[nodiscard]]s32 function_da48(LowLevelBoundaries&b,u32 paragraphs,u32*out_value)noexcept{const DosMemResult r=b.allocate_paragraphs?b.allocate_paragraphs(static_cast<u16>(paragraphs)):DosMemResult{true,1,0};if(out_value)*out_value=r.carry?static_cast<u32>(r.bx):static_cast<u32>(r.ax);return function_10384(b,r);}
[[nodiscard]]s32 function_da6c(LowLevelBoundaries&b,u32 segment)noexcept{const DosMemResult r=b.free_segment?b.free_segment(static_cast<u16>(segment)):DosMemResult{true,1,0};return function_10384(b,r);}
[[nodiscard]]s32 function_da7e(LowLevelBoundaries&b,u32 paragraphs,u32 segment,u32*out_max)noexcept{const DosMemResult r=b.resize_segment?b.resize_segment(static_cast<u16>(segment),static_cast<u16>(paragraphs)):DosMemResult{true,1,0};if(r.carry&&out_max)*out_max=r.bx;return function_10384(b,r);}
[[nodiscard]]u32 function_daa6(Machine&m)noexcept{u32 node=load_le32(dgroup_ptr(m,dgroup(0x30dc))),largest=0;while(node!=0x30d4u){const u32 size=load_le32(dgroup_ptr(m,dgroup(node)))-4u;if(size>largest)largest=size;node=load_le32(dgroup_ptr(m,dgroup(node+8u)));}return largest;}
void function_dde0()noexcept{}
[[nodiscard]]u8 function_dde7(LowLevelBoundaries&b,u32 port)noexcept{return b.in8?b.in8(static_cast<u16>(port)):0;}
void function_ddf2(LowLevelBoundaries&b,u32 port,u32 value)noexcept{if(b.out8)b.out8(static_cast<u16>(port),static_cast<u8>(value));}
void function_ddfe(LowLevelBoundaries&b,u8 extender_mode,u32 vector,u32 offset,u16 selector)noexcept{if(extender_mode>=2&&extender_mode<=8){if(b.set_vector_2504)b.set_vector_2504(static_cast<u8>(vector),selector,offset);}else if(b.set_vector_legacy)b.set_vector_legacy(static_cast<u8>(vector),selector,offset);}
struct VectorAddress{u32 offset=0;u16 selector=0;};
[[nodiscard]]VectorAddress function_de35(LowLevelBoundaries&b,u8 extender_mode,u32 vector)noexcept{VectorAddress r{};if(extender_mode>=2&&extender_mode<=8){if(b.get_vector_2502)b.get_vector_2502(static_cast<u8>(vector),&r.selector,&r.offset);}else if(b.get_vector_legacy)b.get_vector_legacy(static_cast<u8>(vector),&r.selector,&r.offset);return r;}
[[nodiscard]]u8* function_dea7(u8*dst,u32 pattern,u32 dword_count)noexcept{u8*p=dst;while(dword_count&&((reinterpret_cast<std::uintptr_t>(p)&31u)!=0)){store_le32(p,pattern);p+=4;--dword_count;}while(dword_count>=8){for(unsigned i=0;i<8;++i)store_le32(p+i*4u,pattern);p+=32;dword_count-=8;}while(dword_count){store_le32(p,pattern);p+=4;--dword_count;}return p;}
void function_df13(LowLevelBoundaries&b,u32 divisor_source)noexcept{if(divisor_source<=0x12u)return;u32 divisor=0x1234dcu/divisor_source;u8 gate=function_dde7(b,0x61);if((gate&3u)==0){function_ddf2(b,0x61,static_cast<u8>(gate|3u));function_ddf2(b,0x43,0xb6);}function_ddf2(b,0x42,divisor);function_ddf2(b,0x42,divisor>>8);}
void function_df73(LowLevelBoundaries&b)noexcept{function_ddf2(b,0x61,static_cast<u8>(function_dde7(b,0x61)&0xfcu));}
constexpr char kRadixDigits[]="0123456789abcdefghijklmnopqrstuvwxyz";
static char* convert_unsigned(u32 value,char*out,u32 radix)noexcept{char tmp[40]{};char*p=tmp+1;do{const u32 q=value/radix;const u32 rem=value%radix;*p++=kRadixDigits[rem];value=q;}while(value);char*d=out;do{*d++=*--p;}while(p!=tmp);return out;}
[[nodiscard]]char* function_dfb3(u32 value,char*out,u32 radix)noexcept{return convert_unsigned(value,out,radix);}
[[nodiscard]]char* function_e050(u32 value,char*out,u32 radix)noexcept{return convert_unsigned(value,out,radix);}
static char* convert_signed(s32 value,char*out,u32 radix,bool second)noexcept{char*digits=out;u32 magnitude=static_cast<u32>(value);if(radix==10u&&value<0){*digits++='-';magnitude=0u-magnitude;}if(second)(void)function_e050(magnitude,digits,radix);else(void)function_dfb3(magnitude,digits,radix);return out;}
[[nodiscard]]char* function_dffb(s32 value,char*out,u32 radix)noexcept{return convert_signed(value,out,radix,false);}
[[nodiscard]]char* function_e098(s32 value,char*out,u32 radix)noexcept{return convert_signed(value,out,radix,true);}

// E3B4..E4C6: verified callback dispatcher and INT 8 paths.
struct DispatcherBoundary{u32(*invoke)(u32,const u32*,u32)=nullptr;void(*invoke_timer_callback)(u32)=nullptr;void(*out8)(u16,u8)=nullptr;u32(*allocate_timer)(u32)=nullptr;void(*release_timer)(u32)=nullptr;void(*set_timer_rate)(u32,u32)=nullptr;void(*enable_timer)(u32)=nullptr;};
[[nodiscard]]u32 function_e3b4(Machine&m,u32 event,u32 slot)noexcept{if(slot>=16)return 0;u32 entry=load_le32(m.data+0x5216u+slot*4u);if(entry==0)return 0;for(;;entry+=8u){const u32 key=load_le32(m.data+entry);if(key==event)return load_le32(m.data+entry+4u);if(key==0xffffffffu)return 0;}}
[[nodiscard]]u32 function_e3e0(Machine&m,DispatcherBoundary&b,u32 event,const u32*args,u32 count)noexcept{if(count==0)return 0;const u32 target=function_e3b4(m,event,args[0]);return target&&b.invoke?b.invoke(target,args,count):0;}
enum class Irq8Action:u8{Handled,ChainSavedVector}; struct Irq8Result{Irq8Action action=Irq8Action::Handled;u16 selector=0;u32 offset=0;};
[[nodiscard]]Irq8Result function_e3f2(Machine&m,DispatcherBoundary&b)noexcept{if(load_le16(m.data+0x510e)!=0)return{Irq8Action::ChainSavedVector,static_cast<u16>(load_le32(m.data+0x5206)),load_le32(m.data+0x5202)};store_le16(m.data+0x510e,1);store_le32(m.data+0x520a,0);for(u32 slot=0;slot<=16;++slot){if(load_le16(m.data+0x5154u+slot*2u)==2){const u32 i=slot*4u;u32 accumulated=load_le32(m.data+0x5176u+i)+load_le32(m.data+0x51fe);const u32 period=load_le32(m.data+0x51bau+i);if(accumulated>=period){accumulated-=period;store_le32(m.data+0x5176u+i,accumulated);if(slot>=16)store_le16(m.data+0x510e,0);if(b.invoke_timer_callback)b.invoke_timer_callback(load_le32(m.data+0x5110u+i));}else store_le32(m.data+0x5176u+i,accumulated);}store_le32(m.data+0x520a,slot+1u);}store_le16(m.data+0x510e,0);if(b.out8)b.out8(0x20,0x20);return{};}

// E4C7..EA0A: verified timer/PIT setup and slot-state routines.
static bool irq_save_cli(Machine&m)noexcept{const bool prior=m.cpu.interrupt_enable;m.cpu.interrupt_enable=false;return prior;}
static void irq_restore(Machine&m,bool prior)noexcept{m.cpu.interrupt_enable=prior;}
void function_e4c7(Machine&m)noexcept{const bool prior=irq_save_cli(m);store_le32(dgroup_ptr(m,dgroup(0x51fe)),0xffffffffu);for(u32 i=0;i<17;++i)store_le16(dgroup_ptr(m,dgroup(0x5154u+i*2u)),0);for(u32 i=0;i<17;++i)store_le32(dgroup_ptr(m,dgroup(0x5176u+i*4u)),0);for(u32 i=0;i<17;++i)store_le32(dgroup_ptr(m,dgroup(0x51bau+i*4u)),0);irq_restore(m,prior);}
void function_e514(Machine&m,LowLevelBoundaries&b,u16 code_selector)noexcept{const bool prior=irq_save_cli(m);VectorAddress old=function_de35(b,0,8);m.cpu.edx=(m.cpu.edx&0xffff0000u)|old.selector;store_le32(dgroup_ptr(m,dgroup(0x5202)),old.offset);store_le32(dgroup_ptr(m,dgroup(0x5206)),m.cpu.edx);store_le32(dgroup_ptr(m,dgroup(0x5150)),0xe4a7u);if(b.set_vector_legacy)b.set_vector_legacy(8,code_selector,0xe3f2u);irq_restore(m,prior);}
void function_e563(Machine&m,LowLevelBoundaries&b)noexcept{const bool prior=irq_save_cli(m);store_le32(dgroup_ptr(m,dgroup(0x520a)),0xffffffffu);if(b.set_vector_legacy)b.set_vector_legacy(8,static_cast<u16>(load_le32(dgroup_ptr(m,dgroup(0x5206)))),load_le32(dgroup_ptr(m,dgroup(0x5202))));irq_restore(m,prior);}
void function_e59b(Machine&m,LowLevelBoundaries&b,u32 reload)noexcept{const bool prior=irq_save_cli(m);function_ddf2(b,0x43,0x36);store_le32(dgroup_ptr(m,dgroup(0x5212)),reload);function_ddf2(b,0x40,reload);function_ddf2(b,0x40,reload>>8);irq_restore(m,prior);}
void function_e5c9(Machine&m,LowLevelBoundaries&b,u32 period)noexcept{u32 reload=0;if(period<0xd68du)reload=static_cast<u32>((static_cast<std::uint64_t>(period)*0x2710u)/0x20bcu);function_e59b(m,b,reload);}
void function_e5fa(Machine&m,LowLevelBoundaries&b)noexcept{const bool prior=irq_save_cli(m);u32 minimum=0xffffffffu;for(u32 i=0;i<=16;++i){if(load_le16(dgroup_ptr(m,dgroup(0x5154u+i*2u)))!=0){const u32 candidate=load_le32(dgroup_ptr(m,dgroup(0x51bau+i*4u)));if(candidate<minimum)minimum=candidate;}}store_le32(dgroup_ptr(m,dgroup(0x520e)),minimum);if(minimum!=load_le32(dgroup_ptr(m,dgroup(0x51fe)))){store_le32(dgroup_ptr(m,dgroup(0x520a)),0xffffffffu);store_le32(dgroup_ptr(m,dgroup(0x51fe)),minimum);function_e5c9(m,b,minimum);for(u32 i=0;i<17;++i)store_le32(dgroup_ptr(m,dgroup(0x5176u+i*4u)),0);}irq_restore(m,prior);}
void function_e680(Machine&m)noexcept{const bool prior=irq_save_cli(m);store_le16(dgroup_ptr(m,dgroup(0x52e6)),m.cpu.ds);store_le16(dgroup_ptr(m,dgroup(0x510c)),0);store_le16(dgroup_ptr(m,dgroup(0x510e)),0);for(u32 i=0;i<16;++i)store_le32(dgroup_ptr(m,dgroup(0x5216u+i*4u)),0);for(u32 i=0;i<16;++i)store_le32(dgroup_ptr(m,dgroup(0x5256u+i*4u)),0xffffffffu);for(u32 i=0;i<16;++i)store_le32(dgroup_ptr(m,dgroup(0x5296u+i*4u)),0);irq_restore(m,prior);}

struct TimerContinuationBoundaries{void(*call_ebb6)(u32 slot,u32 argument)=nullptr;u16 code_selector=0;};
void function_e809(Machine&m,LowLevelBoundaries&b,u32 slot)noexcept; void function_e856(Machine&m,LowLevelBoundaries&b)noexcept; void function_e87b(Machine&m,u32 slot)noexcept; void function_e925(Machine&m,LowLevelBoundaries&b,u32 slot,u32 period)noexcept;
void function_e6ea(Machine&m,LowLevelBoundaries&b,TimerContinuationBoundaries&x,u32 argument)noexcept{const bool prior=irq_save_cli(m);store_le32(dgroup_ptr(m,dgroup(0x52da)),0);do{const u32 slot=load_le32(dgroup_ptr(m,dgroup(0x52da)));const u32 i=slot*4u;const u32 handle=load_le32(dgroup_ptr(m,dgroup(0x5256u+i)));if(load_le32(dgroup_ptr(m,dgroup(0x5216u+i)))!=0){if(handle!=0xffffffffu)function_e809(m,b,handle);if(x.call_ebb6)x.call_ebb6(slot,argument);}store_le32(dgroup_ptr(m,dgroup(0x52da)),slot+1u);}while(load_le32(dgroup_ptr(m,dgroup(0x52da)))!=16u);function_e856(m,b);irq_restore(m,prior);}
[[nodiscard]]u32 function_e759(Machine&m,LowLevelBoundaries&b,TimerContinuationBoundaries&x,u32 value)noexcept{const bool prior=irq_save_cli(m);u32 byte_index=0;while(byte_index<0x20u&&load_le16(dgroup_ptr(m,dgroup(0x5154u+byte_index)))!=0)byte_index+=2u;if(byte_index==0x20u){irq_restore(m,prior);return 0xffffffffu;}const u32 slot=byte_index>>1;store_le16(dgroup_ptr(m,dgroup(0x5154u+byte_index)),1);const u32 dword_index=slot*4u;store_le32(dgroup_ptr(m,dgroup(0x5110u+dword_index)),value);store_le32(dgroup_ptr(m,dgroup(0x51bau+dword_index)),0xffffffffu);const u16 count=static_cast<u16>(load_le16(dgroup_ptr(m,dgroup(0x510c)))+1u);store_le16(dgroup_ptr(m,dgroup(0x510c)),count);if(count==1u){function_e4c7(m);store_le16(dgroup_ptr(m,dgroup(0x5174)),1);function_e514(m,b,x.code_selector);function_e925(m,b,16,0xd68du);function_e87b(m,16);store_le16(dgroup_ptr(m,dgroup(0x5154u+slot*2u)),1);store_le32(dgroup_ptr(m,dgroup(0x51bau+dword_index)),0xffffffffu);}irq_restore(m,prior);return slot;}
void function_e809(Machine&m,LowLevelBoundaries&b,u32 slot)noexcept{const bool prior=irq_save_cli(m);if(slot!=0xffffffffu){const u32 wi=slot*2u;if(load_le16(dgroup_ptr(m,dgroup(0x5154u+wi)))!=0){store_le16(dgroup_ptr(m,dgroup(0x5154u+wi)),0);const u16 count=static_cast<u16>(load_le16(dgroup_ptr(m,dgroup(0x510c)))-1u);store_le16(dgroup_ptr(m,dgroup(0x510c)),count);if(count==0){function_e59b(m,b,0);function_e563(m,b);}}}irq_restore(m,prior);}
void function_e856(Machine&m,LowLevelBoundaries&b)noexcept{const bool prior=irq_save_cli(m);for(s32 slot=15;slot>=0;--slot)function_e809(m,b,static_cast<u32>(slot));irq_restore(m,prior);}
void function_e87b(Machine&m,u32 slot)noexcept{const bool prior=irq_save_cli(m);u8*p=dgroup_ptr(m,dgroup(0x5154u+slot*2u));if(load_le16(p)==1)store_le16(p,2);irq_restore(m,prior);}
void function_e8ab(Machine&m)noexcept{const bool prior=irq_save_cli(m);for(s32 slot=15;slot>=0;--slot)function_e87b(m,static_cast<u32>(slot));irq_restore(m,prior);}
void function_e8d0(Machine&m,u32 slot)noexcept{const bool prior=irq_save_cli(m);u8*p=dgroup_ptr(m,dgroup(0x5154u+slot*2u));if(load_le16(p)==2)store_le16(p,1);irq_restore(m,prior);}
void function_e900(Machine&m)noexcept{const bool prior=irq_save_cli(m);for(s32 slot=15;slot>=0;--slot)function_e8d0(m,static_cast<u32>(slot));irq_restore(m,prior);}
void function_e925(Machine&m,LowLevelBoundaries&b,u32 slot,u32 period)noexcept{const bool prior=irq_save_cli(m);u8*state=dgroup_ptr(m,dgroup(0x5154u+slot*2u));const u16 saved=load_le16(state);store_le16(state,1);store_le32(dgroup_ptr(m,dgroup(0x51bau+slot*4u)),period);store_le32(dgroup_ptr(m,dgroup(0x5176u+slot*4u)),0);function_e5fa(m,b);store_le16(state,saved);irq_restore(m,prior);}
void function_e97a(Machine&m,LowLevelBoundaries&b,u32 slot,u32 divisor)noexcept{const bool prior=irq_save_cli(m);function_e925(m,b,slot,1000000u/divisor);irq_restore(m,prior);}
void function_e9ad(Machine&m,LowLevelBoundaries&b,u32 slot,u32 value)noexcept{const bool prior=irq_save_cli(m);u32 period=0xd68du;if(value!=0)period=static_cast<u32>((static_cast<std::uint64_t>(0x2710u)*value)/0x2e9cu);function_e925(m,b,slot,period);irq_restore(m,prior);}
[[nodiscard]]u32 function_e9f2(Machine&m)noexcept{const bool prior=irq_save_cli(m);const u32 value=load_le32(dgroup_ptr(m,dgroup(0x5212)));irq_restore(m,prior);return value;}

// EA0B..EDE6: verified registration, event dispatch wrappers, and frame no-op.
void function_ea9a(Machine&m,u32 slot)noexcept{const bool prior=irq_save_cli(m);if(slot<16)store_le32(m.data+0x5216u+slot*4u,0);irq_restore(m,prior);}
[[nodiscard]]u32 function_eac8(Machine&m,DispatcherBoundary&b,u32 slot)noexcept{const u32 args[2]={slot,0xe9f2u};return function_e3e0(m,b,0x64u,args,2);}
[[nodiscard]]u32 function_ea0b(Machine&m,DispatcherBoundary&b,u32 descriptor)noexcept{const bool prior=irq_save_cli(m);u32 slot=0;while(slot<16&&load_le32(m.data+0x5216u+slot*4u)!=0)++slot;u32 result=0xffffffffu;if(slot<16&&load_le32(m.data+descriptor+4u)==0x79706f43u){store_le32(m.data+0x5216u+slot*4u,load_le32(m.data+descriptor));const u32 returned=function_eac8(m,b,slot);if(returned!=0&&load_le32(m.data+returned)<=load_le32(m.data+0x52e8u))result=slot;}irq_restore(m,prior);return result;}
[[nodiscard]]u32 function_eae7(Machine&m,DispatcherBoundary&b,const u32*args,u32 count)noexcept{return function_e3e0(m,b,0x65u,args,count);}
void function_eaf1(Machine&m,DispatcherBoundary&b,u32 a1,u32 a2,u32 a3,u32 a4,u32 a5)noexcept{const bool prior=irq_save_cli(m);if(a1<16){store_le32(m.data+0x52e2,0xffffffffu);const u32 record=function_eac8(m,b,a1);const u32 divisor=load_le32(m.data+record+0x20u);if(divisor!=0xffffffffu){const u32 target=function_e3b4(m,0x67u,a1);if(target!=0&&b.allocate_timer){const u32 timer=b.allocate_timer(target);store_le32(m.data+0x5256u+a1*4u,timer);store_le32(m.data+0x52e2,timer);if(b.set_timer_rate)b.set_timer_rate(timer,divisor);}}const u32 args[5]={a1,a2,a3,a4,a5};(void)function_e3e0(m,b,0x66u,args,5);store_le32(m.data+0x5296u+a1*4u,1);const u32 timer=load_le32(m.data+0x52e2);if(timer!=0xffffffffu&&b.enable_timer)b.enable_timer(timer);}irq_restore(m,prior);}
[[nodiscard]]u32 function_ebb6(Machine&m,DispatcherBoundary&b,u32 slot,u32 argument)noexcept{if(slot>=16)return 0;const u32 i=slot*4u;const u32 pending=load_le32(m.data+0x5296u+i);store_le32(m.data+0x5296u+i,0);if(pending==0)return 0;const u32 timer=load_le32(m.data+0x5256u+i);if(timer!=0xffffffffu&&b.release_timer)b.release_timer(timer);const u32 args[2]={slot,argument};return function_e3e0(m,b,0x68u,args,2);}
constexpr u32 kWrapperEvents[48]={0x78,0x79,0x86,0x7a,0x7b,0x85,0x7c,0x7d,0x7e,0x7f,0x80,0x81,0x82,0x83,0x84,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,0xaa,0xab,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,0xc0,0xc1,0xc2};
[[nodiscard]]u32 function_ebf3_edd2(Machine&m,DispatcherBoundary&b,u32 wrapper_index,const u32*args,u32 count)noexcept{return wrapper_index<48?function_e3e0(m,b,kWrapperEvents[wrapper_index],args,count):0;}
void function_ede0()noexcept{}

// F007..F741: verified formatter parser, conversion materializer, and helpers.
struct FormatState{u8 opaque0[4];u32 width;u32 precision;u32 zero_count;u8 opaque10[4];u8 flags;u8 conversion;u8 pad;u8 prefix[3];}; static_assert(sizeof(FormatState)==0x1c); static_assert(offsetof(FormatState,width)==0x04&&offsetof(FormatState,precision)==0x08&&offsetof(FormatState,zero_count)==0x0c&&offsetof(FormatState,flags)==0x14&&offsetof(FormatState,conversion)==0x15&&offsetof(FormatState,pad)==0x16&&offsetof(FormatState,prefix)==0x17);
[[nodiscard]]const u8* function_f110(const u8*p,FormatState&s)noexcept{s.flags=0;for(;;++p){switch(*p){case '-':s.flags|=0x08;break;case '#':s.flags|=0x01;break;case '+':s.flags=static_cast<u8>((s.flags|0x04)&0xfd);break;case ' ':if((s.flags&0x04)==0)s.flags|=0x02;break;case '0':s.pad='0';break;default:return p;}}}
[[nodiscard]]const u8* function_f007(const u8*p,u8*&arg_cursor,FormatState&s)noexcept{s.prefix[0]=0;s.pad=' ';p=function_f110(p,s);s.width=0;if(*p>='0'&&*p<='9'){do{s.width=s.width*10u+static_cast<u32>(*p-'0');++p;}while(*p>='0'&&*p<='9');}else if(*p=='*'){arg_cursor+=4;s.width=load_le32(arg_cursor-4);if(static_cast<s32>(s.width)<0){s.width=0u-s.width;s.flags|=0x08;}++p;}s.precision=0xffffffffu;if(*p=='.'){++p;s.precision=0;if(*p=='*'){arg_cursor+=4;s.precision=load_le32(arg_cursor-4);if(static_cast<s32>(s.precision)<0)s.precision=0xffffffffu;++p;}else while(*p>='0'&&*p<='9'){s.precision=s.precision*10u+static_cast<u32>(*p-'0');++p;}}const u8 c=*p;const u8*next=p+1;if(c=='l'||c=='w'){++p;s.flags|=0x20;}else if(c=='h'){p=next;s.flags|=0x10;}else if(c=='F'){p=next;s.flags|=0x80;}else if(c=='N'||c=='L'){p=next;s.flags|=0x40;}return p;}
struct FarReadBoundary{u8(*read8)(u16,u32)=nullptr;u16(*read16)(u16,u32)=nullptr;};
[[nodiscard]]u32 function_f163(FarReadBoundary&b,u32 offset,u16 selector,u32 limit)noexcept{u32 n=0;while(n!=limit){if(!b.read8||b.read8(selector,offset)==0)break;++offset;++n;}return n;}
[[nodiscard]]u32 function_f18c(FarReadBoundary&b,u32 offset,u16 selector,u32 limit)noexcept{u32 n=0;while(n!=limit){if(!b.read16||b.read16(selector,offset)==0)break;offset+=2;++n;}return n;}
struct StringBoundary{u32(*length)(const char*)=nullptr;};
void function_f1b3(StringBoundary&b,u32 value,char*out,u32 width)noexcept{(void)function_dffb(static_cast<s32>(value),out,16);const u32 n=b.length?b.length(out):static_cast<u32>(std::strlen(out));s32 pad=static_cast<s32>(width)-1;char*src=out+n;char*dst=out+pad;while(src!=out){--src;*dst--=*src;--pad;}char*z=out+pad;while(pad>=0){*z--='0';--pad;}out[width]='\0';}
void function_f722(char*s)noexcept{while(*s){*s=static_cast<char>(function_d991(static_cast<u8>(*s)));++s;}}

// F206..F2D1: signed 16.16 fixed-point decimal materialization.
void function_f206(char*out,s32 value,FormatState&s)noexcept{char*p=out;u32 fixed=static_cast<u32>(value);if(value<0){fixed=0u-fixed;*p++='-';}if(s.precision==0xffffffffu)s.precision=4;(void)function_dffb(static_cast<s32>(static_cast<u16>(fixed>>16)),p,10);char*integer_begin=p;char*end=p;while(*end)++end;if(s.precision!=0){*end++='.';for(u32 i=0;i<s.precision;++i){fixed&=0x0000ffffu;fixed*=10u;*end++=static_cast<char>(static_cast<u8>(fixed>>16)+static_cast<u8>('0'));}*end='\0';}if((fixed&0x00008000u)!=0){char*q=end;for(;;){if(q==integer_begin){char*scan=integer_begin+1;*integer_begin='1';while(*scan=='0')++scan;if(*scan=='.'){*scan='0';char*dot=scan+1;*dot='.';scan=dot+1;while(*scan=='0')++scan;}*scan++='0';*scan='\0';break;}--q;if(*q=='.')--q;if(*q!='9'){++*q;break;}*q='0';}}}

// F2D2..F2E8: indirect conversion thunk; callee returns EAX:DX.
struct FarReturn{u32 offset=0;u16 selector=0;};
struct FormatterBoundary{FarReturn(*call_55c4)(u32,u32,u32)=nullptr;u8(*read_linear8)(u32)=nullptr;u16(*read_linear16)(u32)=nullptr;u32(*read_linear32)(u32)=nullptr;void(*write_linear32)(u32,u32)=nullptr;u8(*read_selector8)(u16,u32)=nullptr;u16(*read_selector16)(u16,u32)=nullptr;};
[[nodiscard]]FarReturn function_f2d2(FormatterBoundary&b,u32 scratch,u32 cursor_cell,u32 state)noexcept{return b.call_55c4?b.call_55c4(scratch,cursor_cell,state):FarReturn{};}
static u32 formatter_get32(FormatterBoundary&b,u32 a)noexcept{return b.read_linear32?b.read_linear32(a):0;}
static void formatter_put32(FormatterBoundary&b,u32 a,u32 v)noexcept{if(b.write_linear32)b.write_linear32(a,v);}
static u32 formatter_take32(FormatterBoundary&b,u32 c)noexcept{const u32 n=formatter_get32(b,c)+4u;formatter_put32(b,c,n);return formatter_get32(b,n-4u);}
static s32 formatter_take_s16(FormatterBoundary&b,u32 c)noexcept{return static_cast<s16>(formatter_take32(b,c));}
static u16 formatter_take_u16(FormatterBoundary&b,u32 c)noexcept{return static_cast<u16>(formatter_take32(b,c));}
static u32 formatter_bounded8(FormatterBoundary&b,u32 p,u16 s,u32 l)noexcept{FarReadBoundary r{b.read_selector8,b.read_selector16};return function_f163(r,p,s,l);}
static u32 formatter_bounded16(FormatterBoundary&b,u32 p,u16 s,u32 l)noexcept{FarReadBoundary r{b.read_selector8,b.read_selector16};return function_f18c(r,p,s,l);}
static u32 formatter_strlen(const char*p)noexcept{return static_cast<u32>(std::strlen(p));}
static void formatter_hexpad(u32 v,char*out,u32 w)noexcept{StringBoundary sb{formatter_strlen};function_f1b3(sb,v,out,w);}
struct MaterializeResult{u32 source_offset=0;u16 source_selector=0;u32 accounting_eax=0;};

// F2E9..F721: conversion switch and common tail. The original jumps to the
// caller's F001 epilogue; the returned structure is the EAX/DX-equivalent
// payload state immediately before that shared epilogue.
[[nodiscard]]MaterializeResult function_f2e9(FormatterBoundary&b,char*scratch,u32 scratch_guest,u32 cursor_cell,FormatState&s,u32 state_guest,u16 ds,u32 null_string_offset)noexcept{MaterializeResult r{scratch_guest,ds,0};u32 numeric=0;bool sign_processing=true;const u8 conv=s.conversion;s.zero_count=0;if(conv=='X'||conv=='o'||conv=='u'||conv=='x'){if(s.flags&0x20u)numeric=formatter_take32(b,cursor_cell);else if(s.flags&0x10u)numeric=formatter_take_u16(b,cursor_cell);else numeric=formatter_take32(b,cursor_cell);s.flags=static_cast<u8>(s.flags&0xf9u);}if(s.precision!=0xffffffffu&&(conv=='X'||conv=='d'||conv=='i'||conv=='o'||conv=='u'||conv=='x'))s.pad=' ';switch(conv){case 'E':case 'F':case 'G':case 'e':case 'f':case 'g':if((conv=='F'||conv=='f')&&(s.flags&0x10u)){function_f206(scratch,static_cast<s32>(formatter_take32(b,cursor_cell)),s);r.accounting_eax=formatter_strlen(scratch);}else{const FarReturn q=function_f2d2(b,scratch_guest,cursor_cell,state_guest);r.source_offset=q.offset;r.source_selector=q.selector;r.accounting_eax=formatter_bounded8(b,q.offset,q.selector,0xffffffffu);}break;case 'o':{char*digits=scratch;if(s.flags&1u)*digits++='0';(void)function_e098(static_cast<s32>(numeric),digits,8);if(s.precision==0&&digits[0]=='0')scratch[0]=0;r.accounting_eax=formatter_strlen(scratch);break;}case 'd':case 'i':{s32 value;if(s.flags&0x20u)value=static_cast<s32>(formatter_take32(b,cursor_cell));else if(s.flags&0x10u)value=formatter_take_s16(b,cursor_cell);else value=static_cast<s32>(formatter_take32(b,cursor_cell));if(s.flags&0x30u)(void)function_e098(value,scratch,10);else(void)function_dffb(value,scratch,10);if(s.precision==0&&scratch[0]=='0')scratch[0]=0;r.accounting_eax=formatter_strlen(scratch);break;}case 'u':(void)function_e050(numeric,scratch,10);if(s.precision==0&&scratch[0]=='0')scratch[0]=0;r.accounting_eax=formatter_strlen(scratch);break;case 'x':case 'X':if((s.flags&1u)&&numeric!=0){s.prefix[0]='0';s.prefix[1]=conv;s.prefix[2]=0;}(void)function_e050(numeric,scratch,16);if(conv=='X')function_f722(scratch);if(s.precision==0&&scratch[0]=='0')scratch[0]=0;r.accounting_eax=formatter_strlen(scratch);break;case 's':case 'S':{u32 p;u16 sel=ds;if(s.flags&0x80u){const u32 n=formatter_get32(b,cursor_cell)+8u;formatter_put32(b,cursor_cell,n);p=formatter_get32(b,n-8u);sel=b.read_linear16?b.read_linear16(n-4u):0;if(p==0&&sel==0){p=null_string_offset;sel=ds;}}else{p=formatter_take32(b,cursor_cell);if(p==0)p=null_string_offset;}r.source_offset=p;r.source_selector=sel;s.flags=static_cast<u8>(s.flags&0xf9u);sign_processing=false;if(conv=='S'){if(s.flags&0x20u){r.accounting_eax=b.read_selector16?b.read_selector16(sel,p):0;r.source_offset=p+2u;}else{r.accounting_eax=b.read_selector8?b.read_selector8(sel,p):0;r.source_offset=p+1u;}}else r.accounting_eax=(s.flags&0x20u)?formatter_bounded16(b,p,sel,s.precision):formatter_bounded8(b,p,sel,s.precision);break;}case 'p':case 'P':{if(s.width==0)s.width=(s.flags&0x80u)?13u:8u;s.flags=static_cast<u8>(s.flags&0xf9u);const u32 off=formatter_take32(b,cursor_cell);if(s.flags&0x80u){const u16 sel=formatter_take_u16(b,cursor_cell);formatter_hexpad(sel,scratch,4);scratch[4]=':';formatter_hexpad(off,scratch+5,8);}else formatter_hexpad(off,scratch,8);if(conv=='P')function_f722(scratch);r.accounting_eax=formatter_strlen(scratch);break;}case 'c':scratch[0]=static_cast<char>(formatter_take32(b,cursor_cell));scratch[1]=0;sign_processing=false;s.precision=1;s.flags=static_cast<u8>(s.flags&0xf9u);r.accounting_eax=1;break;default:s.width=0;scratch[0]=static_cast<char>(conv);scratch[1]=0;sign_processing=false;s.precision=1;s.flags=static_cast<u8>(s.flags&0xf9u);r.accounting_eax=1;break;}if(sign_processing){const bool local_source=(r.source_selector==ds&&r.source_offset==scratch_guest);const u8 first=local_source?static_cast<u8>(scratch[0]):(b.read_selector8?b.read_selector8(r.source_selector,r.source_offset):0);if(first=='-'){--r.accounting_eax;s.prefix[0]='-';s.prefix[1]=0;++r.source_offset;}else if(s.flags&2u){s.prefix[0]=' ';s.prefix[1]=0;}else if(s.flags&4u){s.prefix[0]='+';s.prefix[1]=0;}if(static_cast<s32>(r.accounting_eax)>static_cast<s32>(s.precision))s.precision=r.accounting_eax;else s.zero_count=s.precision-r.accounting_eax;}if(s.pad=='*'){s.prefix[0]=0;s.flags=static_cast<u8>(s.flags&0xf9u);}if((s.precision==0xffffffffu||static_cast<s32>(r.accounting_eax)<static_cast<s32>(s.precision))&&conv!='c')s.precision=r.accounting_eax;const u32 occupied=formatter_strlen(reinterpret_cast<const char*>(s.prefix))+s.precision+s.zero_count;s.width-=occupied;return r;}

// EDE7..F006: verified formatter driver. F2E9 shares the original F001
// epilogue; the C++ boundary represents that encoded edge as a normal return.
struct FormatterDriverBoundary{
 const u8*(*parse_f007)(const u8*,u32 cursor_cell,FormatState&)=nullptr;
 MaterializeResult(*materialize_f2e9)(u32 scratch_guest,u32 cursor_cell,FormatState&,u32 state_guest)=nullptr;
 void(*emit)(FormatState&,u8)=nullptr;
 u32(*read_linear32)(u32)=nullptr;u16(*read_linear16)(u32)=nullptr;
 void(*write_linear32)(u32,u32)=nullptr;void(*write_linear16)(u32,u16)=nullptr;
 u8(*read_selector8)(u16,u32)=nullptr;
 void(*write_selector32)(u16,u32,u32)=nullptr;void(*write_selector16)(u16,u32,u16)=nullptr;
};
static u32 driver_rd32(FormatterDriverBoundary&b,u32 a)noexcept{return b.read_linear32?b.read_linear32(a):0;}
static u16 driver_rd16(FormatterDriverBoundary&b,u32 a)noexcept{return b.read_linear16?b.read_linear16(a):0;}
static void driver_wr32(FormatterDriverBoundary&b,u32 a,u32 v)noexcept{if(b.write_linear32)b.write_linear32(a,v);}
static u32 driver_take32(FormatterDriverBoundary&b,u32 c)noexcept{const u32 n=driver_rd32(b,c)+4u;driver_wr32(b,c,n);return driver_rd32(b,n-4u);}
static u32 driver_count(const FormatState&s)noexcept{return load_le32(s.opaque10);}
static void driver_set_count(FormatState&s,u32 v)noexcept{store_le32(s.opaque10,v);}
static void driver_output(FormatterDriverBoundary&b,FormatState&s,u8 ch)noexcept{if(b.emit)b.emit(s,ch);}
[[nodiscard]]u32 function_ede7(FormatterDriverBoundary&b,u32 initial_context,const u8*format,u32 cursor_cell,u32 scratch_guest,u32 state_guest)noexcept{
 FormatState s{};store_le32(s.opaque0,initial_context);driver_set_count(s,0);const u8*p=format;
 for(;;){
  if(*p==0)return driver_count(s);
  if(*p!='%'){driver_output(b,s,*p++);continue;}
  // EE72/EE75 increment EAX before EE79 reaches EE10: F007 receives p+1.
  if(!b.parse_f007)return driver_count(s);const u8*conversion=b.parse_f007(p+1,cursor_cell,s);s.conversion=*conversion;p=conversion+1;if(s.conversion==0)return driver_count(s);
  if(s.conversion=='n'){
   const u32 count=driver_count(s);
   if(s.flags&0x20u){if(s.flags&0x80u){const u32 base=driver_rd32(b,cursor_cell)+8u;driver_wr32(b,cursor_cell,base);const u32 off=driver_rd32(b,base-8u);const u16 sel=driver_rd16(b,base-4u);if(b.write_selector32)b.write_selector32(sel,off,count);}else{const u32 dst=driver_take32(b,cursor_cell);driver_wr32(b,dst,count);}}
   else if(s.flags&0x10u){if(s.flags&0x80u){const u32 base=driver_rd32(b,cursor_cell)+8u;driver_wr32(b,cursor_cell,base);const u32 off=driver_rd32(b,base-8u);const u16 sel=driver_rd16(b,base-4u);if(b.write_selector16)b.write_selector16(sel,off,static_cast<u16>(count));}else{const u32 dst=driver_take32(b,cursor_cell);if(b.write_linear16)b.write_linear16(dst,static_cast<u16>(count));}}
   else{if(s.flags&0x80u){const u32 base=driver_rd32(b,cursor_cell)+8u;driver_wr32(b,cursor_cell,base);const u32 off=driver_rd32(b,base-8u);const u16 sel=driver_rd16(b,base-4u);if(b.write_selector32)b.write_selector32(sel,off,count);}else{const u32 dst=driver_take32(b,cursor_cell);driver_wr32(b,dst,count);}}continue;
  }
  if(!b.materialize_f2e9)return driver_count(s);MaterializeResult r=b.materialize_f2e9(scratch_guest,cursor_cell,s,state_guest);u32 source=r.source_offset;const u16 selector=r.source_selector;
  if((s.flags&0x08u)==0&&s.pad==' '){while(static_cast<s32>(--s.width)>=0)driver_output(b,s,s.pad);}
  for(u32 i=0;i<3u&&s.prefix[i]!=0;++i)driver_output(b,s,s.prefix[i]);
  for(;;){const u32 old=s.zero_count--;if(old==0)break;driver_output(b,s,'0');}
  if((s.flags&0x08u)==0&&s.pad!=' '){while(static_cast<s32>(--s.width)>=0)driver_output(b,s,s.pad);}
  for(;;){const u32 old=s.precision--;if(old==0)break;const u8 ch=b.read_selector8?b.read_selector8(selector,source):0;driver_output(b,s,ch);source+=((s.conversion=='s'||s.conversion=='S')&&(s.flags&0x20u))?2u:1u;}
  if(s.flags&0x08u){while(static_cast<s32>(s.width)>0){const u32 old=s.width--;if(old==0)break;driver_output(b,s,' ');}}
 }
}

// DA1C..DA44: 41-byte wrapper. 104B1 initializes the 12-byte local adapter
// cell; CEC8 receives the original three dword arguments followed by that cell.
// The boundary stays address-named because CEC8/FEFC carries selector/register
// state that is not yet representable as an ordinary host call.
struct DosAdapterBoundary{void(*call_104b1)(u32 guest_cell)=nullptr;u32(*call_cec8)(u32,u32,u32,u32)=nullptr;};
[[nodiscard]]u32 function_da1c(DosAdapterBoundary&b,u32 a1,u32 a2,u32 a3,u32 local_guest)noexcept{if(b.call_104b1)b.call_104b1(local_guest);return b.call_cec8?b.call_cec8(a1,a2,a3,local_guest):0;}

// DAD9..DB7F. The encoded routine calls the allocator core at 104E0 with
// EAX=request, EDX=DS, EBX=30B8h, then tries 10AA9 once and 10C46 as the
// retry gate. DB5B passes the pointer to the free core at 10590. Names stay
// address-based until the surrounding runtime contracts are integrated.
struct AllocBoundary{
 u32(*call_104e0)(u32 request,u16 ds,u32 arena)=nullptr;
 u32(*call_10aa9)(u32 request)=nullptr;
 u32(*call_10c46)(u32 request)=nullptr;
 void(*call_10590)(u32 pointer,u16 ds,u32 arena)=nullptr;
 void(*write_linear8)(u32,u8)=nullptr;
 u16 ds=0;
};
[[nodiscard]]u32 function_dad9(AllocBoundary&b,u32 request)noexcept{
 if(request==0)return 0;
 bool retried_10aa9=false;
 for(;;){
  const u32 result=b.call_104e0?b.call_104e0(request,b.ds,0x30b8u):0;
  if(result!=0){if(b.write_linear8)b.write_linear8(0x8e78u,0);return result;}
  if(!retried_10aa9){if(b.call_10aa9&&b.call_10aa9(request)!=0){retried_10aa9=true;continue;}}
  if(!b.call_10c46||b.call_10c46(request)==0){if(b.write_linear8)b.write_linear8(0x8e78u,0);return 0;}
 }
}
void function_db5b(AllocBoundary&b,u32 pointer)noexcept{if(b.call_10590)b.call_10590(pointer,b.ds,0x30b8u);if(b.write_linear8)b.write_linear8(0x8e78u,0);}
void function_db4b(AllocBoundary&b,u32 pointer)noexcept{function_db5b(b,pointer);}
// DAC9..DAD8: exact 16-byte frame wrapper around DAD9.
[[nodiscard]]u32 function_dac9(AllocBoundary&b,u32 request)noexcept{return function_dad9(b,request);}

// DB80..DC8C. Exact 4096-byte ring dictionary walk. 7360 returns a
// zero-extended input byte; 738C consumes one output byte. The dictionary is
// initialized to FEh, write index FEEh. SI is shifted before testing the 0100h
// refill marker. Back-references combine two bytes as
// ((second & F0h)<<4)|first and copy (second&0Fh)+3 bytes.
struct DecodeBoundary{u8(*input_7360)()=nullptr;void(*output_738c)(u8)=nullptr;u8(*read_linear8)(u32)=nullptr;};
void function_db80(DecodeBoundary&b)noexcept{
 u8 dict[4096];std::memset(dict,0xfe,sizeof(dict));u16 write_index=0x0feeu;u16 si=0;
 for(;;){
  si=static_cast<u16>(si>>1);
  if((si&0x0100u)==0){if(b.read_linear8&&b.read_linear8(0x89ceu)==1)return;const u8 v=b.input_7360?b.input_7360():0;si=static_cast<u16>(0xff00u|v);}
  if(si&1u){
   if(b.read_linear8&&b.read_linear8(0x89ceu)==1)return;const u8 v=b.input_7360?b.input_7360():0;if(b.output_738c)b.output_738c(v);dict[write_index]=v;write_index=static_cast<u16>((write_index+1u)&0x0fffu);
  }else{
   if(b.read_linear8&&b.read_linear8(0x89ceu)==1)return;const u8 first=b.input_7360?b.input_7360():0;
   if(b.read_linear8&&b.read_linear8(0x89ceu)==1)return;const u8 second=b.input_7360?b.input_7360():0;
   const u16 source=static_cast<u16>((static_cast<u16>(second&0xf0u)<<4)|first);const u16 last=static_cast<u16>((second&0x0fu)+2u);
   for(u16 i=0;i<=last;++i){const u8 v=dict[(source+i)&0x0fffu];if(b.output_738c)b.output_738c(v);dict[write_index]=v;write_index=static_cast<u16>((write_index+1u)&0x0fffu);}
  }
 }
}

// DC90..DDD7. The routine operates only when header word +4 == 1. It builds
// a word permutation table in workspace 30E8h, then uses REPNE SCASW to find
// the current index in the remaining suffix. SCASW compares the initial EDI
// element before advancing EDI; after the instruction the encoded SUB EDI,2
// restores EDI to the matched element (or final examined element on failure).
struct TransformBoundary{u16(*read16)(u32)=nullptr;u8(*read8)(u32)=nullptr;void(*write16)(u32,u16)=nullptr;void(*write8)(u32,u8)=nullptr;};
static u16 tr16(TransformBoundary&b,u32 a)noexcept{return b.read16?b.read16(a):0;}static u8 tr8(TransformBoundary&b,u32 a)noexcept{return b.read8?b.read8(a):0;}static void tw16(TransformBoundary&b,u32 a,u16 v)noexcept{if(b.write16)b.write16(a,v);}static void tw8(TransformBoundary&b,u32 a,u8 v)noexcept{if(b.write8)b.write8(a,v);}
void function_dc90(TransformBoundary&b,u32 base)noexcept{
 if(tr16(b,base+4)!=1)return;u32 dst=base+8u;u32 source_base=base+8u;if(tr8(b,base+7u)&0x80u){++dst;++source_base;}
 const u32 row=tr16(b,base);const u32 stride=static_cast<u32>(tr8(b,base+6u))<<1;u32 remaining=4u*tr16(b,base+2u);const u32 table_words=remaining;const u32 wrap=remaining<<1;
 u32 bx=0,eax=0;for(u32 ecx=table_words;ecx!=0;--ecx){tw16(b,0x30e8u+bx,static_cast<u16>(eax));bx+=8u;++eax;if(bx>=wrap){bx-=wrap;bx+=2u;}}
 --remaining;dst+=stride;u32 index=1;
 while(remaining!=0){
  const u32 table_entry=0x30e8u+(index<<1);const u16 saved_table_value=tr16(b,table_entry);u32 scan=remaining;u32 after=table_entry;
  if(scan!=0){do{const u16 examined=tr16(b,after);after+=2u;--scan;if(examined==static_cast<u16>(index))break;}while(scan!=0);}
  const u32 match=after-2u;const u16 displaced=tr16(b,match);tw16(b,match,saved_table_value);const u32 mapped=(match-0x30e8u)>>1;tw16(b,table_entry,displaced);
  u32 a=source_base+(index>>2)*row+(index&3u)*stride;u32 s=source_base+(mapped>>2)*row+(mapped&3u)*stride;const u32 saved=a;
  for(u32 n=stride;n!=0;--n){const u8 av=tr8(b,a),sv=tr8(b,s);tw8(b,a,sv);tw8(b,s,av);++a;++s;}
  for(u32 n=0;n<stride;++n)tw8(b,dst+n,tr8(b,saved+n));++index;dst+=stride;--remaining;
 }
 const u16 old_count=tr16(b,base+2u);tw16(b,base+4u,old_count);tw16(b,base+2u,1);const u16 product=static_cast<u16>(static_cast<u16>(old_count)*static_cast<u16>(static_cast<u16>(tr8(b,base+6u))<<3));tw16(b,base,product);
}

static_assert(0xDB80u-0xDAD9u==167u); static_assert(0xDC8Du-0xDB80u==269u); static_assert(0xDDD8u-0xDC90u==328u);
static_assert(0xE4C7u-0xE3B4u==275u); static_assert(0xF007u-0xEDE7u==544u); static_assert(0xEBF3u-0xEA0Bu==488u); static_assert(0xEDD3u-0xEBF3u==480u); static_assert(0xEDE7u-0xEDE0u==7u);
static_assert(0xF110u-0xF007u==265u); static_assert(0xF163u-0xF110u==83u); static_assert(0xF18Cu-0xF163u==41u); static_assert(0xF1B3u-0xF18Cu==39u); static_assert(0xF206u-0xF1B3u==83u); static_assert(0xF2D2u-0xF206u==204u); static_assert(0xF2E9u-0xF2D2u==23u); static_assert(0xF722u-0xF2E9u==1081u); static_assert(0xF742u-0xF722u==32u);
// E0D0..E173: allocate and clone the encoded variable-size object. Source object pointer is the unaligned dword at arg1+5.
// The 28-byte fixed prefix is copied exactly (eleven words, one dword, two bytes); payload length is source[16h]-1 and 6304 receives (dst+1Ch,count,arg2).
struct RuntimeCloneBoundary{u32(*alloc_71c8)(u32)=nullptr;u16(*read16)(u32)=nullptr;u32(*read32)(u32)=nullptr;u8(*read8)(u32)=nullptr;void(*write16)(u32,u16)=nullptr;void(*write32)(u32,u32)=nullptr;void(*write8)(u32,u8)=nullptr;void(*call_6304)(u32,u32,u32)=nullptr;};
[[nodiscard]]u32 function_e0d0(RuntimeCloneBoundary&b,u32 arg1,u32 payload)noexcept{const u32 src=b.read32?b.read32(arg1+5u):0u;const u32 n=b.read32?b.read32(src+0x16u):0u;const u32 dst=b.alloc_71c8?b.alloc_71c8(0x1bu+n):0u;if(b.read16&&b.write16)for(u32 o=0;o<=0x14u;o+=2u)b.write16(dst+o,b.read16(src+o));if(b.read32&&b.write32)b.write32(dst+0x16u,b.read32(src+0x16u));if(b.read8&&b.write8){b.write8(dst+0x1au,b.read8(src+0x1au));b.write8(dst+0x1bu,b.read8(src+0x1bu));}if(b.call_6304)b.call_6304(dst+0x1cu,n-1u,payload);return dst;}
static_assert(0xE174u-0xE0D0u==164u);

// E174..E186, E1F4..E206, E26C..E27E: three address-distinct wrappers with identical encoded bodies.
// Each establishes EBP, forwards its sole dword argument to 7240, discards the pushed argument, and returns EAX unchanged.
struct Runtime7240Boundary{u32(*call_7240)(u32)=nullptr;};
[[nodiscard]]u32 function_e174(Runtime7240Boundary&b,u32 a)noexcept{return b.call_7240?b.call_7240(a):0u;}
[[nodiscard]]u32 function_e1f4(Runtime7240Boundary&b,u32 a)noexcept{return b.call_7240?b.call_7240(a):0u;}
[[nodiscard]]u32 function_e26c(Runtime7240Boundary&b,u32 a)noexcept{return b.call_7240?b.call_7240(a):0u;}
static_assert(0xE187u-0xE174u==19u); static_assert(0xE207u-0xE1F4u==19u); static_assert(0xE27Fu-0xE26Cu==19u);

// E190..E1F0 and E208..E268: address-distinct constructor-like readers. The first stack dword is encoded but unused.
// 6304 is the read wrapper: these bodies request exactly two bytes from source into [EBP-8], zero-extend that word, allocate len+2, store len, set 2904 through 72D0, run 72E0, then call 746C(0,0,dst+2,len,0).
struct RuntimeConstructBoundary{u16(*read_word_6304)(u32)=nullptr;u32(*alloc_71c8)(u32)=nullptr;void(*write16)(u32,u16)=nullptr;void(*call_72d0)(u32)=nullptr;void(*call_72e0)()=nullptr;void(*call_746c)(u32,u32,u32,u32,u32)=nullptr;};
[[nodiscard]]u32 runtime_construct_e190_body(RuntimeConstructBoundary&b,u32 source)noexcept{const u16 len=b.read_word_6304?b.read_word_6304(source):0u;const u32 dst=b.alloc_71c8?b.alloc_71c8(static_cast<u32>(len)+2u):0u;if(b.write16)b.write16(dst,len);if(b.call_72d0)b.call_72d0(source);if(b.call_72e0)b.call_72e0();if(b.call_746c)b.call_746c(0u,0u,dst+2u,static_cast<u32>(len),0u);return dst;}
[[nodiscard]]u32 function_e190(RuntimeConstructBoundary&b,u32 /*unused_arg1*/,u32 source)noexcept{return runtime_construct_e190_body(b,source);}
[[nodiscard]]u32 function_e208(RuntimeConstructBoundary&b,u32 /*unused_arg1*/,u32 source)noexcept{return runtime_construct_e190_body(b,source);}
static_assert(0xE1F1u-0xE190u==97u); static_assert(0xE269u-0xE208u==97u);

// E280..E2C2: install the E2C3 INT 24h trampoline while preserving the caller-supplied far callback at 5102:5106.
// Runtime modes 2..4 use the extender-specific INT 21h AX=2506h, CL=24h form; all other modes use AH=25h, AL=24h. The encoded routine returns EAX=0.
struct Int24InstallBoundary{void(*store_callback)(u32,u16)=nullptr;u8(*runtime_mode)()=nullptr;void(*install_extender_vector)(u8,u32)=nullptr;void(*install_dos_vector)(u8,u32)=nullptr;};
[[nodiscard]]u32 function_e280(Int24InstallBoundary&b,u32 callback_offset,u32 callback_selector)noexcept{if(b.store_callback)b.store_callback(callback_offset,static_cast<u16>(callback_selector));const u8 mode=b.runtime_mode?b.runtime_mode():0u;if(mode>=2u&&mode<=4u){if(b.install_extender_vector)b.install_extender_vector(0x24u,0xE2C3u);}else if(b.install_dos_vector)b.install_dos_vector(0x24u,0xE2C3u);return 0u;}
static_assert(0xE2C3u-0xE280u==67u);

// E360..E3B2: signed decimal parser. Character classes are indexed by unsigned byte+1 at DGROUP+03B4; bit 02h skips leading class, bit 20h accepts digits.
[[nodiscard]]u32 function_e360(const u8*data,u32 text)noexcept{u32 p=text;auto cls=[&](u8 c)noexcept->u8{return data[0x3b4u+static_cast<u32>(static_cast<u8>(c+1u))];};while((cls(data[p])&0x02u)!=0)++p;const u8 sign=data[p];if(sign==0x2bu||sign==0x2du)++p;u32 v=0;while((cls(data[p])&0x20u)!=0){v=v*10u+static_cast<u32>(data[p]);++p;v-=0x30u;}if(sign==0x2du)v=0u-v;return v;}
static_assert(0xE3B3u-0xE360u==83u);

constexpr u32 kVerifiedTranslatedBytes=12645u;
constexpr u32 kCoverageDenominator=kCodeObjectVirtualSize;
} // namespace code1
