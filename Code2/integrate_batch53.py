from pathlib import Path
import base64,zlib,re

root=Path('.')
cpp=root/'Code2/Code2.cpp'
ledger=root/'Code2/RESIDUAL_LEDGER.md'
progress=root/'Code2/PROGRESS.md'
addendum=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
payload=root/'Code2/batch53.zb64'

c=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 159155u;' in c
assert 'constexpr u32 kVerifiedNonCodeBytes = 2917u;' in c
assert 'function_26018' not in c
c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 159155u;',
            'constexpr u32 kVerifiedTranslatedBytes = 164226u;',1)
c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 2917u;',
            'constexpr u32 kVerifiedNonCodeBytes = 3457u;',1)

pat=re.compile(r"static_assert\(kVerifiedTranslatedBytes == 154218u \+.*?\(0x25DFAu-0x25D24u\)\);",re.S)
m=pat.search(c)
assert m, 'Batch52 global aggregate not found'
newagg='''static_assert(kVerifiedTranslatedBytes == 159155u +
              (0x273E7u-0x26018u));'''
c=c[:m.start()]+newagg+c[m.end():]

end='\n} // namespace code2\n'
assert c.endswith(end)
batch=zlib.decompress(base64.b64decode(payload.read_text())).decode().rstrip()+'\n'
c=c[:-len(end)]+'\n'+batch+end
assert 'function_26018' in c and '==5071u);' in c
cpp.write_text(c)

l=ledger.read_text()
anchor='25DFA..25DFB | alignment | 2\n'
assert anchor in l
line='25DFC..26017 | table | 540\n'
assert line not in l
l=l.replace(anchor,anchor+line,1)
assert '**Total proven non-code/data:** **2,917 bytes**.' in l
l=l.replace('**Total proven non-code/data:** **2,917 bytes**.',
            '**Total proven non-code/data:** **3,457 bytes**.',1)
evidence='''

Batch 53 evidence: `25DFC..26017` is the exact 135-entry CS dword jump table selected by `function_26018`. The dispatcher subtracts `A7h`, rejects unsigned indices above `86h`, and jumps through `JMP DWORD PTR CS:[EBP*4+25DFCh]`, proving an exact 135-dword ownership span. All table entries were decoded from immutable CODE.2 and retained verbatim in `kApp53Table25dfc`; they resolve to the address-distinct item handlers, four special resource/item handlers, or the shared `273DA` cleanup. The nine bytes `273E7..273EF` are deliberately left unresolved; the next independently decoded routine begins at `273F0`. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.'''
l=l.rstrip()+evidence+'\n'
ledger.write_text(l)

p=progress.read_text()
p=p.replace('object%20accounting-67.62%25-red','object%20accounting-69.96%25-red',1)
p=p.replace('**Verified translated instruction bytes:** **159,155**',
            '**Verified translated instruction bytes:** **164,226**',1)
p=p.replace('**Proven non-code/data bytes:** **2,917**',
            '**Proven non-code/data bytes:** **3,457**',1)
p=p.replace('**Reconciled object bytes:** **162,072 / 239,698 = 67.6151% (67.62%)**',
            '**Reconciled object bytes:** **167,683 / 239,698 = 69.9559% (69.96%)**',1)
p=p.replace('**Unresolved object bytes:** **77,626**',
            '**Unresolved object bytes:** **72,015**',1)
checkpoint='''

## CODE.2 Batch 53 checkpoint

Batch 53 adds **5,071 translated instruction bytes** plus **540 proven table bytes**, bringing cumulative accounting to **164,226 translated + 3,457 non-code = 167,683 / 239,698 = 69.9559% (69.96%)**; unresolved is **72,015 bytes**.

The sequential frontier now reaches `273E6`. `25DFC..26017` is the exact 135-entry CS dword table owned by `26018`; `273E7..273EF` remains unresolved and the next independently decoded routine begins at `273F0`. Preserve `26018`'s cooldown early-return that skips `19CF4`, the load-before-`-1` item-kind override, category-`0Eh` table ownership, AL-only peer-slot XORs, live global reloads after opaque group-effect calls, low-word fixed-field extraction used for `9*x` delays, the partial-word status test at item `42h`, and item `73h`'s ten-argument pre-pushed `B02C` frame. Semantic audit remains not started.
'''
p=p.rstrip()+checkpoint+'\n'
progress.write_text(p)

a=addendum.read_text().rstrip()+''' 

## CODE.2 Batch 53 checkpoint
Batch 53 adds **5,071 translated bytes** and **540 proven table bytes**, bringing cumulative accounting to **164,226 translated + 3,457 non-code = 167,683 / 239,698 = 69.9559% (69.96%)**. The sequential frontier reaches `273E7`; the nine bytes through `273EF` remain unresolved and the next independently decoded routine begins at `273F0`. Preserve the exact 135-entry `25DFC` table, `26018` cooldown early-return, load-before-`-1` kind override, AL-only peer-slot XORs, post-call global reloads, partial-word status test, and the item-`73h` pre-pushed `B02C` frame.
'''+ '\n'
addendum.write_text(a)
