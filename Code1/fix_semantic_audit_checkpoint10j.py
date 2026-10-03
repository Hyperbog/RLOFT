from pathlib import Path

cpp = Path('Code1/Code1.cpp')
s = cpp.read_text()
assert 'Semantic audit checkpoint 10j' not in Path('Code1/FINAL_BINARY_AUDIT.md').read_text()

start = s.index('[[nodiscard]]u8* function_dea7(')
end = s.index('void function_df13', start)
new_dea7 = r'''[[nodiscard]]u8* function_dea7(u8*dst,u32 pattern,u32 dword_count)noexcept{
 u8*p=dst;
 if(dword_count==0u)return p;
 while((reinterpret_cast<std::uintptr_t>(p)&31u)!=0u){store_le32(p,pattern);p+=4u;if(--dword_count==0u)return p;}
 const u32 saved=dword_count;const u32 groups=dword_count>>2;
 for(u32 group=0;group<groups;++group){
  // DECE performs a discarded CMP [EAX+20h],DL while filling each odd
  // 16-byte group when another group remains. Preserve that memory touch.
  if((group&1u)!=0u&&group+1u<groups){const volatile u8 prefetched=*reinterpret_cast<volatile const u8*>(p+0x10u);(void)prefetched;}
  store_le32(p+0u,pattern);store_le32(p+4u,pattern);store_le32(p+8u,pattern);store_le32(p+12u,pattern);p+=0x10u;
 }
 u32 tail=saved&3u;while(tail!=0u){store_le32(p,pattern);p+=4u;--tail;}return p;
}
static_assert(0xDF13u-0xDEA7u==108u);
'''
s = s[:start] + new_dea7 + s[end:]

start = s.index('struct Int24Frame12{')
marker = 'static_assert(0xE360u-0xE2C3u==157u);'
end = s.index(marker, start) + len(marker)
new_e2 = r'''struct Int24Frame12{u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0;u16 ds=0,es=0,ss=0;u32 esp=0;};
struct Int24Boundary12{u16(*restore_ds_cec0)()=nullptr;u32(*far_callback)(u32,u32,u32,u32)=nullptr;};
// E2C3..E35F: IRET trampoline represented as an interrupt frame. The far callback receives the four encoded cdecl dwords: low16(saved EAX), low16(saved EDI), the BP:SI linearized address, and the selected segment value. Original DS/ES are restored by the final POPs.
void function_e2c3(Machine&m,Int24Boundary12&b,Int24Frame12&f)noexcept{
 const u16 incoming_ds=f.ds,incoming_es=f.es;const u16 work_ds=b.restore_ds_cec0?b.restore_ds_cec0():f.ds;f.ds=work_ds;f.es=work_ds;
 store_le32(dgroup_ptr(m,dgroup(0x50fcu)),f.esp);store_le16(dgroup_ptr(m,dgroup(0x5100u)),f.ss);const u32 old_top=load_le32(dgroup_ptr(m,dgroup(0x5300u)));store_le32(dgroup_ptr(m,dgroup(0x5108u)),old_top);store_le32(dgroup_ptr(m,dgroup(0x5300u)),0x8a68u);f.ss=work_ds;f.esp=0x8e68u;
 u32 linear=(f.ebp&0xffffu)<<4;linear=(linear&0xffff0000u)|static_cast<u16>(static_cast<u16>(linear)+static_cast<u16>(f.esi));
 const u8 mode=*dgroup_ptr(m,dgroup(0x531eu));u16 sel=0x34u;if(mode==1u)sel=work_ds;
 const u32 ret=b.far_callback?b.far_callback(f.eax&0xffffu,f.edi&0xffffu,linear,static_cast<u32>(sel)):0u;f.eax=ret;
 f.esp=load_le32(dgroup_ptr(m,dgroup(0x50fcu)));f.ss=load_le16(dgroup_ptr(m,dgroup(0x5100u)));store_le32(dgroup_ptr(m,dgroup(0x5300u)),load_le32(dgroup_ptr(m,dgroup(0x5108u))));f.ds=incoming_ds;f.es=incoming_es;
} static_assert(0xE360u-0xE2C3u==157u);'''
s = s[:start] + new_e2 + s[end:]
cpp.write_text(s)

audit = Path('Code1/FINAL_BINARY_AUDIT.md')
a = audit.read_text()
a += r'''

## Semantic audit checkpoint 10j — `DDE0..E3B2`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the low-level port/vector wrappers, Watcom fill and integer-conversion runtime, object clone/constructor helpers, INT 24h installation/trampoline, and signed-decimal parser through `E3B2`. The source object hash was reverified as `c0bf2c313aa8a24c9a0c0a3ce498f87a1d09299bb826413acaeb844412007203` before this pass.

`DEA7` retained the correct 32-byte alignment and unrolled dword-fill result but had normalized away an encoded read side effect. At `DECE` the binary executes `CMP BYTE PTR [EAX+20h],DL` before the next unrolled stores; its flags are subsequently overwritten, but the memory access itself is architecturally observable through fault/touch timing. The C++ now retains that look-ahead byte read at the exact odd-group points while preserving the original store order and tail count. `DDE0..DE35`, `DE70..DEA0`, `DF13`, and `DF73` otherwise rechecked cleanly.

The paired unsigned/signed radix converters `DFB3/DFFB` and `E050/E098` were rechecked instruction by instruction and against the Open Watcom runtime implementation in `bld/clib/convert/c/ltoa.c`. The compiler helper resets the remainder cell from the radix before each unsigned divide, matching the two address-distinct binary variants; the existing standard base conversion and decimal-only sign handling therefore remain correct and required no semantic change.

The major correction is the `E2C3` INT-24 IRET trampoline. The binary saves the interrupt frame, copies saved EAX into working EDX before restoring DS, then calls the stored far callback with exactly four cdecl dwords: `low16(saved EAX)`, `low16(saved EDI)`, the BP:SI-derived linear address, and the selected segment value (`DS` in mode 1, otherwise `34h`). The prior C++ exposed five callback arguments, incorrectly used saved EDX as the first argument, shifted the remaining slots, and left DS/ES changed after the callback. The corrected boundary uses the four encoded arguments and restores the interrupt caller's original DS and ES after restoring SS:ESP and the runtime stack-top global. `E280` installation logic and `E360..E3B2` decimal parsing rechecked without further changes.

`E0D0`, the `E174/E1F4/E26C` free wrappers, and the address-distinct `E190/E208` constructor-like readers were also rechecked directly. The known non-code islands inside the span (`DE6F`, `DEA1..DEA6`, the two 37-byte radix tables at `DF8E..DFB2` and `E02B..E04F`, `E0C8..E0CF`, `E187..E18F`, `E1F1..E1F3`, `E207`, `E269..E26B`, and `E27F`) remain unchanged. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `E3B2`; `E3B3` is the verified one-byte zero-alignment boundary and the next code begins at `E3B4`.
'''
audit.write_text(a)
print('applied semantic audit checkpoint 10j')
