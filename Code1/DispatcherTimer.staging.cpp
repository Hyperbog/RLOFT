/* Temporary verification staging for CODE.1.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 *
 * Address-named, instruction-accounted staging only.  This file does not
 * increase the official coverage counter until folded into Code1.cpp.
 * Ranges represented: E3B4..E4C6 and EA0B..EBF2.
 */
#include <cstdint>
namespace code1_staging {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
static u16 load16(const u8*p)noexcept{return static_cast<u16>(p[0]|(static_cast<u16>(p[1])<<8));}
static u32 load32(const u8*p)noexcept{return static_cast<u32>(p[0]|(static_cast<u32>(p[1])<<8)|(static_cast<u32>(p[2])<<16)|(static_cast<u32>(p[3])<<24));}
static void store16(u8*p,u16 v)noexcept{p[0]=static_cast<u8>(v);p[1]=static_cast<u8>(v>>8);}
static void store32(u8*p,u32 v)noexcept{p[0]=static_cast<u8>(v);p[1]=static_cast<u8>(v>>8);p[2]=static_cast<u8>(v>>16);p[3]=static_cast<u8>(v>>24);}
struct Machine{u8*data=nullptr;bool interrupt_enable=false;};
struct DispatcherBoundary{
    u32(*invoke)(u32 target,const u32*args,u32 count)=nullptr;
    void(*invoke_timer_callback)(u32 target)=nullptr;
    void(*out8)(u16 port,u8 value)=nullptr;
    u32(*allocate_timer)(u32 callback)=nullptr;
    void(*release_timer)(u32 slot)=nullptr;
    void(*set_timer_rate)(u32 slot,u32 divisor)=nullptr;
    void(*enable_timer)(u32 slot)=nullptr;
};

// E3B4..E3DF (44): EBX=slot, EAX=event.  Each slot points at an
// {event,target} dword table terminated by event FFFFFFFFh.
[[nodiscard]]u32 function_e3b4(Machine&m,u32 event,u32 slot)noexcept{
    if(slot>=16)return 0;
    u32 entry=load32(m.data+0x5216u+slot*4u);
    if(entry==0)return 0;
    for(;;entry+=8u){
        const u32 key=load32(m.data+entry);
        if(key==event)return load32(m.data+entry+4u);
        if(key==0xffffffffu)return 0;
    }
}

// E3E0..E3F1 (18): resolve the event for args[0] (the first stack
// argument/slot) and tail-transfer to the resolved target when nonzero.
[[nodiscard]]u32 function_e3e0(Machine&m,DispatcherBoundary&b,u32 event,const u32*args,u32 count)noexcept{
    if(count==0)return 0;
    const u32 target=function_e3b4(m,event,args[0]);
    return target&&b.invoke?b.invoke(target,args,count):0;
}

enum class Irq8Action:u8{Handled,ChainSavedVector};
struct Irq8Result{Irq8Action action=Irq8Action::Handled;u16 selector=0;u32 offset=0;};

// E3F2..E4C6 (213): INT 8 handler.  Slots 0..16 whose state word is 2
// accumulate the global step at 51FEh.  On period expiry the period is
// subtracted once and the callback at 5110h is invoked.  Slot 16 clears the
// reentrancy word before its callback.  A reentrant entry chains to the saved
// vector at 5206:5202 instead of issuing the PIC EOI locally.
[[nodiscard]]Irq8Result function_e3f2(Machine&m,DispatcherBoundary&b)noexcept{
    if(load16(m.data+0x510e)!=0){
        return{Irq8Action::ChainSavedVector,static_cast<u16>(load32(m.data+0x5206)),load32(m.data+0x5202)};
    }
    store16(m.data+0x510e,1);
    store32(m.data+0x520a,0);
    for(u32 slot=0;slot<=16;++slot){
        if(load16(m.data+0x5154u+slot*2u)==2){
            const u32 i=slot*4u;
            u32 accumulated=load32(m.data+0x5176u+i)+load32(m.data+0x51fe);
            const u32 period=load32(m.data+0x51bau+i);
            if(accumulated>=period){
                accumulated-=period;
                store32(m.data+0x5176u+i,accumulated);
                if(slot>=16)store16(m.data+0x510e,0);
                if(b.invoke_timer_callback)b.invoke_timer_callback(load32(m.data+0x5110u+i));
            }else{
                store32(m.data+0x5176u+i,accumulated);
            }
        }
        store32(m.data+0x520a,slot+1u);
    }
    store16(m.data+0x510e,0);
    if(b.out8)b.out8(0x20,0x20);
    return{};
}

// EA9A..EAC7 (46): clear slot table entry when slot < 16, preserving IF.
void function_ea9a(Machine&m,u32 slot)noexcept{
    const bool prior=m.interrupt_enable;m.interrupt_enable=false;
    if(slot<16)store32(m.data+0x5216u+slot*4u,0);
    m.interrupt_enable=prior;
}

// EAC8..EAE6 (31): event 64h receives stack arguments {slot,E9F2h}.
[[nodiscard]]u32 function_eac8(Machine&m,DispatcherBoundary&b,u32 slot)noexcept{
    const u32 args[2]={slot,0x0000e9f2u};
    return function_e3e0(m,b,0x64u,args,2);
}

// EA0B..EA99 (143): find a free slot; require descriptor+4 to contain
// 79706F43h (bytes "Copy"); install descriptor[0]; call EAC8; accept the
// slot only if the returned record exists and returned[0] <= [52E8] unsigned.
// descriptor and EAC8's return are original data-group offsets.
[[nodiscard]]u32 function_ea0b(Machine&m,DispatcherBoundary&b,u32 descriptor)noexcept{
    const bool prior=m.interrupt_enable;m.interrupt_enable=false;
    u32 slot=0;
    while(slot<16&&load32(m.data+0x5216u+slot*4u)!=0)++slot;
    u32 result=0xffffffffu;
    if(slot<16&&load32(m.data+descriptor+4u)==0x79706f43u){
        store32(m.data+0x5216u+slot*4u,load32(m.data+descriptor));
        const u32 returned=function_eac8(m,b,slot);
        if(returned!=0&&load32(m.data+returned)<=load32(m.data+0x52e8u))result=slot;
    }
    m.interrupt_enable=prior;
    return result;
}

// EAE7..EAF0 (10): event 65h is a pure tail-dispatch wrapper.  The caller
// supplies the unchanged original stack arguments because this wrapper does
// not establish its own frame.
[[nodiscard]]u32 function_eae7(Machine&m,DispatcherBoundary&b,const u32*args,u32 count)noexcept{
    return function_e3e0(m,b,0x65u,args,count);
}

// EAF1..EBB5 (197): slot setup/control.  EAC8 supplies a record whose +20h
// dword is a divisor/sentinel.  Event 67h may supply a timer callback; event
// 66h receives the five original arguments.  Names remain address-based.
void function_eaf1(Machine&m,DispatcherBoundary&b,u32 a1,u32 a2,u32 a3,u32 a4,u32 a5)noexcept{
    const bool prior=m.interrupt_enable;m.interrupt_enable=false;
    if(a1<16){
        store32(m.data+0x52e2,0xffffffffu);
        const u32 record=function_eac8(m,b,a1);
        const u32 divisor=load32(m.data+record+0x20u); // original has no null guard
        if(divisor!=0xffffffffu){
            const u32 target=function_e3b4(m,0x67u,a1);
            if(target!=0&&b.allocate_timer){
                const u32 timer=b.allocate_timer(target);
                store32(m.data+0x5256u+a1*4u,timer);
                store32(m.data+0x52e2,timer);
                if(b.set_timer_rate)b.set_timer_rate(timer,divisor);
            }
        }
        const u32 args[5]={a1,a2,a3,a4,a5};
        (void)function_e3e0(m,b,0x66u,args,5);
        store32(m.data+0x5296u+a1*4u,1);
        const u32 timer=load32(m.data+0x52e2);
        if(timer!=0xffffffffu&&b.enable_timer)b.enable_timer(timer);
    }
    m.interrupt_enable=prior;
}

// EBB6..EBF2 (61): atomically clear slot pending state.  If it had been set,
// release its assigned timer (unless FFFFFFFFh), then tail-dispatch event 68h
// with the two original arguments.
[[nodiscard]]u32 function_ebb6(Machine&m,DispatcherBoundary&b,u32 slot,u32 argument)noexcept{
    if(slot>=16)return 0;
    const u32 i=slot*4u;
    const u32 pending=load32(m.data+0x5296u+i);
    store32(m.data+0x5296u+i,0);
    if(pending==0)return 0;
    const u32 timer=load32(m.data+0x5256u+i);
    if(timer!=0xffffffffu&&b.release_timer)b.release_timer(timer);
    const u32 args[2]={slot,argument};
    return function_e3e0(m,b,0x68u,args,2);
}

static_assert(0xE4C7u-0xE3B4u==275u);
static_assert(0xEBF3u-0xEA0Bu==488u);
constexpr u32 kStagedInstructionAccountedBytes=763u;
} // namespace code1_staging
