/* CODE.1 formatter driver verification staging.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Range: EDE7..F006 = 544 bytes. Structural lift; NOT counted yet.
 *
 * This routine owns the stack-resident FormatState consumed by F007/F2E9.
 * F2E9 returns through the shared F001 epilogue in the original instruction
 * stream; this source represents that edge as a normal materializer return.
 */
#include <cstdint>
namespace code1_formatter_driver {
using u8=std::uint8_t;using u16=std::uint16_t;using u32=std::uint32_t;using s32=std::int32_t;
struct FormatState{u32 context;u32 width;u32 precision;u32 zero_count;u32 count;u8 flags;u8 conversion;u8 pad;u8 prefix[3];};
static_assert(sizeof(FormatState)==0x1c);
struct MaterializeResult{u32 source_offset;u16 source_selector;u32 accounting_eax;};
struct Boundary{
 const u8*(*parse_f007)(const u8*,u32 cursor_cell,FormatState&)=nullptr;
 MaterializeResult(*materialize_f2e9)(u32 scratch_guest,u32 cursor_cell,FormatState&,u32 state_guest)=nullptr;
 void(*emit)(FormatState&,u8)=nullptr;
 u32(*read_linear32)(u32)=nullptr;u16(*read_linear16)(u32)=nullptr;
 void(*write_linear32)(u32,u32)=nullptr;void(*write_linear16)(u32,u16)=nullptr;
 u8(*read_selector8)(u16,u32)=nullptr;
 void(*write_selector32)(u16,u32,u32)=nullptr;void(*write_selector16)(u16,u32,u16)=nullptr;
};
static u32 rd32(Boundary&b,u32 a)noexcept{return b.read_linear32?b.read_linear32(a):0;}
static u16 rd16(Boundary&b,u32 a)noexcept{return b.read_linear16?b.read_linear16(a):0;}
static void wr32(Boundary&b,u32 a,u32 v)noexcept{if(b.write_linear32)b.write_linear32(a,v);}
static u32 take32(Boundary&b,u32 cursor_cell)noexcept{const u32 n=rd32(b,cursor_cell)+4u;wr32(b,cursor_cell,n);return rd32(b,n-4u);}
static void output(Boundary&b,FormatState&s,u8 ch)noexcept{if(b.emit)b.emit(s,ch);}

// EDE7..F006. format is the byte stream initially loaded from [EBP+1C].
// cursor_cell models [EBP+20], a pointer to the caller's advancing argument
// cursor. null/context input is the dword copied from [EBP+18] to state+00.
[[nodiscard]]u32 function_ede7(Boundary&b,u32 initial_context,const u8*format,u32 cursor_cell,u32 scratch_guest,u32 state_guest)noexcept{
 FormatState s{};s.context=initial_context;s.count=0;const u8*p=format;
 for(;;){
  if(*p==0)return s.count;
  if(*p!='%'){output(b,s,*p++);continue;}

  // EE10..EE2E: F007 returns the conversion-byte address. ESI is advanced
  // once past it and that post-conversion address becomes the next format p.
  if(!b.parse_f007)return s.count;
  const u8*conversion=b.parse_f007(p,cursor_cell,s);
  s.conversion=*conversion;p=conversion+1;
  if(s.conversion==0)return s.count;

  if(s.conversion=='n'){
   // EE39..EECE. Tests of 40h are encoded but do not alter the near-store
   // operations. 20h selects dword storage; 10h selects word storage; 80h
   // selects an eight-byte far-pointer argument (offset dword + selector).
   if(s.flags&0x20u){
    if(s.flags&0x80u){const u32 base=rd32(b,cursor_cell)+8u;wr32(b,cursor_cell,base);const u32 off=rd32(b,base-8u);const u16 sel=rd16(b,base-4u);if(b.write_selector32)b.write_selector32(sel,off,s.count);}
    else{const u32 dst=take32(b,cursor_cell);wr32(b,dst,s.count);}
   }else if(s.flags&0x10u){
    if(s.flags&0x80u){const u32 base=rd32(b,cursor_cell)+8u;wr32(b,cursor_cell,base);const u32 off=rd32(b,base-8u);const u16 sel=rd16(b,base-4u);if(b.write_selector16)b.write_selector16(sel,off,static_cast<u16>(s.count));}
    else{const u32 dst=take32(b,cursor_cell);if(b.write_linear16)b.write_linear16(dst,static_cast<u16>(s.count));}
   }else{
    if(s.flags&0x80u){const u32 base=rd32(b,cursor_cell)+8u;wr32(b,cursor_cell,base);const u32 off=rd32(b,base-8u);const u16 sel=rd16(b,base-4u);if(b.write_selector32)b.write_selector32(sel,off,s.count);}
    else{const u32 dst=take32(b,cursor_cell);wr32(b,dst,s.count);}
   }
   continue;
  }

  if(!b.materialize_f2e9)return s.count;
  MaterializeResult r=b.materialize_f2e9(scratch_guest,cursor_cell,s,state_guest);
  u32 source=r.source_offset;const u16 selector=r.source_selector;

  // EEE9..EF0C: right-aligned space padding before prefix.
  if((s.flags&0x08u)==0&&s.pad==' '){while(static_cast<s32>(--s.width)>=0)output(b,s,s.pad);}

  // EF0E..EF2C: prefix at state+17, NUL terminated.
  for(u32 i=0;i<3u&&s.prefix[i]!=0;++i)output(b,s,s.prefix[i]);

  // EF2E..EF43: precision-derived zero fill. The post-decrement loop emits
  // exactly the pre-loop zero_count value and leaves the field wrapped/decremented.
  for(;;){const u32 old=s.zero_count--;if(old==0)break;output(b,s,'0');}

  // EF45..EF68: non-space pad is emitted here for right alignment.
  if((s.flags&0x08u)==0&&s.pad!=' '){while(static_cast<s32>(--s.width)>=0)output(b,s,s.pad);}

  // EF6A..EFCD: payload length is state.precision. s/S with flag 20h advances
  // the selector-qualified source by two bytes per emitted low byte; all other
  // payload paths advance by one byte.
  for(;;){const u32 old=s.precision--;if(old==0)break;const u8 ch=b.read_selector8?b.read_selector8(selector,source):0;output(b,s,ch);source+=((s.conversion=='s'||s.conversion=='S')&&(s.flags&0x20u))?2u:1u;}

  // EFCF..EFFC: left alignment emits trailing spaces only while width > 0.
  if(s.flags&0x08u){while(static_cast<s32>(s.width)>0){const u32 old=s.width--;if(old==0)break;output(b,s,' ');}}
 }
}
static_assert(0xF007u-0xEDE7u==544u);
constexpr u32 kStructuralUncountedBytes=544u;
} // namespace code1_formatter_driver
