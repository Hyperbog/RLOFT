/* Temporary staging unit for CODE.1 E6EA..EA0A.
 * Source of truth: immutable CODE.1. This unit exists only so the newly
 * instruction-checked routines are reviewable before they are folded into
 * Code1.cpp; no host-timer modernization is performed here.
 */
#include <cstdint>
namespace code1_timer_stage {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
struct Machine{u8*data;bool interrupt_enable;};
struct LowLevelBoundaries{};
struct TimerBoundaries{
 void(*pit_reload)(Machine&,LowLevelBoundaries&,u32)=nullptr;
 void(*restore_int8)(Machine&,LowLevelBoundaries&)=nullptr;
 void(*initialize)(Machine&)=nullptr;
 void(*install_int8)(Machine&,LowLevelBoundaries&)=nullptr;
 void(*recalculate)(Machine&,LowLevelBoundaries&)=nullptr;
 void(*call_ebb6)(u32,u32)=nullptr;
};
static u16 r16(const u8*p)noexcept{return static_cast<u16>(p[0]|(static_cast<u16>(p[1])<<8));}
static u32 r32(const u8*p)noexcept{return static_cast<u32>(p[0]|(static_cast<u32>(p[1])<<8)|(static_cast<u32>(p[2])<<16)|(static_cast<u32>(p[3])<<24));}
static void w16(u8*p,u16 v)noexcept{p[0]=static_cast<u8>(v);p[1]=static_cast<u8>(v>>8);}
static void w32(u8*p,u32 v)noexcept{p[0]=static_cast<u8>(v);p[1]=static_cast<u8>(v>>8);p[2]=static_cast<u8>(v>>16);p[3]=static_cast<u8>(v>>24);}
static bool cli(Machine&m)noexcept{const bool p=m.interrupt_enable;m.interrupt_enable=false;return p;}
static void restore_if(Machine&m,bool p)noexcept{m.interrupt_enable=p;}
static u8* p(Machine&m,u32 o)noexcept{return m.data+o;}

void function_e809(Machine&,LowLevelBoundaries&,TimerBoundaries&,u32)noexcept;
void function_e856(Machine&,LowLevelBoundaries&,TimerBoundaries&)noexcept;
void function_e87b(Machine&,u32)noexcept;
void function_e925(Machine&,LowLevelBoundaries&,TimerBoundaries&,u32,u32)noexcept;

// E6EA..E758 (111 bytes): walk 16 registered slots, release assigned timer,
// dispatch EBB6(slot,arg), then perform the all-slot timer release pass.
void function_e6ea(Machine&m,LowLevelBoundaries&low,TimerBoundaries&b,u32 arg)noexcept{
 const bool prior=cli(m); w32(p(m,0x52da),0);
 for(u32 slot=0;slot<16;++slot){
  w32(p(m,0x52da),slot);
  const u32 assigned=r32(p(m,0x5256+slot*4));
  if(r32(p(m,0x5216+slot*4))!=0){
   if(assigned!=0xffffffffu)function_e809(m,low,b,assigned);
   if(b.call_ebb6)b.call_ebb6(slot,arg);
  }
 }
 function_e856(m,low,b); restore_if(m,prior);
}

// E759..E808 (176 bytes): allocate one of the 16 state words. On the first
// active allocation the original code resets timer state, installs INT 8,
// configures special slot 16, then reinstates the newly allocated slot.
u32 function_e759(Machine&m,LowLevelBoundaries&low,TimerBoundaries&b,u32 value)noexcept{
 const bool prior=cli(m); u32 slot=0;
 while(slot<16&&r16(p(m,0x5154+slot*2))!=0)++slot;
 if(slot==16){restore_if(m,prior);return 0xffffffffu;}
 w16(p(m,0x5154+slot*2),1); w32(p(m,0x5110+slot*4),value); w32(p(m,0x51ba+slot*4),0xffffffffu);
 const u16 active=static_cast<u16>(r16(p(m,0x510c))+1u); w16(p(m,0x510c),active);
 if(active==1){
  if(b.initialize)b.initialize(m); w16(p(m,0x5174),1);
  if(b.install_int8)b.install_int8(m,low);
  function_e925(m,low,b,16,0xd68du); function_e87b(m,16);
  w16(p(m,0x5154+slot*2),1); w32(p(m,0x51ba+slot*4),0xffffffffu);
 }
 restore_if(m,prior); return slot;
}

// E809..E855 (77 bytes): release a slot. Last release writes PIT reload zero
// and restores the saved INT 8 vector.
void function_e809(Machine&m,LowLevelBoundaries&low,TimerBoundaries&b,u32 slot)noexcept{
 const bool prior=cli(m);
 if(slot!=0xffffffffu&&r16(p(m,0x5154+slot*2))!=0){
  w16(p(m,0x5154+slot*2),0);
  const u16 active=static_cast<u16>(r16(p(m,0x510c))-1u); w16(p(m,0x510c),active);
  if(active==0){if(b.pit_reload)b.pit_reload(m,low,0);if(b.restore_int8)b.restore_int8(m,low);}
 }
 restore_if(m,prior);
}

// E856..E87A (37 bytes).
void function_e856(Machine&m,LowLevelBoundaries&low,TimerBoundaries&b)noexcept{const bool prior=cli(m);for(int slot=15;slot>=0;--slot)function_e809(m,low,b,static_cast<u32>(slot));restore_if(m,prior);}
// E87B..E8AA (48 bytes): state 1 -> 2.
void function_e87b(Machine&m,u32 slot)noexcept{const bool prior=cli(m);u8*q=p(m,0x5154+slot*2);if(r16(q)==1)w16(q,2);restore_if(m,prior);}
// E8AB..E8CF (37 bytes).
void function_e8ab(Machine&m)noexcept{const bool prior=cli(m);for(int slot=15;slot>=0;--slot)function_e87b(m,static_cast<u32>(slot));restore_if(m,prior);}
// E8D0..E8FF (48 bytes): state 2 -> 1.
void function_e8d0(Machine&m,u32 slot)noexcept{const bool prior=cli(m);u8*q=p(m,0x5154+slot*2);if(r16(q)==2)w16(q,1);restore_if(m,prior);}
// E900..E924 (37 bytes).
void function_e900(Machine&m)noexcept{const bool prior=cli(m);for(int slot=15;slot>=0;--slot)function_e8d0(m,static_cast<u32>(slot));restore_if(m,prior);}

// E925..E979 (85 bytes): temporarily force state=1 while replacing the slot
// period and clearing its accumulator, recalculate, then restore prior state.
void function_e925(Machine&m,LowLevelBoundaries&low,TimerBoundaries&b,u32 slot,u32 period)noexcept{
 const bool prior=cli(m); const u16 old=r16(p(m,0x5154+slot*2));
 w16(p(m,0x5154+slot*2),1); w32(p(m,0x51ba+slot*4),period); w32(p(m,0x5176+slot*4),0);
 if(b.recalculate)b.recalculate(m,low); w16(p(m,0x5154+slot*2),old); restore_if(m,prior);
}
// E97A..E9AC (51 bytes). Original DIV is intentionally unguarded.
void function_e97a(Machine&m,LowLevelBoundaries&low,TimerBoundaries&b,u32 slot,u32 divisor)noexcept{const bool prior=cli(m);function_e925(m,low,b,slot,1000000u/divisor);restore_if(m,prior);}
// E9AD..E9F1 (69 bytes).
void function_e9ad(Machine&m,LowLevelBoundaries&low,TimerBoundaries&b,u32 slot,u32 value)noexcept{const bool prior=cli(m);const u32 period=value==0?0xd68du:static_cast<u32>((static_cast<std::uint64_t>(0x2710u)*value)/0x2e9cu);function_e925(m,low,b,slot,period);restore_if(m,prior);}
// E9F2..EA0A (25 bytes).
u32 function_e9f2(Machine&m)noexcept{const bool prior=cli(m);const u32 value=r32(p(m,0x5212));restore_if(m,prior);return value;}

constexpr u32 kStagedVerifiedBytes=801u;
} // namespace code1_timer_stage
