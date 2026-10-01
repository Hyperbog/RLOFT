/* CODE.1 DA1C..DAD8 DOS/runtime-memory continuation staging.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Exact functional code: DA1C..DA44, DA48..DAD8 = 186 bytes. NOT COUNTED.
 * DA45..DA47 = three zero bytes classified as data/alignment.
 */
#include <cstdint>
namespace code1_dos_memory_next {
using u16=std::uint16_t;using u32=std::uint32_t;
struct Boundary{
 void(*call_104b1)(u32 guest_address)=nullptr;
 u32(*call_cec8)(u32 esi,u32 edi,u32 edx,u32 ebx)=nullptr;
 struct DosResult{u16 ax=0;u16 bx=0;bool carry=false;};
 DosResult(*dos_48_alloc)(u16 paragraphs)=nullptr;
 DosResult(*dos_49_free)(u16 selector)=nullptr;
 DosResult(*dos_4a_resize)(u16 paragraphs,u16 selector)=nullptr;
 /* 10384 branches immediately on the carry flag left by INT 21h; carry is
  * therefore part of the call contract and must not be discarded. */
 u32(*call_10384)(u16 ax,bool carry)=nullptr;
 u32(*call_dad9)(u32 request)=nullptr;
 u32(*read32)(u32 address)=nullptr;
};
/* DA1C..DA44: initialize 12-byte adapter cell then call CEC8(service,input,
 * output,cell). */
[[nodiscard]]u32 function_da1c(Boundary&b,u32 service,u32 input,u32 output,u32 local_guest)noexcept{
 if(b.call_104b1)b.call_104b1(local_guest);return b.call_cec8?b.call_cec8(service,input,output,local_guest):0;
}
/* DA48..DA6B: AH=48. EBX receives the requested paragraph count before the
 * interrupt. Immediately afterward XCHG EDX,EBX makes the DOS-returned BX
 * value become EDX and restores the output pointer to EBX. On success EDX is
 * replaced by returned AX and EAX is zero. On carry, DOS BX is the largest
 * available block and that exact value is written through arg2; 10384 maps
 * AX while CF remains set. */
[[nodiscard]]u32 function_da48(Boundary&b,u32 paragraphs,u32&out_value)noexcept{
 const auto r=b.dos_48_alloc?b.dos_48_alloc(u16(paragraphs)):Boundary::DosResult{};
 u32 eax=0;u32 edx=r.bx;
 if(!r.carry)edx=r.ax;else eax=b.call_10384?b.call_10384(r.ax,true):r.ax;
 out_value=edx;return eax;
}
/* DA6C..DA7D: AH=49 then unconditional CALL 10384 while INT 21h CF is still
 * live. 10384 returns zero when CF=0 and maps low AX when CF=1. */
[[nodiscard]]u32 function_da6c(Boundary&b,u32 selector)noexcept{
 const auto r=b.dos_49_free?b.dos_49_free(u16(selector)):Boundary::DosResult{};return b.call_10384?b.call_10384(r.ax,r.carry):(r.carry?r.ax:0u);
}
/* DA7E..DAA5: AH=4A. EBX is loaded with the requested paragraph count for
 * INT 21h. MOV EDX,EBX captures DOS's returned BX before the saved output
 * pointer is restored. Therefore the carry path stores returned BX (largest
 * possible block), not the original request. 10384 then consumes AX+CF. */
[[nodiscard]]u32 function_da7e(Boundary&b,u32 paragraphs,u32 selector,u32&out_value)noexcept{
 const auto r=b.dos_4a_resize?b.dos_4a_resize(u16(paragraphs),u16(selector)):Boundary::DosResult{};
 if(r.carry)out_value=r.bx;
 return b.call_10384?b.call_10384(r.ax,r.carry):(r.carry?r.ax:0u);
}
/* DAA6..DAC8: scan arena chain [30DC] to sentinel 30D4. Unsigned SUB by four
 * is preserved: a stored size below four wraps before the unsigned comparison. */
[[nodiscard]]u32 function_daa6(Boundary&b)noexcept{if(!b.read32)return 0;u32 node=b.read32(0x30dcu),best=0;while(node!=0x30d4u){const u32 candidate=b.read32(node)-4u;if(candidate>best)best=candidate;node=b.read32(node+8u);}return best;}
/* DAC9..DAD8: ordinary wrapper around integrated DAD9. */
[[nodiscard]]u32 function_dac9(Boundary&b,u32 request)noexcept{return b.call_dad9?b.call_dad9(request):0;}
static_assert(0xDA45u-0xDA1Cu==41u);static_assert(0xDA6Cu-0xDA48u==36u);static_assert(0xDA7Eu-0xDA6Cu==18u);static_assert(0xDAA6u-0xDA7Eu==40u);static_assert(0xDAC9u-0xDAA6u==35u);static_assert(0xDAD9u-0xDAC9u==16u);
constexpr u32 kInstructionBoundedStagedBytes=186u;
} // namespace code1_dos_memory_next
