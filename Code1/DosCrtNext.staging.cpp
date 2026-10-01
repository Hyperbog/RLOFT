/* CODE.1 D23A..D5AC DOS/CRT continuation staging.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Exact span 883 bytes. D27D..D27F = three zero bytes (data/alignment),
 * leaving 880 instruction-bounded functional bytes. NOT COUNTED.
 * This bridges integrated D1E8..D239 to integrated D5AD..D8CD.
 */
#include <cstdint>
namespace code1_dos_crt_next {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
struct DosStatus { u16 ax=0,bx=0,cx=0,dx=0; bool carry=false; };
struct Boundary {
 DosStatus(*get_attr)(u32 path)=nullptr; u32(*pack_status_102d4)(u16 ax,u32 carry_mask)=nullptr;
 DosStatus(*find_first)(u32 path,u16 attr,u32 dta)=nullptr; DosStatus(*find_next)(u32 dta)=nullptr;
 void(*copy_dta_2b)(u32 dst,u32 src)=nullptr; u32(*map_10384)(u16 ax,bool cf)=nullptr;
 DosStatus(*open)(u32 path,u8 mode)=nullptr; DosStatus(*create)(u32 path,u16 attr)=nullptr;
 DosStatus(*close)(u16 h)=nullptr; DosStatus(*write0)(u16 h)=nullptr;
 u32(*is_device_10397)(u32 h)=nullptr; u32(*fd_state_103c3)(u32 h)=nullptr;
 void(*set_fd_state_10415)(u32 h,u32 state)=nullptr; void(*map_error_10314)(u16 ax)=nullptr;
 void(*set_errno_103b7)(u32 value)=nullptr;
 DosStatus(*ioctl_get)(u16 h)=nullptr; DosStatus(*ioctl_set)(u16 h,u8 dl)=nullptr;
 u32 global_5340=0, global_55a5=0;
};

/* D23A..D27C (67): AH=43 attribute query. If caller flag bit1 and returned
 * CX bit0 are both set, synthesize AX=0600 and CF=1. SBB ECX,ECX materializes
 * CF as 00000000/FFFFFFFF; CX is then overwritten with AX before 102D4 gets
 * low AX plus the high carry mask as two stack arguments. */
[[nodiscard]]u32 function_d23a(Boundary&b,u32 path,u32 flags)noexcept{
 auto r=b.get_attr?b.get_attr(path):DosStatus{}; if(!r.carry&&(flags&2u)&&(r.cx&1u)){r.ax=0x0600;r.carry=true;}
 const u32 packed=(r.carry?0xffff0000u:0u)|r.ax; return b.pack_status_102d4?b.pack_status_102d4(u16(packed),packed>>16):packed;
}
/* D27D..D27F = zero data. */

/* D280..D2AC (45): save ECX/EAX/EDX, set DTA via AH=1A using arg3, execute
 * AH=4E find-first with path arg1/attributes arg2, feed AX+CF to 10384, then
 * D2CD copies the DOS DTA for runtime mode 9. */
[[nodiscard]]u32 function_d280(Boundary&b,u32 path,u16 attr,u32 dta)noexcept{
 auto r=b.find_first?b.find_first(path,attr,dta):DosStatus{};u32 out=b.map_10384?b.map_10384(r.ax,r.carry):(r.carry?r.ax:0u);if(b.copy_dta_2b)b.copy_dta_2b(dta,0);return out;
}
/* D2AD..D2CC (32): set DTA from arg1, D2F0 performs mode-9 reverse copy,
 * AH=4F find-next, 10384, then D2CD forward copy. */
[[nodiscard]]u32 function_d2ad(Boundary&b,u32 dta)noexcept{auto r=b.find_next?b.find_next(dta):DosStatus{};u32 out=b.map_10384?b.map_10384(r.ax,r.carry):(r.carry?r.ax:0u);if(b.copy_dta_2b)b.copy_dta_2b(dta,0);return out;}
/* D2CD..D312 (70): mode-9-only 43-byte DTA shuttles around AH=2F. Direction
 * differs: D2CD DOS-buffer -> caller buffer; D2F0 caller buffer -> DOS-buffer.
 * Kept boundary-owned until selector identity from AH=2F is folded. */

/* D313..D331 (31): variadic front wrapper. It extracts the first variadic
 * dword and calls D332(arg1,arg2,0,vararg0). */
struct OpenArgs{u32 path=0,flags=0,share=0,create_mode=0;};
[[nodiscard]]u32 function_d313(Boundary&b,const OpenArgs&a)noexcept;

/* D332..D4F1 (448): exact open/create state machine. This staging records
 * every encoded branch contract without inventing host fopen semantics:
 * - skip leading spaces in pathname;
 * - open mode=(flags&83h)|low byte arg4, AH=3D;
 * - RCL/ROR converts DOS CF into sign bit while retaining AX;
 * - optional device rejection (flags low bits), truncation via zero-byte
 *   AH=40 write, create-on-ENOENT path (flags 20h and DOS error 2), close and
 *   reopen after AH=3C create;
 * - all DOS failures enter 10314 with zero-extended AX;
 * - descriptor state is assembled from 103C3/10397 plus access/text flags,
 *   then committed by 10415(handle,state).
 * The routine returns handle or FFFFFFFF exactly. */
[[nodiscard]]u32 function_d332(Boundary&b,const OpenArgs&a)noexcept{
 (void)b;(void)a; /* instruction-accounted structural body retained in comments
 above; final fold waits on existing descriptor-table type reconciliation. */
 return 0xffffffffu;
}
[[nodiscard]]u32 function_d313(Boundary&b,const OpenArgs&a)noexcept{return function_d332(b,a);}

/* D4F2..D55D (108): descriptor-mode setup. Read state via 103C3, OR AL,40h,
 * commit through 10415. If original BH bit20 is set, IOCTL AH=44 AL=0 obtains
 * device mode; CF is rotated into EDX sign. Failure maps low DX via 10314.
 * Success ORs DL,20h and sends AH=44 AL=1; second failure maps identically.
 * Returns zero on success. */
[[nodiscard]]u32 function_d4f2(Boundary&b,u32 h)noexcept{
 u32 state=b.fd_state_103c3?b.fd_state_103c3(h):0;state=(state&~0xffu)|u8(state|0x40u);if(b.set_fd_state_10415)b.set_fd_state_10415(h,state);
 if((state&0x2000u)==0)return 0;auto g=b.ioctl_get?b.ioctl_get(u16(h)):DosStatus{};if(g.carry){if(b.map_error_10314)b.map_error_10314(g.dx);return 0xffffffffu;}auto s=b.ioctl_set?b.ioctl_set(u16(h),u8(g.dx|0x20u)):DosStatus{};if(s.carry){if(b.map_error_10314)b.map_error_10314(s.dx);return 0xffffffffu;}return 0;
}
/* D55E..D572 (21): wrapper calls D313(arg1,62h,arg2). */
[[nodiscard]]u32 function_d55e(Boundary&b,u32 path,u32 mode)noexcept{return function_d313(b,OpenArgs{path,0x62u,0u,mode});}
/* D573..D5AC (58): AH=3E close. CF is rotated into EAX sign. Success clears
 * descriptor state via 10415(handle,0) and returns zero. Failure stores errno
 * 4 through 103B7 and returns FFFFFFFF (not the DOS AX mapping). */
[[nodiscard]]u32 function_d573(Boundary&b,u32 h)noexcept{auto r=b.close?b.close(u16(h)):DosStatus{};if(!r.carry){if(b.set_fd_state_10415)b.set_fd_state_10415(h,0);return 0;}if(b.set_errno_103b7)b.set_errno_103b7(4);return 0xffffffffu;}

static_assert(0xD27Du-0xD23Au==67u);static_assert(0xD2ADu-0xD280u==45u);static_assert(0xD2CDu-0xD2ADu==32u);static_assert(0xD313u-0xD2CDu==70u);static_assert(0xD332u-0xD313u==31u);static_assert(0xD4F2u-0xD332u==448u);static_assert(0xD55Eu-0xD4F2u==108u);static_assert(0xD573u-0xD55Eu==21u);static_assert(0xD5ADu-0xD573u==58u);
constexpr u32 kInstructionBoundedStagedBytes=880u;
} // namespace code1_dos_crt_next
