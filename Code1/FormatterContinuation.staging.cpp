/* CODE.1 formatter continuation verification staging.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Verified ranges: F206..F2E8 = 227 instruction-accounted bytes.
 * Structural lift under verification: F2E9..F721 = 1081 bytes (NOT counted yet).
 */
#include <cstdint>
#include <cstring>
namespace code1_formatter_continuation {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t; using s32=std::int32_t;
struct FormatState{u8 opaque0[4];u32 width;u32 precision;u32 zero_count;u8 opaque10[4];u8 flags;u8 conversion;u8 pad;u8 prefix[3];};
static_assert(sizeof(FormatState)==0x1c);
extern char* function_dffb(s32,char*,u32)noexcept;
extern char* function_e050(u32,char*,u32)noexcept;
extern u32 function_d991(u32)noexcept;

// F206..F2D1 (204): fixed-point decimal materialization and encoded rounding pass.
void function_f206(char*out,s32 value,FormatState&s)noexcept{
 char*p=out; s32 magnitude=value; if(magnitude<0){magnitude=-magnitude;*p++='-';}
 if(s.precision==0xffffffffu)s.precision=4;
 (void)function_dffb(static_cast<s32>(static_cast<u16>(magnitude)),p,10);
 char*integer_begin=p; while(*p)++p;
 if(s.precision!=0){*p++='.';for(u32 i=0;i<s.precision;++i){magnitude*=10;*p++=static_cast<char>(static_cast<u8>(static_cast<u16>(magnitude)>>8)+static_cast<u8>('0'));}*p='\0';}
 if((static_cast<u16>(magnitude)>>8)&0x80u){if(p==integer_begin){*integer_begin='1';}else{--p;if(*p=='.')--p;for(;;){if(*p!='9'){++*p;break;}*p='0';if(p==integer_begin){*p='1';break;}--p;if(*p=='.')--p;}}}
}

// Runtime-address boundary used by F2E9. All original 32-bit cursor and selector
// values stay guest addresses; no guest address is reinterpreted as a host pointer.
struct FormatterBoundary{
 u32(*call_55c4)(u32 scratch,u32 cursor_cell,u32 state)=nullptr;
 u8(*read_linear8)(u32)=nullptr; u16(*read_linear16)(u32)=nullptr; u32(*read_linear32)(u32)=nullptr;
 void(*write_linear32)(u32,u32)=nullptr;
 u8(*read_selector8)(u16,u32)=nullptr; u16(*read_selector16)(u16,u32)=nullptr;
};
[[nodiscard]]u32 function_f2d2(FormatterBoundary&b,u32 scratch,u32 cursor_cell,u32 state)noexcept{return b.call_55c4?b.call_55c4(scratch,cursor_cell,state):0;}
static u32 get32(FormatterBoundary&b,u32 a)noexcept{return b.read_linear32?b.read_linear32(a):0;}
static void put32(FormatterBoundary&b,u32 a,u32 v)noexcept{if(b.write_linear32)b.write_linear32(a,v);}
static u32 take32(FormatterBoundary&b,u32 cursor_cell)noexcept{const u32 next=get32(b,cursor_cell)+4u;put32(b,cursor_cell,next);return get32(b,next-4u);}
static u16 take16_promoted(FormatterBoundary&b,u32 cursor_cell)noexcept{return static_cast<u16>(take32(b,cursor_cell));}
static u32 bounded8(FormatterBoundary&b,u32 p,u16 sel,u32 lim)noexcept{u32 n=0;while(n!=lim&&b.read_selector8&&b.read_selector8(sel,p+n)!=0)++n;return n;}
static u32 bounded16(FormatterBoundary&b,u32 p,u16 sel,u32 lim)noexcept{u32 n=0;while(n!=lim&&b.read_selector16&&b.read_selector16(sel,p+n*2u)!=0)++n;return n;}
static u32 cstrlen(const char*p)noexcept{return static_cast<u32>(std::strlen(p));}
static void uppercase(char*p)noexcept{while(*p){*p=static_cast<char>(function_d991(static_cast<u8>(*p)));++p;}}
static void hexpad(u32 value,char*out,u32 width)noexcept{char tmp[16]{};(void)function_dffb(static_cast<s32>(value),tmp,16);const u32 n=cstrlen(tmp);u32 i=0;for(;i+n<width;++i)out[i]='0';for(u32 j=0;j<n&&i<width;++j)out[i++]=tmp[j];out[width]='\0';}

struct MaterializeResult{u32 source_offset=0;u16 source_selector=0;u32 length=0;bool local_source=true;};

// F2E9..F721 (1081): structural lift under byte-for-byte review.
// The encoded arguments are scratch offset, ADDRESS OF the argument cursor cell,
// state offset, and fallback-string offset. Guest addresses remain guest addresses.
[[nodiscard]]MaterializeResult function_f2e9(FormatterBoundary&b,char*scratch,u32 scratch_guest,u32 cursor_cell,FormatState&s,u32 state_guest,u16 ds,u32 null_string_offset)noexcept{
 MaterializeResult r{};r.source_selector=ds;r.local_source=true;u32 numeric=0;bool signed_family=false,numeric_family=false;u32 radix=10;const u8 conv=s.conversion;
 if(conv=='X'||conv=='d'||conv=='i'||conv=='o'||conv=='u'||conv=='x'){
  if(s.flags&0x20u)numeric=take32(b,cursor_cell);else if(s.flags&0x10u)numeric=take16_promoted(b,cursor_cell);else numeric=take32(b,cursor_cell);
  s.flags=static_cast<u8>(s.flags&0xf9u);
 }
 if(s.precision!=0xffffffffu&&(conv=='d'||conv=='i'||conv=='o'||conv=='u'||conv=='x'||conv=='X'))s.pad=' ';
 switch(conv){
 case 'E':case 'F':case 'G':case 'e':case 'f':case 'g':
  if(s.flags&0x10u){function_f206(scratch,static_cast<s32>(take32(b,cursor_cell)),s);r.length=cstrlen(scratch);}
  else{(void)function_f2d2(b,scratch_guest,cursor_cell,state_guest);r.length=cstrlen(scratch);}break;
 case 'o':radix=8;if(s.flags&1u)*scratch='0';numeric_family=true;break;
 case 'd':case 'i':signed_family=true;numeric_family=true;radix=10;break;
 case 'u':numeric_family=true;radix=10;break;
 case 'x':case 'X':if((s.flags&1u)&&numeric!=0){s.prefix[0]='0';s.prefix[1]=conv;s.prefix[2]=0;}radix=16;numeric_family=true;break;
 case 's':case 'S':{
  u32 p=0;u16 sel=ds;
  if(s.flags&0x80u){const u32 cursor=get32(b,cursor_cell)+8u;put32(b,cursor_cell,cursor);p=get32(b,cursor-8u);sel=b.read_linear16?b.read_linear16(cursor-4u):0;if(p==0&&sel==0){p=null_string_offset;sel=ds;}}
  else{p=take32(b,cursor_cell);if(p==0)p=null_string_offset;}
  s.flags=static_cast<u8>(s.flags&0xf9u);r.source_offset=p;r.source_selector=sel;r.local_source=false;
  r.length=(conv=='S'&&(s.flags&0x20u))?bounded16(b,p,sel,s.precision):bounded8(b,p,sel,s.precision);break;}
 case 'p':case 'P':{
  if(s.width==0)s.width=(s.flags&0x80u)?13u:8u;s.flags=static_cast<u8>(s.flags&0xf9u);const u32 off=take32(b,cursor_cell);
  if(s.flags&0x80u){const u16 sel=take16_promoted(b,cursor_cell);hexpad(sel,scratch,4);scratch[4]=':';hexpad(off,scratch+5,8);}else hexpad(off,scratch,8);
  if(conv=='P')uppercase(scratch);r.length=cstrlen(scratch);break;}
 case 'c':scratch[0]=static_cast<char>(take32(b,cursor_cell));scratch[1]=0;s.precision=1;s.flags=static_cast<u8>(s.flags&0xf9u);r.length=1;break;
 default:s.width=0;scratch[0]=static_cast<char>(conv);scratch[1]=0;s.precision=1;s.flags=static_cast<u8>(s.flags&0xf9u);r.length=1;break;
 }
 if(numeric_family){
  if(signed_family)(void)function_dffb(static_cast<s32>(numeric),scratch,radix);else (void)function_e050(numeric,scratch,radix);
  if(conv=='X')uppercase(scratch);r.length=cstrlen(scratch);
  // F493..F4A4: precision zero suppresses a single generated zero by writing NUL
  // through ES:EDI; the source pointer itself is retained.
  if(s.precision==0&&scratch[0]=='0'){scratch[0]=0;r.length=0;}
 }
 if(r.local_source){r.source_offset=scratch_guest;r.source_selector=ds;if(r.length==0)r.length=cstrlen(scratch);}

 // F695..F6D7: sign/prefix and numeric precision accounting. The original EAX
 // carries the source length into this block. A leading '-' is removed from the
 // returned source span and moved into state.prefix; otherwise blank/plus flags
 // may synthesize a one-byte prefix. zero_count is precision-length when needed.
 if(numeric_family||conv=='E'||conv=='F'||conv=='G'||conv=='e'||conv=='f'||conv=='g'){
  if(r.length!=0&&b.read_selector8){const u8 first=r.local_source?static_cast<u8>(scratch[0]):b.read_selector8(r.source_selector,r.source_offset);if(first=='-'){--r.length;s.prefix[0]='-';s.prefix[1]=0;++r.source_offset;}else if(s.flags&0x02u){s.prefix[0]=' ';s.prefix[1]=0;}else if(s.flags&0x04u){s.prefix[0]='+';s.prefix[1]=0;}}
  if(r.length>s.precision)s.precision=r.length;else s.zero_count=s.precision-r.length;
 }

 // F6D8..F6F9: '*' padding cancels sign/prefix flags. For every conversion except
 // 'c', an unset precision or a precision greater than the actual source length is
 // replaced with that actual length. This is exactly the branch structure encoded.
 if(s.pad=='*'){s.prefix[0]=0;s.flags=static_cast<u8>(s.flags&0xf9u);}
 if((s.precision==0xffffffffu||r.length<s.precision)&&conv!='c')s.precision=r.length;

 // F6FA..F721: strlen(prefix) + precision + zero_count is subtracted from width;
 // EDI and the saved selector are the function's returned source pair.
 const u32 prefix_len=cstrlen(reinterpret_cast<const char*>(s.prefix));
 const u32 occupied=prefix_len+s.precision+s.zero_count;
 s.width-=occupied;
 return r;
}

static_assert(0xF2D2u-0xF206u==204u);
static_assert(0xF2E9u-0xF2D2u==23u);
static_assert(0xF722u-0xF2E9u==1081u);
constexpr u32 kVerifiedStagedBytes=227u;
constexpr u32 kStructuralUncountedBytes=1081u;
} // namespace code1_formatter_continuation
