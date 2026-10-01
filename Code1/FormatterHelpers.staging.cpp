/* CODE.1 formatter-helper verification staging.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Staging does not increase official coverage until folded into Code1.cpp.
 */
#include <cstdint>
#include <cstring>
namespace code1_formatter_staging {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t; using s32=std::int32_t;
static u32 load32(const u8*p)noexcept{return static_cast<u32>(p[0]|(static_cast<u32>(p[1])<<8)|(static_cast<u32>(p[2])<<16)|(static_cast<u32>(p[3])<<24));}
static void store32(u8*p,u32 v)noexcept{p[0]=static_cast<u8>(v);p[1]=static_cast<u8>(v>>8);p[2]=static_cast<u8>(v>>16);p[3]=static_cast<u8>(v>>24);}
struct FormatState{u8 opaque0[4];u32 width;u32 precision;u8 opaque0c[8];u8 flags;u8 opaque15;u8 pad;u8 opaque17;};
static_assert(sizeof(FormatState)==0x18);

// F110..F162 (83): scan '-', '#', '+', space and '0' flags.
[[nodiscard]]const u8* function_f110(const u8*p,FormatState&s)noexcept{s.flags=0;for(;;++p){switch(*p){case '-':s.flags|=0x08;break;case '#':s.flags|=0x01;break;case '+':s.flags=static_cast<u8>((s.flags|0x04)&0xfd);break;case ' ':if((s.flags&0x04)==0)s.flags|=0x02;break;case '0':s.pad='0';break;default:return p;}}}

// F007..F10F (265): parse flags, width, precision and encoded length modifier.
// arg_cursor is the original pointer stored through ESI; '*' consumes one dword.
[[nodiscard]]const u8* function_f007(const u8*p,u8*&arg_cursor,FormatState&s)noexcept{s.opaque17=0;s.pad=' ';p=function_f110(p,s);s.width=0;if(*p>='0'&&*p<='9'){do{s.width=s.width*10u+static_cast<u32>(*p-'0');++p;}while(*p>='0'&&*p<='9');}else if(*p=='*'){arg_cursor+=4;s.width=load32(arg_cursor-4);if(static_cast<s32>(s.width)<0){s.width=0u-s.width;s.flags|=0x08;}++p;}s.precision=0xffffffffu;if(*p=='.'){++p;s.precision=0;if(*p=='*'){arg_cursor+=4;s.precision=load32(arg_cursor-4);if(static_cast<s32>(s.precision)<0)s.precision=0xffffffffu;++p;}else while(*p>='0'&&*p<='9'){s.precision=s.precision*10u+static_cast<u32>(*p-'0');++p;}}const u8 c=*p;const u8*next=p+1;if(c=='l'||c=='w'){++p;s.flags|=0x20;}else if(c=='h'){p=next;s.flags|=0x10;}else if(c=='F'){p=next;s.flags|=0x80;}else if(c=='N'||c=='L'){p=next;s.flags|=0x40;}return p;}

// F163..F18B (41): bounded byte-string length through a selector.
// Selector access remains a boundary; no flat-memory assumption is introduced.
struct FarReadBoundary{u8(*read8)(u16,u32)=nullptr;u16(*read16)(u16,u32)=nullptr;};
[[nodiscard]]u32 function_f163(FarReadBoundary&b,u32 offset,u16 selector,u32 limit)noexcept{u32 n=0;while(n!=limit){if(!b.read8||b.read8(selector,offset)==0)break;++offset;++n;}return n;}
// F18C..F1B2 (39): bounded two-byte-string length through a selector.
[[nodiscard]]u32 function_f18c(FarReadBoundary&b,u32 offset,u16 selector,u32 limit)noexcept{u32 n=0;while(n!=limit){if(!b.read16||b.read16(selector,offset)==0)break;offset+=2;++n;}return n;}

// F1B3..F205 (83): hexadecimal conversion followed by left zero-padding.
// strlen is kept as an explicit boundary because CODE.1 calls 6EE9 here.
struct StringBoundary{u32(*length)(const char*)=nullptr;};
extern char* function_dffb(s32,char*,u32)noexcept;
void function_f1b3(StringBoundary&b,u32 value,char*out,u32 width)noexcept{(void)function_dffb(static_cast<s32>(value),out,16);const u32 n=b.length?b.length(out):static_cast<u32>(std::strlen(out));s32 pad=static_cast<s32>(width)-1;char*src=out+n;char*dst=out+pad;while(src!=out){--src;*dst--=*src;--pad;}char*z=out+pad;while(pad>=0){*z--='0';--pad;}out[width]='\0';}

// F722..F741 (32): uppercase a NUL-terminated byte string through D991 semantics.
[[nodiscard]]u32 function_d991(u32 ch)noexcept{return(ch>=0x61u&&ch<=0x7au)?ch-0x20u:ch;}
void function_f722(char*s)noexcept{while(*s){*s=static_cast<char>(function_d991(static_cast<u8>(*s)));++s;}}

static_assert(0xF110u-0xF007u==265u);
static_assert(0xF163u-0xF110u==83u);
static_assert(0xF18Cu-0xF163u==41u);
static_assert(0xF1B3u-0xF18Cu==39u);
static_assert(0xF206u-0xF1B3u==83u);
static_assert(0xF742u-0xF722u==32u);
constexpr u32 kStagedInstructionAccountedBytes=543u;
} // namespace code1_formatter_staging
