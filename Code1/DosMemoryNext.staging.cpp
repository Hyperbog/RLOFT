/* CODE.1 DA1C..DAD8 DOS/runtime-memory continuation staging.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Exact instruction-bounded range: DA1C..DAD8 = 189 bytes. NOT COUNTED.
 * This directly closes the gap between the integrated D8D0..DA1B helpers
 * and the integrated DAD9 allocator wrapper.
 */
#include <cstdint>
namespace code1_dos_memory_next {
using u16=std::uint16_t;using u32=std::uint32_t;

struct Boundary{
 /* 104B1 initializes the 12-byte FEFC far-cell/register adapter object. */
 void(*call_104b1)(u32 guest_address)=nullptr;
 /* CEC8 exact four-dword adapter: ESI,EDI,EDX,EBX. */
 u32(*call_cec8)(u32 esi,u32 edi,u32 edx,u32 ebx)=nullptr;
 /* DOS AH=48/49/4A boundaries preserve carry/error behavior explicitly. */
 struct DosResult{u16 ax=0;u16 bx=0;bool carry=false;};
 DosResult(*dos_48_alloc)(u16 paragraphs)=nullptr;
 DosResult(*dos_49_free)(u16 selector)=nullptr;
 DosResult(*dos_4a_resize)(u16 paragraphs,u16 selector)=nullptr;
 u32(*map_dos_error_10384)(u16 ax)=nullptr;
 u32(*call_dad9)(u32 request)=nullptr;
 u32(*read32)(u32 address)=nullptr;
};

/* DA1C..DA44 (41 bytes). 104B1 prepares the 12-byte local far-cell and CEC8
 * receives stack args service/input/output plus that local as its fourth arg.
 * CEC8 itself returns dword [third argument] after FEFC. */
[[nodiscard]]u32 function_da1c(Boundary&b,u32 service,u32 input,u32 output,u32 local_guest)noexcept{
 if(b.call_104b1)b.call_104b1(local_guest);
 return b.call_cec8?b.call_cec8(service,input,output,local_guest):0;
}

/* DA45..DA47 are three zero bytes and are data/alignment, not part of DA1C. */

/* DA48..DA6B (36 bytes): DOS AH=48 allocate-memory adapter. The requested
 * paragraph count is the low word of arg1. On success EDX receives returned
 * AX and EAX becomes zero; on carry, 10384 maps the DOS error in AX. In both
 * cases the resulting EDX value is written through arg2. */
[[nodiscard]]u32 function_da48(Boundary&b,u32 paragraphs,u32&out_value)noexcept{
 const auto r=b.dos_48_alloc?b.dos_48_alloc(u16(paragraphs)):Boundary::DosResult{};
 u32 eax=0;u32 edx=paragraphs; // encoded XCHG EDX,EBX makes old EBX available on failure
 if(!r.carry){edx=r.ax;eax=0;}else{eax=b.map_dos_error_10384?b.map_dos_error_10384(r.ax):r.ax;}
 out_value=edx;return eax;
}

/* DA6C..DA7D (18 bytes): ES=arg1 low word, AH=49, INT 21h, then 10384 is
 * called unconditionally so the runtime helper determines the final status
 * from DOS carry/AX state. Boundary keeps that status contract explicit. */
[[nodiscard]]u32 function_da6c(Boundary&b,u32 selector)noexcept{
 const auto r=b.dos_49_free?b.dos_49_free(u16(selector)):Boundary::DosResult{};
 return b.map_dos_error_10384?b.map_dos_error_10384(r.ax):r.ax;
}

/* DA7E..DAA5 (40 bytes): DOS AH=4A resize. EAX supplies paragraph count,
 * EDX supplies selector for ES, and EBX is the output pointer. The encoded
 * routine writes the paragraph count to *out only on carry, then calls 10384
 * unconditionally. */
[[nodiscard]]u32 function_da7e(Boundary&b,u32 paragraphs,u32 selector,u32&out_value)noexcept{
 const auto r=b.dos_4a_resize?b.dos_4a_resize(u16(paragraphs),u16(selector)):Boundary::DosResult{};
 if(r.carry)out_value=paragraphs;
 return b.map_dos_error_10384?b.map_dos_error_10384(r.ax):r.ax;
}

/* DAA6..DAC8 (35 bytes): scan the linked arena list rooted at [30DC] until
 * sentinel 30D4. Each node contributes max(dword[node]-4); next=node+8. */
[[nodiscard]]u32 function_daa6(Boundary&b)noexcept{
 if(!b.read32)return 0;u32 node=b.read32(0x30dcu),best=0;
 while(node!=0x30d4u){const u32 candidate=b.read32(node)-4u;if(candidate>best)best=candidate;node=b.read32(node+8u);}return best;
}

/* DAC9..DAD8 (16 bytes): ordinary stack wrapper around already integrated
 * DAD9 allocator entry. */
[[nodiscard]]u32 function_dac9(Boundary&b,u32 request)noexcept{return b.call_dad9?b.call_dad9(request):0;}

static_assert(0xDA45u-0xDA1Cu==41u);
static_assert(0xDA6Cu-0xDA48u==36u);
static_assert(0xDA7Eu-0xDA6Cu==18u);
static_assert(0xDAA6u-0xDA7Eu==40u);
static_assert(0xDAC9u-0xDAA6u==35u);
static_assert(0xDAD9u-0xDAC9u==16u);
constexpr u32 kInstructionBoundedStagedBytes=186u; // excludes DA45..DA47 zero data
} // namespace code1_dos_memory_next
