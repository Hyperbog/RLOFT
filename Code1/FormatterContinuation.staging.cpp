/* CODE.1 formatter continuation verification staging.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Verified range: F206..F721 = 1308 instruction-accounted bytes.
 */
#include <cstdint>
#include <cstring>
namespace code1_formatter_continuation {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t; using s16=std::int16_t; using s32=std::int32_t;
struct FormatState{u8 opaque0[4];u32 width;u32 precision;u32 zero_count;u8 opaque10[4];u8 flags;u8 conversion;u8 pad;u8 prefix[3];};
static_assert(sizeof(FormatState)==0x1c);
extern char* function_dffb(s32,char*,u32)noexcept;
extern char* function_e050(u32,char*,u32)noexcept;
extern char* function_e098(s32,char*,u32)noexcept;
extern u32 function_d991(u32)noexcept;

// F206..F2D1 (204): signed 16.16 fixed-point decimal materialization.
// WORD [EBP-2] is the integer/high word. Fractional iterations clear that
// word, multiply the retained low word by ten, and emit the new high word.
void function_f206(char*out,s32 value,FormatState&s)noexcept{
 char*p=out;u32 fixed=static_cast<u32>(value);
 if(value<0){fixed=0u-fixed;*p++='-';}
 if(s.precision==0xffffffffu)s.precision=4;
 (void)function_dffb(static_cast<s32>(static_cast<u16>(fixed>>16)),p,10);
 char*integer_begin=p;char*end=p;while(*end)++end;
 if(s.precision!=0){*end++='.';for(u32 i=0;i<s.precision;++i){fixed&=0x0000ffffu;fixed*=10u;*end++=static_cast<char>(static_cast<u8>(fixed>>16)+static_cast<u8>('0'));}*end='\0';}
 // F27E tests bit 15 of the retained low word. F284..F2B4 performs the
 // encoded carry walk, including the carry-through-front decimal rewrite.
 if((fixed&0x00008000u)!=0){char*q=end;for(;;){if(q==integer_begin){char*scan=integer_begin+1;*integer_begin='1';while(*scan=='0')++scan;if(*scan=='.'){*scan='0';char*dot=scan+1;*dot='.';scan=dot+1;while(*scan=='0')++scan;}*scan++='0';*scan='\0';break;}--q;if(*q=='.')--q;if(*q!='9'){++*q;break;}*q='0';}}
}

// F2D2..F2E8 (23): indirect thunk. The callee's return is EAX:DX.
struct FarReturn{u32 offset=0;u16 selector=0;};
struct FormatterBoundary{FarReturn(*call_55c4)(u32,u32,u32)=nullptr;u8(*read_linear8)(u32)=nullptr;u16(*read_linear16)(u32)=nullptr;u32(*read_linear32)(u32)=nullptr;void(*write_linear32)(u32,u32)=nullptr;u8(*read_selector8)(u16,u32)=nullptr;u16(*read_selector16)(u16,u32)=nullptr;};
[[nodiscard]]FarReturn function_f2d2(FormatterBoundary&b,u32 scratch,u32 cursor_cell,u32 state)noexcept{return b.call_55c4?b.call_55c4(scratch,cursor_cell,state):FarReturn{};}
static u32 get32(FormatterBoundary&b,u32 a)noexcept{return b.read_linear32?b.read_linear32(a):0;}
static void put32(FormatterBoundary&b,u32 a,u32 v)noexcept{if(b.write_linear32)b.write_linear32(a,v);}
static u32 take32(FormatterBoundary&b,u32 c)noexcept{const u32 n=get32(b,c)+4u;put32(b,c,n);return get32(b,n-4u);}
static s32 take_s16_promoted(FormatterBoundary&b,u32 c)noexcept{return static_cast<s16>(take32(b,c));}
static u16 take_u16_promoted(FormatterBoundary&b,u32 c)noexcept{return static_cast<u16>(take32(b,c));}
static u32 bounded8(FormatterBoundary&b,u32 p,u16 s,u32 l)noexcept{u32 n=0;while(n!=l&&b.read_selector8&&b.read_selector8(s,p+n)!=0)++n;return n;}
static u32 bounded16(FormatterBoundary&b,u32 p,u16 s,u32 l)noexcept{u32 n=0;while(n!=l&&b.read_selector16&&b.read_selector16(s,p+n*2u)!=0)++n;return n;}
static u32 cstrlen(const char*p)noexcept{return static_cast<u32>(std::strlen(p));}
static void uppercase(char*p)noexcept{while(*p){*p=static_cast<char>(function_d991(static_cast<u8>(*p)));++p;}}
static void hexpad(u32 v,char*out,u32 w)noexcept{char t[16]{};(void)function_dffb(static_cast<s32>(v),t,16);const u32 n=cstrlen(t);u32 i=0;for(;i+n<w;++i)out[i]='0';for(u32 j=0;j<n&&i<w;++j)out[i++]=t[j];out[w]='\0';}
struct MaterializeResult{u32 source_offset=0;u16 source_selector=0;u32 accounting_eax=0;};

// F2E9..F721 (1081): verified materializer. The routine shares F001's
// epilogue with its caller; this function returns the EAX/EDX-equivalent
// payload state while preserving the exact state mutations made before F71D.
[[nodiscard]]MaterializeResult function_f2e9(FormatterBoundary&b,char*scratch,u32 scratch_guest,u32 cursor_cell,FormatState&s,u32 state_guest,u16 ds,u32 null_string_offset)noexcept{
 MaterializeResult r{scratch_guest,ds,0};u32 numeric=0;bool sign_processing=true;const u8 conv=s.conversion;
 s.zero_count=0; // F303: MOV DWORD PTR [EBX+0C],0.
 if(conv=='X'||conv=='o'||conv=='u'||conv=='x'){if(s.flags&0x20u)numeric=take32(b,cursor_cell);else if(s.flags&0x10u)numeric=take_u16_promoted(b,cursor_cell);else numeric=take32(b,cursor_cell);s.flags=static_cast<u8>(s.flags&0xf9u);}
 if(s.precision!=0xffffffffu&&(conv=='X'||conv=='d'||conv=='i'||conv=='o'||conv=='u'||conv=='x'))s.pad=' ';
 switch(conv){
 case 'E':case 'F':case 'G':case 'e':case 'f':case 'g':
  if((conv=='F'||conv=='f')&&(s.flags&0x10u)){function_f206(scratch,static_cast<s32>(take32(b,cursor_cell)),s);r.accounting_eax=cstrlen(scratch);}
  else{const FarReturn q=function_f2d2(b,scratch_guest,cursor_cell,state_guest);r.source_offset=q.offset;r.source_selector=q.selector;r.accounting_eax=bounded8(b,q.offset,q.selector,0xffffffffu);}break;
 case 'o':{char*digits=scratch;if(s.flags&1u)*digits++='0';(void)function_e098(static_cast<s32>(numeric),digits,8);if(s.precision==0&&digits[0]=='0')scratch[0]=0;r.accounting_eax=cstrlen(scratch);break;}
 case 'd':case 'i':{s32 value;if(s.flags&0x20u)value=static_cast<s32>(take32(b,cursor_cell));else if(s.flags&0x10u)value=take_s16_promoted(b,cursor_cell);else value=static_cast<s32>(take32(b,cursor_cell));if(s.flags&0x30u)(void)function_e098(value,scratch,10);else(void)function_dffb(value,scratch,10);if(s.precision==0&&scratch[0]=='0')scratch[0]=0;r.accounting_eax=cstrlen(scratch);break;}
 case 'u':(void)function_e050(numeric,scratch,10);if(s.precision==0&&scratch[0]=='0')scratch[0]=0;r.accounting_eax=cstrlen(scratch);break;
 case 'x':case 'X':if((s.flags&1u)&&numeric!=0){s.prefix[0]='0';s.prefix[1]=conv;s.prefix[2]=0;}(void)function_e050(numeric,scratch,16);if(conv=='X')uppercase(scratch);if(s.precision==0&&scratch[0]=='0')scratch[0]=0;r.accounting_eax=cstrlen(scratch);break;
 case 's':case 'S':{u32 p;u16 sel=ds;if(s.flags&0x80u){const u32 n=get32(b,cursor_cell)+8u;put32(b,cursor_cell,n);p=get32(b,n-8u);sel=b.read_linear16?b.read_linear16(n-4u):0;if(p==0&&sel==0){p=null_string_offset;sel=ds;}}else{p=take32(b,cursor_cell);if(p==0)p=null_string_offset;}r.source_offset=p;r.source_selector=sel;s.flags=static_cast<u8>(s.flags&0xf9u);sign_processing=false;if(conv=='S'){if(s.flags&0x20u){r.accounting_eax=b.read_selector16?b.read_selector16(sel,p):0;r.source_offset=p+2u;}else{r.accounting_eax=b.read_selector8?b.read_selector8(sel,p):0;r.source_offset=p+1u;}}else r.accounting_eax=(s.flags&0x20u)?bounded16(b,p,sel,s.precision):bounded8(b,p,sel,s.precision);break;}
 case 'p':case 'P':{if(s.width==0)s.width=(s.flags&0x80u)?13u:8u;s.flags=static_cast<u8>(s.flags&0xf9u);const u32 off=take32(b,cursor_cell);if(s.flags&0x80u){const u16 sel=take_u16_promoted(b,cursor_cell);hexpad(sel,scratch,4);scratch[4]=':';hexpad(off,scratch+5,8);}else hexpad(off,scratch,8);if(conv=='P')uppercase(scratch);r.accounting_eax=cstrlen(scratch);break;}
 case 'c':scratch[0]=static_cast<char>(take32(b,cursor_cell));scratch[1]=0;sign_processing=false;s.precision=1;s.flags=static_cast<u8>(s.flags&0xf9u);r.accounting_eax=1;break;
 default:s.width=0;scratch[0]=static_cast<char>(conv);scratch[1]=0;sign_processing=false;s.precision=1;s.flags=static_cast<u8>(s.flags&0xf9u);r.accounting_eax=1;break;}
 // F69B reads ES:EDI. Generated scratch remains host-local in this staging
 // representation, so its first byte must be read from scratch rather than
 // incorrectly routing the guest scratch offset through the external boundary.
 if(sign_processing){const bool local_source=(r.source_selector==ds&&r.source_offset==scratch_guest);const u8 first=local_source?static_cast<u8>(scratch[0]):(b.read_selector8?b.read_selector8(r.source_selector,r.source_offset):0);if(first=='-'){--r.accounting_eax;s.prefix[0]='-';s.prefix[1]=0;++r.source_offset;}else if(s.flags&2u){s.prefix[0]=' ';s.prefix[1]=0;}else if(s.flags&4u){s.prefix[0]='+';s.prefix[1]=0;}if(static_cast<s32>(r.accounting_eax)>static_cast<s32>(s.precision))s.precision=r.accounting_eax;else s.zero_count=s.precision-r.accounting_eax;}
 if(s.pad=='*'){s.prefix[0]=0;s.flags=static_cast<u8>(s.flags&0xf9u);}
 if((s.precision==0xffffffffu||static_cast<s32>(r.accounting_eax)<static_cast<s32>(s.precision))&&conv!='c')s.precision=r.accounting_eax;
 const u32 occupied=cstrlen(reinterpret_cast<const char*>(s.prefix))+s.precision+s.zero_count;s.width-=occupied;return r;
}
static_assert(0xF2D2u-0xF206u==204u);
static_assert(0xF2E9u-0xF2D2u==23u);
static_assert(0xF722u-0xF2E9u==1081u);
constexpr u32 kVerifiedStagedBytes=1308u;
constexpr u32 kStructuralUncountedBytes=0u;
} // namespace code1_formatter_continuation
