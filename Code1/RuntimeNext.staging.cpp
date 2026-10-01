/* CODE.1 next-region verification staging.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Verified instruction-accounted ranges:
 *   DAD9..DB7F = 167 bytes
 *   DB80..DC8C = 269 bytes
 *   DC90..DDD7 = 328 bytes
 * These bytes remain excluded from official coverage until fold into Code1.cpp.
 */
#include <cstdint>
#include <cstring>
namespace code1_runtime_next {
using u8=std::uint8_t;using u16=std::uint16_t;using u32=std::uint32_t;using s32=std::int32_t;

// DAD9..DB7F. The encoded routine calls the allocator core at 104E0 with
// EAX=request, EDX=DS, EBX=30B8h, then tries 10AA9 once and 10C46 as the
// retry gate. DB5B passes the pointer to the free core at 10590. Names stay
// address-based until the surrounding runtime contracts are integrated.
struct AllocBoundary{
 u32(*call_104e0)(u32 request,u16 ds,u32 arena)=nullptr;
 u32(*call_10aa9)(u32 request)=nullptr;
 u32(*call_10c46)(u32 request)=nullptr;
 void(*call_10590)(u32 pointer,u16 ds,u32 arena)=nullptr;
 void(*write_linear8)(u32,u8)=nullptr;
 u16 ds=0;
};
[[nodiscard]]u32 function_dad9(AllocBoundary&b,u32 request)noexcept{
 if(request==0)return 0;
 bool retried_10aa9=false;
 for(;;){
  const u32 result=b.call_104e0?b.call_104e0(request,b.ds,0x30b8u):0;
  if(result!=0){if(b.write_linear8)b.write_linear8(0x8e78u,0);return result;}
  if(!retried_10aa9){if(b.call_10aa9&&b.call_10aa9(request)!=0){retried_10aa9=true;continue;}}
  if(!b.call_10c46||b.call_10c46(request)==0){if(b.write_linear8)b.write_linear8(0x8e78u,0);return 0;}
 }
}
void function_db5b(AllocBoundary&b,u32 pointer)noexcept{if(b.call_10590)b.call_10590(pointer,b.ds,0x30b8u);if(b.write_linear8)b.write_linear8(0x8e78u,0);}
void function_db4b(AllocBoundary&b,u32 pointer)noexcept{function_db5b(b,pointer);}

// DB80..DC8C. Exact 4096-byte ring dictionary walk. 7360 returns a
// zero-extended input byte; 738C consumes one output byte. The dictionary is
// initialized to FEh, write index FEEh. SI is shifted before testing the 0100h
// refill marker. Back-references combine two bytes as
// ((second & F0h)<<4)|first and copy (second&0Fh)+3 bytes.
struct DecodeBoundary{u8(*input_7360)()=nullptr;void(*output_738c)(u8)=nullptr;u8(*read_linear8)(u32)=nullptr;};
void function_db80(DecodeBoundary&b)noexcept{
 u8 dict[4096];std::memset(dict,0xfe,sizeof(dict));u16 write_index=0x0feeu;u16 si=0;
 for(;;){
  si=static_cast<u16>(si>>1);
  if((si&0x0100u)==0){if(b.read_linear8&&b.read_linear8(0x89ceu)==1)return;const u8 v=b.input_7360?b.input_7360():0;si=static_cast<u16>(0xff00u|v);}
  if(si&1u){
   if(b.read_linear8&&b.read_linear8(0x89ceu)==1)return;const u8 v=b.input_7360?b.input_7360():0;if(b.output_738c)b.output_738c(v);dict[write_index]=v;write_index=static_cast<u16>((write_index+1u)&0x0fffu);
  }else{
   if(b.read_linear8&&b.read_linear8(0x89ceu)==1)return;const u8 first=b.input_7360?b.input_7360():0;
   if(b.read_linear8&&b.read_linear8(0x89ceu)==1)return;const u8 second=b.input_7360?b.input_7360():0;
   const u16 source=static_cast<u16>((static_cast<u16>(second&0xf0u)<<4)|first);const u16 last=static_cast<u16>((second&0x0fu)+2u);
   for(u16 i=0;i<=last;++i){const u8 v=dict[(source+i)&0x0fffu];if(b.output_738c)b.output_738c(v);dict[write_index]=v;write_index=static_cast<u16>((write_index+1u)&0x0fffu);}
  }
 }
}

// DC90..DDD7. The routine operates only when header word +4 == 1. It builds
// a word permutation table in workspace 30E8h, then uses REPNE SCASW to find
// the current index in the remaining suffix. SCASW compares the initial EDI
// element before advancing EDI; after the instruction the encoded SUB EDI,2
// restores EDI to the matched element (or final examined element on failure).
struct TransformBoundary{u16(*read16)(u32)=nullptr;u8(*read8)(u32)=nullptr;void(*write16)(u32,u16)=nullptr;void(*write8)(u32,u8)=nullptr;};
static u16 tr16(TransformBoundary&b,u32 a)noexcept{return b.read16?b.read16(a):0;}static u8 tr8(TransformBoundary&b,u32 a)noexcept{return b.read8?b.read8(a):0;}static void tw16(TransformBoundary&b,u32 a,u16 v)noexcept{if(b.write16)b.write16(a,v);}static void tw8(TransformBoundary&b,u32 a,u8 v)noexcept{if(b.write8)b.write8(a,v);}
void function_dc90(TransformBoundary&b,u32 base)noexcept{
 if(tr16(b,base+4)!=1)return;u32 dst=base+8u;u32 source_base=base+8u;if(tr8(b,base+7u)&0x80u){++dst;++source_base;}
 const u32 row=tr16(b,base);const u32 stride=static_cast<u32>(tr8(b,base+6u))<<1;u32 remaining=4u*tr16(b,base+2u);const u32 table_words=remaining;const u32 wrap=remaining<<1;
 u32 bx=0,eax=0;for(u32 ecx=table_words;ecx!=0;--ecx){tw16(b,0x30e8u+bx,static_cast<u16>(eax));bx+=8u;++eax;if(bx>=wrap){bx-=wrap;bx+=2u;}}
 --remaining;dst+=stride;u32 index=1;
 while(remaining!=0){
  const u32 table_entry=0x30e8u+(index<<1);const u16 saved_table_value=tr16(b,table_entry);u32 scan=remaining;u32 after=table_entry;
  if(scan!=0){do{const u16 examined=tr16(b,after);after+=2u;--scan;if(examined==static_cast<u16>(index))break;}while(scan!=0);}
  const u32 match=after-2u;const u16 displaced=tr16(b,match);tw16(b,match,saved_table_value);const u32 mapped=(match-0x30e8u)>>1;tw16(b,table_entry,displaced);
  u32 a=source_base+(index>>2)*row+(index&3u)*stride;u32 s=source_base+(mapped>>2)*row+(mapped&3u)*stride;const u32 saved=a;
  for(u32 n=stride;n!=0;--n){const u8 av=tr8(b,a),sv=tr8(b,s);tw8(b,a,sv);tw8(b,s,av);++a;++s;}
  for(u32 n=0;n<stride;++n)tw8(b,dst+n,tr8(b,saved+n));++index;dst+=stride;--remaining;
 }
 const u16 old_count=tr16(b,base+2u);tw16(b,base+4u,old_count);tw16(b,base+2u,1);const u16 product=static_cast<u16>(static_cast<u16>(old_count)*static_cast<u16>(static_cast<u16>(tr8(b,base+6u))<<3));tw16(b,base,product);
}
static_assert(0xDB80u-0xDAD9u==167u);static_assert(0xDC8Du-0xDB80u==269u);static_assert(0xDDD8u-0xDC90u==328u);
constexpr u32 kVerifiedStagedBytes=764u;
} // namespace code1_runtime_next
