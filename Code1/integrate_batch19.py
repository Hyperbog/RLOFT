from pathlib import Path
cpp=Path('Code1/Code1.cpp')
text=cpp.read_text()
assert 'function_de70(' not in text
assert 'constexpr u32 kVerifiedTranslatedBytes=69436u;' in text
marker='} // namespace code1'
assert marker in text
block=r'''

// Batch 19: DE70..DEA0 is the byte-aligning front end to the already integrated DEA7 dword fill core.
// 84A0 constructs EDX as four copies of the caller byte before entering this routine.
// The initial destination-byte read at DE74 is retained as an explicit touch even though TEST AL,3 overwrites its flags.
struct FillDe70Boundary{
 u8(*read8)(u32)=nullptr;
 void(*write8)(u32,u8)=nullptr;
 u32(*call_dea7)(u32,u32,u32)=nullptr;
};
[[nodiscard]]u32 function_de70(FillDe70Boundary&b,u32 dst,u32 pattern,u32 count)noexcept{
 if(count==0u)return dst;
 if(b.read8)(void)b.read8(dst);
 while((dst&3u)!=0u&&count!=0u){
  if(b.write8)b.write8(dst,static_cast<u8>(pattern));
  ++dst;
  pattern=(pattern>>8)|(pattern<<24);
  --count;
 }
 const u32 saved=count;
 const u32 dwords=count>>2;
 if(b.call_dea7)dst=b.call_dea7(dst,pattern,dwords);
 else dst+=dwords*4u;
 count=saved&3u;
 if(count==0u)return dst;
 if(b.write8)b.write8(dst,static_cast<u8>(pattern));
 if(--count==0u)return dst;
 if(b.write8)b.write8(dst+1u,static_cast<u8>(pattern>>8));
 if(--count==0u)return dst;
 if(b.write8)b.write8(dst+2u,static_cast<u8>(pattern));
 return dst;
}
static_assert(0xDEA1u-0xDE70u==49u);
// DE6F is the zero byte immediately preceding the call target; DEA1..DEA6 are six NOP alignment bytes.
'''
text=text.replace('constexpr u32 kVerifiedTranslatedBytes=69436u;','constexpr u32 kVerifiedTranslatedBytes=69485u;')
text=text.replace(marker,block+'\n'+marker)
cpp.write_text(text)

p=Path('Code1/PROGRESS.md');s=p.read_text()
s=s.replace('verified%20conversion-98.63%25-red','verified%20conversion-98.70%25-red')
s=s.replace('**Verified conversion:** 98.63%','**Verified conversion:** 98.70%')
s=s.replace('**Remaining:** 1.37%','**Remaining:** 1.30%')
s=s.replace('Current verified translated bytes: **69,436**.','Current verified translated bytes: **69,485**.')
s=s.replace('Raw coverage: **98.6279%**, displayed as **98.63%**.','Raw coverage: **98.6975%**, displayed as **98.70%**.')
needle='- Batch 18 self-modifying scaled VGA blitters: `C7A0..C8C0`, `C8C4..C9D1`, `C9D4..CAC9`, `CACC..CBD0`, with alignment islands excluded: **1,066 bytes**.\n'
assert needle in s
s=s.replace(needle,needle+'- Batch 19 fill front end `DE70..DEA0`: **49 bytes**. `DE6F` is a preceding zero byte and `DEA1..DEA6` are alignment NOPs.\n')
s=s.replace('Total integrated verified translated bytes: **69,436**.','Total integrated verified translated bytes: **69,485**.')
s=s.replace('`DE6F..DEA0` is not accepted as a proven ordinary function boundary and remains on the unresolved-boundary queue. It is not counted or treated as integration-ready.','`DE70..DEA0` is now proven by the direct call at `84B8` and integrated. `DE6F` is not part of the entry; it is the preceding zero byte. `DEA1..DEA6` are alignment NOPs.')
s=s.replace('`DE6F..DEA0` is an unresolved boundary and is deliberately outside that staging total.','`DE70..DEA0` is integrated; `DE6F` and `DEA1..DEA6` are classified non-code bytes.')
p.write_text(s)

for name in ['Code1/STAGING_LEDGER.md','Code1/DECOMPILATION_CONTINUITY.md']:
 q=Path(name);t=q.read_text();t+='\n\n## Batch 19 integration checkpoint\nResolved the old DE6F ambiguity: the real direct-call entry is **DE70**, reached by `84B8`; `DE6F` is the preceding zero byte. Integrated `DE70..DEA0` (**49 bytes**) as the byte-aligning front end to the existing `DEA7` dword-fill core, preserving the initial destination read, 32-bit pattern rotation during alignment, exact dword-count handoff, and 1/2/3-byte tail. `DEA1..DEA6` are alignment NOPs. Verified total: **69,485 / 70,402 = 98.70%**.\n'
 q.write_text(t)
print('integrated batch19')
