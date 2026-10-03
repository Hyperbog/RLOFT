from pathlib import Path

cpp_path=Path('Code1/Code1.cpp')
cpp=cpp_path.read_text()

old="[[nodiscard]]u32 function_e3e0(Machine&m,DispatcherBoundary&b,u32 event,const u32*args,u32 count)noexcept{if(count==0)return 0;const u32 target=function_e3b4(m,event,args[0]);return target&&b.invoke?b.invoke(target,args,count):0;}"
new="[[nodiscard]]u32 function_e3e0(Machine&m,DispatcherBoundary&b,u32 event,const u32*args,u32 count)noexcept{const u32 target=function_e3b4(m,event,args[0]);return target&&b.invoke?b.invoke(target,args,count):0;}"
assert old in cpp
cpp=cpp.replace(old,new,1)

start=cpp.index('enum class Irq8Action:u8{Handled,ChainSavedVector};')
end=cpp.index('// E4C7..EA0A: verified timer/PIT setup and slot-state routines.')
irq_block=r'''enum class Irq8Action:u8{Handled,ChainSavedVector};
struct Irq8Result{Irq8Action action=Irq8Action::Handled;u16 selector=0;u32 offset=0;};
// E3F2..E4A6: IRQ-8 body. A reentrant entry does not chain; it skips the timer scan,
// acknowledges the PIC at E4A0, restores the saved frame, and IRETs.
[[nodiscard]]Irq8Result function_e3f2(Machine&m,DispatcherBoundary&b)noexcept{
 if(load_le16(m.data+0x510e)!=0){if(b.out8)b.out8(0x20,0x20);return{};}
 store_le16(m.data+0x510e,1);store_le32(m.data+0x520a,0);
 for(u32 slot=0;slot<=16;++slot){
  if(load_le16(m.data+0x5154u+slot*2u)==2){
   const u32 i=slot*4u;
   u32 accumulated=load_le32(m.data+0x5176u+i)+load_le32(m.data+0x51fe);
   const u32 period=load_le32(m.data+0x51bau+i);
   if(accumulated>=period){
    accumulated-=period;store_le32(m.data+0x5176u+i,accumulated);
    if(slot>=16)store_le16(m.data+0x510e,0);
    if(b.invoke_timer_callback)b.invoke_timer_callback(load_le32(m.data+0x5110u+i));
   }else store_le32(m.data+0x5176u+i,accumulated);
  }
  store_le32(m.data+0x520a,slot+1u);
 }
 store_le16(m.data+0x510e,0);if(b.out8)b.out8(0x20,0x20);return{};
}
// E4A7..E4C6 is the distinct far-chain trampoline stored at 5150 by E514. The machine
// swaps the saved ECX/EAX frame slots with the old INT-8 offset/selector, restores the
// remaining saved registers, and RETFs through that substituted far pointer. The host
// frame abstraction already leaves the interrupted register values intact, so the
// observable boundary payload is the far target selected by those XCHG instructions.
[[nodiscard]]Irq8Result function_e4a7(Machine&m)noexcept{return{Irq8Action::ChainSavedVector,static_cast<u16>(load_le32(m.data+0x5206u)),load_le32(m.data+0x5202u)};}
static_assert(0xE3E0u-0xE3B4u==44u);
static_assert(0xE3F2u-0xE3E0u==18u);
static_assert(0xE4A7u-0xE3F2u==181u);
static_assert(0xE4C7u-0xE4A7u==32u);

'''
cpp=cpp[:start]+irq_block+cpp[end:]

old_ea="[[nodiscard]]u32 function_ea0b(Machine&m,DispatcherBoundary&b,u32 descriptor)noexcept{const bool prior=irq_save_cli(m);u32 slot=0;while(slot<16&&load_le32(m.data+0x5216u+slot*4u)!=0)++slot;u32 result=0xffffffffu;if(slot<16&&load_le32(m.data+descriptor+4u)==0x79706f43u){store_le32(m.data+0x5216u+slot*4u,load_le32(m.data+descriptor));const u32 returned=function_eac8(m,b,slot);if(returned!=0&&load_le32(m.data+returned)<=load_le32(m.data+0x52e8u))result=slot;}irq_restore(m,prior);return result;}"
new_ea=r'''[[nodiscard]]u32 function_ea0b(Machine&m,DispatcherBoundary&b,u32 descriptor)noexcept{
 const bool prior=irq_save_cli(m);store_le32(m.data+0x52dau,0u);u32 result=0xffffffffu;
 for(;;){
  const u32 slot=load_le32(m.data+0x52dau);const u32 i=slot*4u;
  if(load_le32(m.data+0x5216u+i)==0u){
   if(load_le32(m.data+descriptor+4u)==0x79706f43u){
    store_le32(m.data+0x5216u+i,load_le32(m.data+descriptor));
    const u32 returned=function_eac8(m,b,slot);
    if(returned!=0u&&load_le32(m.data+returned)<=load_le32(m.data+0x52e8u))result=load_le32(m.data+0x52dau);
   }
   break;
  }
  store_le32(m.data+0x52dau,slot+1u);
  if(load_le32(m.data+0x52dau)==16u)break;
 }
 irq_restore(m,prior);return result;
}'''
assert old_ea in cpp
cpp=cpp.replace(old_ea,new_ea,1)

needle="[[nodiscard]]u32 function_ebf3_edd2(Machine&m,DispatcherBoundary&b,u32 wrapper_index,const u32*args,u32 count)noexcept{return wrapper_index<48?function_e3e0(m,b,kWrapperEvents[wrapper_index],args,count):0;}\nvoid function_ede0()noexcept{}"
assert needle in cpp
addresses=[0xebf3+10*i for i in range(48)]
events=[0x78,0x79,0x86,0x7a,0x7b,0x85,0x7c,0x7d,0x7e,0x7f,0x80,0x81,0x82,0x83,0x84,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,0xaa,0xab,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,0xc0,0xc1,0xc2]
lines=["[[nodiscard]]u32 function_ebf3_edd2(Machine&m,DispatcherBoundary&b,u32 wrapper_index,const u32*args,u32 count)noexcept{return wrapper_index<48?function_e3e0(m,b,kWrapperEvents[wrapper_index],args,count):0;}","// EBF3..EDD2: preserve all 48 address-distinct MOV EAX,event / JMP E3E0 entry points."]
for a,e in zip(addresses,events):
    lines.append(f"[[nodiscard]]u32 function_{a:x}(Machine&m,DispatcherBoundary&b,const u32*args,u32 count)noexcept{{return function_e3e0(m,b,0x{e:x}u,args,count);}}")
lines.append('static_assert(0xEDD3u-0xEBF3u==480u);')
lines.append('// EDD3..EDDF is the verified thirteen-byte zero-alignment island.')
lines.append('void function_ede0()noexcept{}')
lines.append('static_assert(0xEDE7u-0xEDE0u==7u);')
cpp=cpp.replace(needle,'\n'.join(lines),1)

cpp_path.write_text(cpp)

audit=Path('Code1/FINAL_BINARY_AUDIT.md')
s=audit.read_text()
entry=r'''

## Semantic audit checkpoint 10k — `E3B4..EDE6`

Fresh direct disassembly of immutable `CODE.1` reconciled the callback dispatcher, IRQ-8 handler/chain trampoline, timer/PIT subsystem, registration/event subsystem, all 48 fixed event wrappers, and the seven-byte `EDE0` frame no-op. The source and extracted-object hashes were reverified before comparison.

`E3E0` is a pure stack-forwarding dispatcher: the machine always reads the first caller dword at `[ESP+4]` to select the slot and has no encoded argument-count precheck. The C++ no longer invents a zero-count early return.

The largest correction is the split between `E3F2` and `E4A7`. A reentrant `E3F2` entry (`WORD 510E != 0`) jumps directly to the common restore/EOI/IRET tail; it does **not** chain to the previous INT-8 vector. The old-vector chain is the separate `E4A7..E4C6` far trampoline whose address is stored at `5150` by `E514`. That trampoline loads the saved vector from `5202:5206`, swaps those values into the saved ECX/EAX frame slots, restores the remaining registers, and executes `RETF`. The reconstruction now represents `E4A7` as its own address-distinct chain boundary and leaves reentrant `E3F2` on the encoded handled/EOI path.

`EA0B` was also corrected to preserve DGROUP `52DA` as the actual registration-slot scan cursor. The binary initializes it to zero, increments the global after each occupied slot, leaves it at 16 on exhaustion, and returns the global value after a successful descriptor/version check; the former C++ used only a local loop variable and lost those observable writes.

Finally, `EBF3..EDD2` is no longer represented solely by a normalized indexed helper. All 48 ten-byte `MOV EAX,event ; JMP E3E0` thunks now have address-distinct C++ entry points while retaining the verified event sequence. `E4C7..EA0A`, `EA9A..EBF2`, and `EDE0..EDE6` otherwise rechecked cleanly; `EDD3..EDDF` remains the known thirteen-byte zero-alignment island. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `EDE6`; the next code begins at `EDE7`.
'''
assert '## Semantic audit checkpoint 10k' not in s
s+=entry
audit.write_text(s)
print('applied semantic audit checkpoint 10k')
