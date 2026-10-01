/* Temporary verification staging for CODE.1.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 *
 * Address-named, instruction-accounted staging only. This file does not
 * increase the official coverage counter until folded into Code1.cpp.
 * Ranges represented: E3B4..E4C6 and EA0B..EDE6 (EDD3..EDDF is data).
 */
#include <cstdint>
namespace code1_staging {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
static u16 load16(const u8*p)noexcept{return static_cast<u16>(p[0]|(static_cast<u16>(p[1])<<8));}
static u32 load32(const u8*p)noexcept{return static_cast<u32>(p[0]|(static_cast<u32>(p[1])<<8)|(static_cast<u32>(p[2])<<16)|(static_cast<u32>(p[3])<<24));}
static void store16(u8*p,u16 v)noexcept{p[0]=static_cast<u8>(v);p[1]=static_cast<u8>(v>>8);}
static void store32(u8*p,u32 v)noexcept{p[0]=static_cast<u8>(v);p[1]=static_cast<u8>(v>>8);p[2]=static_cast<u8>(v>>16);p[3]=static_cast<u8>(v>>24);}
struct Machine{u8*data=nullptr;bool interrupt_enable=false;};
struct DispatcherBoundary{u32(*invoke)(u32,const u32*,u32)=nullptr;void(*invoke_timer_callback)(u32)=nullptr;void(*out8)(u16,u8)=nullptr;u32(*allocate_timer)(u32)=nullptr;void(*release_timer)(u32)=nullptr;void(*set_timer_rate)(u32,u32)=nullptr;void(*enable_timer)(u32)=nullptr;};

// E3B4..E3DF (44): EBX=slot, EAX=event; FFFFFFFF terminates each pair table.
[[nodiscard]]u32 function_e3b4(Machine&m,u32 event,u32 slot)noexcept{if(slot>=16)return 0;u32 entry=load32(m.data+0x5216u+slot*4u);if(entry==0)return 0;for(;;entry+=8u){const u32 key=load32(m.data+entry);if(key==event)return load32(m.data+entry+4u);if(key==0xffffffffu)return 0;}}
// E3E0..E3F1 (18): resolve args[0]'s event and tail-transfer when nonzero.
[[nodiscard]]u32 function_e3e0(Machine&m,DispatcherBoundary&b,u32 event,const u32*args,u32 count)noexcept{if(count==0)return 0;const u32 target=function_e3b4(m,event,args[0]);return target&&b.invoke?b.invoke(target,args,count):0;}
enum class Irq8Action:u8{Handled,ChainSavedVector}; struct Irq8Result{Irq8Action action=Irq8Action::Handled;u16 selector=0;u32 offset=0;};
// E3F2..E4C6 (213): INT 8 handler and saved-vector chain path.
[[nodiscard]]Irq8Result function_e3f2(Machine&m,DispatcherBoundary&b)noexcept{if(load16(m.data+0x510e)!=0)return{Irq8Action::ChainSavedVector,static_cast<u16>(load32(m.data+0x5206)),load32(m.data+0x5202)};store16(m.data+0x510e,1);store32(m.data+0x520a,0);for(u32 slot=0;slot<=16;++slot){if(load16(m.data+0x5154u+slot*2u)==2){const u32 i=slot*4u;u32 accumulated=load32(m.data+0x5176u+i)+load32(m.data+0x51fe);const u32 period=load32(m.data+0x51bau+i);if(accumulated>=period){accumulated-=period;store32(m.data+0x5176u+i,accumulated);if(slot>=16)store16(m.data+0x510e,0);if(b.invoke_timer_callback)b.invoke_timer_callback(load32(m.data+0x5110u+i));}else store32(m.data+0x5176u+i,accumulated);}store32(m.data+0x520a,slot+1u);}store16(m.data+0x510e,0);if(b.out8)b.out8(0x20,0x20);return{};}

// EA9A..EAC7 (46): clear slot table entry when slot < 16, preserving IF.
void function_ea9a(Machine&m,u32 slot)noexcept{const bool prior=m.interrupt_enable;m.interrupt_enable=false;if(slot<16)store32(m.data+0x5216u+slot*4u,0);m.interrupt_enable=prior;}
// EAC8..EAE6 (31): event 64h receives {slot,E9F2h}.
[[nodiscard]]u32 function_eac8(Machine&m,DispatcherBoundary&b,u32 slot)noexcept{const u32 args[2]={slot,0xe9f2u};return function_e3e0(m,b,0x64u,args,2);}
// EA0B..EA99 (143): free-slot search, "Copy" descriptor check and limit check.
[[nodiscard]]u32 function_ea0b(Machine&m,DispatcherBoundary&b,u32 descriptor)noexcept{const bool prior=m.interrupt_enable;m.interrupt_enable=false;u32 slot=0;while(slot<16&&load32(m.data+0x5216u+slot*4u)!=0)++slot;u32 result=0xffffffffu;if(slot<16&&load32(m.data+descriptor+4u)==0x79706f43u){store32(m.data+0x5216u+slot*4u,load32(m.data+descriptor));const u32 returned=function_eac8(m,b,slot);if(returned!=0&&load32(m.data+returned)<=load32(m.data+0x52e8u))result=slot;}m.interrupt_enable=prior;return result;}
// EAE7..EAF0 (10): event 65h pure tail-dispatch.
[[nodiscard]]u32 function_eae7(Machine&m,DispatcherBoundary&b,const u32*args,u32 count)noexcept{return function_e3e0(m,b,0x65u,args,count);}
// EAF1..EBB5 (197): slot setup/control; original EAC8 null dereference remains unguarded.
void function_eaf1(Machine&m,DispatcherBoundary&b,u32 a1,u32 a2,u32 a3,u32 a4,u32 a5)noexcept{const bool prior=m.interrupt_enable;m.interrupt_enable=false;if(a1<16){store32(m.data+0x52e2,0xffffffffu);const u32 record=function_eac8(m,b,a1);const u32 divisor=load32(m.data+record+0x20u);if(divisor!=0xffffffffu){const u32 target=function_e3b4(m,0x67u,a1);if(target!=0&&b.allocate_timer){const u32 timer=b.allocate_timer(target);store32(m.data+0x5256u+a1*4u,timer);store32(m.data+0x52e2,timer);if(b.set_timer_rate)b.set_timer_rate(timer,divisor);}}const u32 args[5]={a1,a2,a3,a4,a5};(void)function_e3e0(m,b,0x66u,args,5);store32(m.data+0x5296u+a1*4u,1);const u32 timer=load32(m.data+0x52e2);if(timer!=0xffffffffu&&b.enable_timer)b.enable_timer(timer);}m.interrupt_enable=prior;}
// EBB6..EBF2 (61): clear pending state, release timer, then event 68h tail-dispatch.
[[nodiscard]]u32 function_ebb6(Machine&m,DispatcherBoundary&b,u32 slot,u32 argument)noexcept{if(slot>=16)return 0;const u32 i=slot*4u;const u32 pending=load32(m.data+0x5296u+i);store32(m.data+0x5296u+i,0);if(pending==0)return 0;const u32 timer=load32(m.data+0x5256u+i);if(timer!=0xffffffffu&&b.release_timer)b.release_timer(timer);const u32 args[2]={slot,argument};return function_e3e0(m,b,0x68u,args,2);}

// EBF3..EDD2 (480): 48 exact 10-byte `mov eax,event ; jmp E3E0` wrappers.
constexpr u32 kWrapperEvents[48]={0x78,0x79,0x86,0x7a,0x7b,0x85,0x7c,0x7d,0x7e,0x7f,0x80,0x81,0x82,0x83,0x84,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,0xaa,0xab,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,0xc0,0xc1,0xc2};
[[nodiscard]]u32 function_ebf3_edd2(Machine&m,DispatcherBoundary&b,u32 wrapper_index,const u32*args,u32 count)noexcept{return wrapper_index<48?function_e3e0(m,b,kWrapperEvents[wrapper_index],args,count):0;}
// EDD3..EDDF is 13 bytes of zero data and is deliberately not counted as code.
// EDE0..EDE6 (7): push ebp; mov ebp,esp; mov esp,ebp; pop ebp; ret.
void function_ede0()noexcept{}

static_assert(0xE4C7u-0xE3B4u==275u);static_assert(0xEBF3u-0xEA0Bu==488u);static_assert(0xEDD3u-0xEBF3u==480u);static_assert(0xEDE7u-0xEDE0u==7u);
constexpr u32 kStagedInstructionAccountedBytes=1250u;
} // namespace code1_staging
