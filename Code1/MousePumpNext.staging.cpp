/* CODE.1 D154..D1E7 mouse/event-pump continuation staging.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * Functional instructions: D154..D178 (37 bytes), D17C..D1E7 (108 bytes)
 * = 145 bytes. D179..D17B is LEA EAX,[EAX+0] alignment NOP, excluded.
 * NOT COUNTED until folded into Code1.cpp.
 */
#include <cstdint>
namespace code1_mouse_pump_next {
using u16=std::uint16_t;using u32=std::uint32_t;
struct Boundary{
 u32(*call_84d0)()=nullptr;
 u32(*call_d0b4)()=nullptr; // returns pointer whose first word is tested
 void(*call_8960)()=nullptr;
 u32(*call_82a4)()=nullptr;
 void(*call_82ec)()=nullptr;
 void(*call_8304)()=nullptr;
 void(*call_82c0)()=nullptr;
 u16(*read16)(u32)=nullptr;
};
static u16 word_at(Boundary&b,u32 p)noexcept{return b.read16?b.read16(p):0;}

/* D154..D178. If 84D0 returns zero, return immediately. Otherwise repeatedly
 * call D0B4, preserve its returned pointer across 8960, and continue while
 * the word at that pointer is nonzero. */
void function_d154(Boundary&b)noexcept{
 if(!b.call_84d0||b.call_84d0()==0)return;
 for(;;){const u32 p=b.call_d0b4?b.call_d0b4():0;if(b.call_8960)b.call_8960();if(word_at(b,p)==0)return;}
}

/* D17C..D1E7. First drain the same D0B4/8960 loop while 84D0 is active.
 * Then poll 82A4. While it is zero, inspect D0B4's returned word: a nonzero
 * word breaks to the final 82A4 test; a zero word causes 8960 and another
 * poll. If the final 82A4 is nonzero, call 82EC then 8304. Otherwise repeatedly
 * call 8960 and D0B4 until D0B4's first word becomes zero. If the initial
 * 84D0 was zero, call 82C0 instead. */
void function_d17c(Boundary&b)noexcept{
 if(b.call_84d0&&b.call_84d0()!=0){
  for(;;){const u32 p=b.call_d0b4?b.call_d0b4():0;if(b.call_8960)b.call_8960();if(word_at(b,p)==0)break;}
  while(!b.call_82a4||b.call_82a4()==0){const u32 p=b.call_d0b4?b.call_d0b4():0;if(word_at(b,p)!=0)break;if(b.call_8960)b.call_8960();}
  if(b.call_82a4&&b.call_82a4()!=0){if(b.call_82ec)b.call_82ec();if(b.call_8304)b.call_8304();return;}
  do{if(b.call_8960)b.call_8960();const u32 p=b.call_d0b4?b.call_d0b4():0;if(word_at(b,p)==0)break;}while(true);
 }else if(b.call_82c0)b.call_82c0();
}
static_assert(0xD179u-0xD154u==37u);static_assert(0xD1E8u-0xD17Cu==108u);
constexpr u32 kInstructionBoundedStagedBytes=145u;
} // namespace code1_mouse_pump_next
