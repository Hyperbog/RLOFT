/* CODE.1 formatter continuation verification staging.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Ranges: F206..F2D1 and F2D2..F2E8 = 227 instruction-accounted bytes.
 */
#include <cstdint>
#include <cstring>
namespace code1_formatter_continuation {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t; using s32=std::int32_t;
struct FormatState{u8 opaque0[4];u32 width;u32 precision;u8 opaque0c[8];u8 flags;u8 opaque15;u8 pad;u8 opaque17;};
static_assert(sizeof(FormatState)==0x18);
extern char* function_dffb(s32,char*,u32)noexcept;

// F206..F2D1 (204): fixed-point decimal materialization and encoded rounding pass.
// The source uses the low word of the incoming signed value for the integer conversion,
// then repeatedly multiplies the retained magnitude by ten while emitting precision digits.
void function_f206(char*out,s32 value,FormatState&s)noexcept{
 char*p=out; s32 magnitude=value; if(magnitude<0){magnitude=-magnitude;*p++='-';}
 if(s.precision==0xffffffffu)s.precision=4;
 (void)function_dffb(static_cast<s32>(static_cast<u16>(magnitude)),p,10);
 char*integer_begin=p; while(*p)++p;
 if(s.precision!=0){*p++='.';for(u32 i=0;i<s.precision;++i){magnitude*=10;*p++=static_cast<char>(static_cast<u8>(static_cast<u16>(magnitude)>>8)+static_cast<u8>('0'));}*p='\0';}
 // CODE.1 tests bit 15 of the retained high word as the rounding condition.
 if((static_cast<u16>(magnitude)>>8)&0x80u){
  if(p==integer_begin){*integer_begin='1';p=integer_begin+1;}
  else{--p;if(*p=='.')--p;for(;;){if(*p!='9'){++*p;break;}*p='0';if(p==integer_begin){*p='1';break;}--p;if(*p=='.')--p;}}
 }
}

// F2D2..F2E8 (23): exact three-argument indirect thunk through DGROUP 55C4h.
struct FormatterBoundary{u32(*call_55c4)(u32,u32,u32)=nullptr;};
[[nodiscard]]u32 function_f2d2(FormatterBoundary&b,u32 a,u32 c,u32 d)noexcept{return b.call_55c4?b.call_55c4(a,c,d):0;}

static_assert(0xF2D2u-0xF206u==204u);
static_assert(0xF2E9u-0xF2D2u==23u);
constexpr u32 kStagedInstructionAccountedBytes=227u;
} // namespace code1_formatter_continuation
