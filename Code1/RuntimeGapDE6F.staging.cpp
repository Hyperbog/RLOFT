/* CODE.1 DE6F..E0C7 runtime helper staging.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * 521 functional instruction bytes. NOT COUNTED.
 * Data/alignment excluded: DEA1..DEA6 NOP padding; DF8E..DFB2 and E02B..E04F digit tables.
 */
#include <cstdint>
namespace code1_runtime_de6f {
using u8=std::uint8_t; using u32=std::uint32_t; using s32=std::int32_t;
struct Boundary {
 u8(*in8)(u32 port)=nullptr;
 void(*out8)(u32 port,u8 value)=nullptr;
};

/* DE6F..DEA0 (50 bytes): byte-count fill front-end. EAX is destination,
 * EDX contains the repeated fill pattern and ECX is byte count. The encoded
 * path aligns EAX to a dword boundary by rotating EDX one byte per leading
 * store, calls DEA7 for whole dwords, then emits the final 0..3 bytes. */
void fill_bytes(u8* p,u32 pattern,u32 count) noexcept {
 while(count && (reinterpret_cast<std::uintptr_t>(p)&3u)){*p++=u8(pattern);pattern=(pattern>>8)|(pattern<<24);--count;}
 const u32 dwords=count>>2; if(dwords){u32* q=reinterpret_cast<u32*>(p);for(u32 i=0;i<dwords;++i)q[i]=pattern;p+=dwords*4u;}
 count&=3u; if(count)*p=u8(pattern); if(count>1)p[1]=u8(pattern>>8); if(count>2)p[2]=u8(pattern);
}

/* DEA7..DF12 (108 bytes): optimized dword fill. The original performs an
 * unrolled 32-byte loop after aligning EAX to 32 bytes; this lift preserves
 * the exact externally visible sequence/value while keeping the byte range
 * and alignment behavior documented rather than replacing it with memset. */
void fill_dwords(u32* p,u32 value,u32 count) noexcept { while(count--)*p++=value; }

/* DF13..DF72 (96 bytes): PIT channel-2 divisor programming. Values <= 0x12
 * are ignored. Otherwise divisor=0x1234dc/arg; port 61h is read and, when its
 * low two bits are not already 3, enabled and command B6h is written to 43h.
 * Divisor low then high byte are written to 42h. */
void function_df13(Boundary& b,u32 divisor_arg) noexcept {
 if(divisor_arg<=0x12u)return; const u32 divisor=0x1234dcu/divisor_arg;
 u8 p61=b.in8?b.in8(0x61u):0; if((p61&3u)!=3u){p61|=3u;if(b.out8){b.out8(0x61u,p61);b.out8(0x43u,0xb6u);}}
 if(b.out8){b.out8(0x42u,u8(divisor));b.out8(0x42u,u8(divisor>>8));}
}

/* DF73..DF8D (27 bytes): clear PIT/speaker enable bits 0 and 1 at port 61h. */
void function_df73(Boundary& b) noexcept {const u8 v=b.in8?b.in8(0x61u):0;if(b.out8)b.out8(0x61u,u8(v&0xfcu));}

static constexpr char kDigits[]="0123456789abcdefghijklmnopqrstuvwxyz";

/* DFB3..DFFA (72 bytes): unsigned integer-to-string. The machine routine
 * builds digits backward in a 40-byte local scratch area, then copies them
 * forward including the terminating NUL. Base is supplied by the caller. */
char* function_dfb3(u32 value,char* out,u32 base) noexcept {
 char scratch[40]{}; char* q=scratch+1; do{const u32 r=value%base;value/=base;*q++=kDigits[r];}while(value); --q;
 char* ret=out; do{*out=*q--; }while(*out++!='\0'); return ret;
}

/* DFFB..E02A (48 bytes): signed front-end. Only base 10 emits a leading '-'
 * and negates a negative input; all other bases pass the 32-bit bit-pattern
 * unchanged to DFB3. */
char* function_dffb(s32 value,char* out,u32 base) noexcept {char* digits=out;u32 v=u32(value);if(base==10u&&value<0){v=u32(-value);*out='-';digits=out+1;}function_dfb3(v,digits,base);return out;}

/* E050..E097 (72 bytes): second unsigned conversion implementation. It keeps
 * the current remainder in a local dword and indexes the separate E02B digit
 * table; externally it has the same value/base/string contract as DFB3. */
char* function_e050(u32 value,char* out,u32 base) noexcept {return function_dfb3(value,out,base);}

/* E098..E0C7 (48 bytes): signed front-end for E050, with the same decimal-only
 * sign rule as DFFB. */
char* function_e098(s32 value,char* out,u32 base) noexcept {char* digits=out;u32 v=u32(value);if(base==10u&&value<0){v=u32(-value);*out='-';digits=out+1;}function_e050(v,digits,base);return out;}

static_assert(0xDEA1u-0xDE6Fu==50u); static_assert(0xDF13u-0xDEA7u==108u);
static_assert(0xDF73u-0xDF13u==96u); static_assert(0xDF8Eu-0xDF73u==27u);
static_assert(0xDFFBu-0xDFB3u==72u); static_assert(0xE02Bu-0xDFFBu==48u);
static_assert(0xE098u-0xE050u==72u); static_assert(0xE0C8u-0xE098u==48u);
constexpr u32 kInstructionBoundedStagedBytes=521u;
} // namespace code1_runtime_de6f
