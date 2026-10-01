/* Temporary verification staging for CODE.1.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 *
 * These routines are kept address-named.  This file is temporary and will be
 * folded into Code1.cpp; it does not change the official coverage counter by
 * itself.  Ranges represented here: E3B4..E3F1 and EA0B..EAE6.
 */
#include <cstdint>
namespace code1_staging {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;
static u16 load16(const u8*p)noexcept{return static_cast<u16>(p[0]|(static_cast<u16>(p[1])<<8));}
static u32 load32(const u8*p)noexcept{return static_cast<u32>(p[0]|(static_cast<u32>(p[1])<<8)|(static_cast<u32>(p[2])<<16)|(static_cast<u32>(p[3])<<24));}
static void store32(u8*p,u32 v)noexcept{p[0]=static_cast<u8>(v);p[1]=static_cast<u8>(v>>8);p[2]=static_cast<u8>(v>>16);p[3]=static_cast<u8>(v>>24);}
struct Machine{u8*data=nullptr;bool interrupt_enable=false;};
struct DispatcherBoundary{u32(*invoke)(u32 target,u32 slot,const u32*args,u32 count)=nullptr;};

// E3B4..E3DF (44 bytes): search one slot's {event,target} pairs.
// EBX supplies slot, EAX supplies event.  FFFFFFFF terminates the table.
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

// E3E0..E3F1 (18 bytes): resolve through E3B4 and tail-jump to the target.
[[nodiscard]]u32 function_e3e0(Machine&m,DispatcherBoundary&b,u32 event,u32 slot,const u32*args,u32 count)noexcept{
    const u32 target=function_e3b4(m,event,slot);
    return target&&b.invoke?b.invoke(target,slot,args,count):0;
}

// EA0B..EA99 (143 bytes): find a free 0..15 slot, require the literal
// dword 79706F43h (bytes "Copy") at descriptor+4, install descriptor[0],
// call EAC8, and accept only when returned[0] <= dword 52E8h unsigned.
[[nodiscard]]u32 function_ea0b(Machine&m,DispatcherBoundary&b,const u32*descriptor)noexcept;

// EA9A..EAC7 (46 bytes): clear slot table entry when slot < 16.
void function_ea9a(Machine&m,u32 slot)noexcept{
    const bool prior=m.interrupt_enable;m.interrupt_enable=false;
    if(slot<16)store32(m.data+0x5216u+slot*4u,0);
    m.interrupt_enable=prior;
}

// EAC8..EAE6 (31 bytes): dispatch event 64h with E9F2 and slot as the
// two stack arguments.  The boundary preserves the unresolved indirect call.
[[nodiscard]]u32 function_eac8(Machine&m,DispatcherBoundary&b,u32 slot)noexcept{
    const u32 args[1]={0x0000e9f2u};
    return function_e3e0(m,b,0x64u,slot,args,1);
}

[[nodiscard]]u32 function_ea0b(Machine&m,DispatcherBoundary&b,const u32*descriptor)noexcept{
    const bool prior=m.interrupt_enable;m.interrupt_enable=false;
    u32 slot=0;
    while(slot<16&&load32(m.data+0x5216u+slot*4u)!=0)++slot;
    u32 result=0xffffffffu;
    if(slot<16&&descriptor!=nullptr&&descriptor[1]==0x79706f43u){
        store32(m.data+0x5216u+slot*4u,descriptor[0]);
        const u32 returned=function_eac8(m,b,slot);
        if(returned!=0&&load32(m.data+returned)<=load32(m.data+0x52e8u))result=slot;
    }
    m.interrupt_enable=prior;
    return result;
}

static_assert(0xE3F2u-0xE3B4u==62u);
static_assert(0xEAE7u-0xEA0Bu==220u);
constexpr u32 kStagedInstructionAccountedBytes=282u;
} // namespace code1_staging
