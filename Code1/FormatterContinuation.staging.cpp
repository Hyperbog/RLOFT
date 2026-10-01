/* CODE.1 formatter continuation verification staging.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Ranges: F206..F721 = 1308 instruction-accounted bytes.
 */
#include <cstdint>
#include <cstring>
namespace code1_formatter_continuation {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t; using s16=std::int16_t; using s32=std::int32_t;
struct FormatState{u8 opaque0[4];u32 width;u32 precision;u8 opaque0c[8];u8 flags;u8 conversion;u8 pad;u8 prefix[3];};
static_assert(sizeof(FormatState)==0x1a);
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

// F2D2..F2E8 (23): exact three-argument indirect thunk through DGROUP 55C4h.
struct FormatterBoundary{
 u32(*call_55c4)(u32,u32,u32)=nullptr;
 u8(*read8)(u16,u32)=nullptr; u16(*read16)(u16,u32)=nullptr; void(*write8)(u16,u32,u8)=nullptr;
};
[[nodiscard]]u32 function_f2d2(FormatterBoundary&b,u32 a,u32 c,u32 d)noexcept{return b.call_55c4?b.call_55c4(a,c,d):0;}
static u32 bounded8(FormatterBoundary&b,u32 p,u16 sel,u32 lim)noexcept{u32 n=0;while(n!=lim&&b.read8&&b.read8(sel,p+n)!=0)++n;return n;}
static u32 bounded16(FormatterBoundary&b,u32 p,u16 sel,u32 lim)noexcept{u32 n=0;while(n!=lim&&b.read16&&b.read16(sel,p+n*2u)!=0)++n;return n;}
static u32 cstrlen(const char*p)noexcept{return static_cast<u32>(std::strlen(p));}
static void uppercase(char*p)noexcept{while(*p){*p=static_cast<char>(function_d991(static_cast<u8>(*p)));++p;}}
static void hexpad(u32 value,char*out,u32 width)noexcept{char tmp[16]{};(void)function_dffb(static_cast<s32>(value),tmp,16);const u32 n=cstrlen(tmp);u32 z=width>n?width-n:0;u32 i=0;while(i<z)out[i++]='0';for(u32 j=0;j<n&&i<width;++j)out[i++]=tmp[j];out[width]='\0';}

struct MaterializeResult{u32 source_offset=0;u16 source_selector=0;u32 length=0;bool local_source=true;};

// F2E9..F721 (1081): conversion materializer. This keeps argument-cursor movement,
// selector-qualified strings, integer width/sign/prefix bookkeeping and pointer rendering explicit.
// Floating conversions remain behind the exact F206/F2D2 helpers selected by the encoded flags.
[[nodiscard]]MaterializeResult function_f2e9(FormatterBoundary&b,char*scratch,u8*&arg_cursor,FormatState&s,u16 ds,u32 null_string_offset)noexcept{
 MaterializeResult r{}; r.source_offset=0; r.source_selector=ds; r.local_source=true;
 u32 numeric=0; bool signed_family=false; bool numeric_family=false; u32 radix=10;
 const u8 conv=s.conversion;
 if(conv=='X'||conv=='d'||conv=='i'||conv=='o'||conv=='u'||conv=='x'){
  if(s.flags&0x20u){arg_cursor+=4;numeric=*reinterpret_cast<const u32*>(arg_cursor-4);}else if(s.flags&0x10u){arg_cursor+=4;numeric=static_cast<u16>(*reinterpret_cast<const u32*>(arg_cursor-4));}else{arg_cursor+=4;numeric=*reinterpret_cast<const u32*>(arg_cursor-4);}
  s.flags=static_cast<u8>(s.flags&0xf9u); if(conv=='d'||conv=='i')signed_family=true;
 }
 if(s.precision!=0xffffffffu&&(conv=='d'||conv=='i'||conv=='o'||conv=='u'||conv=='x'||conv=='X'))s.pad=' ';
 switch(conv){
 case 'E':case 'F':case 'G':case 'e':case 'f':case 'g':{
  if(s.flags&0x10u){arg_cursor+=4;function_f206(scratch,static_cast<s32>(*reinterpret_cast<const u32*>(arg_cursor-4)),s);r.source_selector=ds;r.length=bounded8(b,0,ds,0);r.local_source=true;break;}
  const u32 p=function_f2d2(b,reinterpret_cast<u32>(arg_cursor),reinterpret_cast<u32>(&s),reinterpret_cast<u32>(scratch));(void)p;r.source_selector=ds;r.local_source=true;break;}
 case 'o': radix=8; if((s.flags&1u)!=0){*scratch='0';} numeric_family=true; break;
 case 'd':case 'i': signed_family=true; numeric_family=true; radix=10; break;
 case 'u': numeric_family=true; radix=10; break;
 case 'x':case 'X': if((s.flags&1u)!=0&&numeric!=0){s.prefix[0]='0';s.prefix[1]=static_cast<u8>(conv);s.prefix[2]=0;}radix=16;numeric_family=true;break;
 case 's':case 'S':{
  u32 p=0;u16 sel=ds;
  if(s.flags&0x80u){arg_cursor+=8;p=*reinterpret_cast<const u32*>(arg_cursor-8);sel=*reinterpret_cast<const u16*>(arg_cursor-4);if(p==0&&sel==0){p=null_string_offset;sel=ds;}}
  else{arg_cursor+=4;p=*reinterpret_cast<const u32*>(arg_cursor-4);if(p==0)p=null_string_offset;}
  s.flags=static_cast<u8>(s.flags&0xf9u);r.source_offset=p;r.source_selector=sel;r.local_source=false;
  if(conv=='S'&&(s.flags&0x20u)){r.length=(b.read16?bounded16(b,p,sel,s.precision):0);}else r.length=(b.read8?bounded8(b,p,sel,s.precision):0);break;}
 case 'p':case 'P':{
  if(s.width==0)s.width=(s.flags&0x80u)?13u:8u;s.flags=static_cast<u8>(s.flags&0xf9u);arg_cursor+=4;const u32 off=*reinterpret_cast<const u32*>(arg_cursor-4);
  if(s.flags&0x80u){arg_cursor+=4;const u16 sel=*reinterpret_cast<const u16*>(arg_cursor-4);hexpad(sel,scratch,4);scratch[4]=':';hexpad(off,scratch+5,8);}else hexpad(off,scratch,8);
  if(conv=='P')uppercase(scratch);r.length=cstrlen(scratch);break;}
 case 'c': arg_cursor+=4;scratch[0]=static_cast<char>(*reinterpret_cast<const u8*>(arg_cursor-4));scratch[1]=0;s.precision=1;s.flags=static_cast<u8>(s.flags&0xf9u);r.length=1;break;
 default: s.width=0;scratch[0]=static_cast<char>(conv);scratch[1]=0;s.precision=1;s.flags=static_cast<u8>(s.flags&0xf9u);r.length=1;break;
 }
 if(numeric_family){
  if(signed_family)(void)function_dffb(static_cast<s32>(numeric),scratch,radix);else (void)function_e050(numeric,scratch,radix);if(conv=='X')uppercase(scratch);r.length=cstrlen(scratch);
  if(s.precision==0&&scratch[0]=='0')scratch[0]=0;
 }
 if(r.local_source&&r.length==0)r.length=cstrlen(scratch);
 if(r.local_source&&scratch[0]=='-'){--r.length;s.prefix[0]='-';s.prefix[1]=0;r.source_offset=1;}else if((s.flags&2u)!=0){s.prefix[0]=' ';s.prefix[1]=0;}else if((s.flags&4u)!=0){s.prefix[0]='+';s.prefix[1]=0;}
 if(r.length>s.precision)s.precision=r.length;else if(s.precision!=0xffffffffu)s.opaque0[0]=static_cast<u8>(s.precision-r.length);
 if(s.pad=='*'){s.prefix[0]=0;s.flags=static_cast<u8>(s.flags&0xf9u);}
 if(s.precision==0xffffffffu||r.length<s.precision){if(conv!='c')s.precision=r.length;}
 s.width-=static_cast<u32>(cstrlen(reinterpret_cast<const char*>(s.prefix)))+s.precision+s.opaque0[0];
 return r;
}

static_assert(0xF2D2u-0xF206u==204u);
static_assert(0xF2E9u-0xF2D2u==23u);
static_assert(0xF722u-0xF2E9u==1081u);
constexpr u32 kStagedInstructionAccountedBytes=1308u;
} // namespace code1_formatter_continuation
