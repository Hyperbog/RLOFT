from pathlib import Path
import re

cpp=Path('Code2/Code2.cpp')
s=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 34466u;' in s
assert 'constexpr u32 kVerifiedNonCodeBytes = 295u;' in s
assert 'app10_load_tail_06cad' not in s
frag=Path('Code2/batch10.inc').read_text()
marker='\n} // namespace code2\n'
assert s.count(marker)==1
s=s.replace('constexpr u32 kVerifiedTranslatedBytes = 34466u;','constexpr u32 kVerifiedTranslatedBytes = 36959u;')
s=s.replace('constexpr u32 kVerifiedNonCodeBytes = 295u;','constexpr u32 kVerifiedNonCodeBytes = 305u;')
pat=re.compile(r'static_assert\(kVerifiedTranslatedBytes ==.*?\);',re.S)
m=pat.search(s);assert m
new='''static_assert(kVerifiedTranslatedBytes == 34466u +\n              (0x0706Eu-0x06CADu) + (0x0719Cu-0x07070u) +\n              (0x071B8u-0x0719Cu) + (0x07376u-0x071B8u) +\n              (0x0739Du-0x07378u) + (0x075C9u-0x073A0u) +\n              (0x079E0u-0x07938u));'''
s=s[:m.start()]+new+s[m.end():]
s=s.replace(marker,'\n'+frag.rstrip()+'\n'+marker)
cpp.write_text(s)

p=Path('Code2/PROGRESS.md');t=p.read_text()
t=t.replace('object%20accounting-14.50%25-red','object%20accounting-15.55%25-red')
t=t.replace('**Verified translated instruction bytes:** **34,466**','**Verified translated instruction bytes:** **36,959**')
t=t.replace('**Proven non-code/data bytes:** **295**','**Proven non-code/data bytes:** **305**')
t=t.replace('**Reconciled object bytes:** **34,761 / 239,698 = 14.5020% (14.50%)**','**Reconciled object bytes:** **37,264 / 239,698 = 15.5462% (15.55%)**')
t=t.replace('**Unresolved object bytes:** **204,937**','**Unresolved object bytes:** **202,434**')
section='''## Batch 10 — save-load shared tail, resource panel, background renderer, and wildcard file copy

Added **2,493 translated instruction bytes** and **10 directly proven alignment bytes**, bringing cumulative accounting to **36,959 translated + 305 non-code = 37,264 / 239,698 = 15.5462% (15.55%)**.

### Newly translated ranges

- `06CAD..0706D` — **961 bytes**: backward-jump shared load tail owned by `07938`; seeks past the 32h-byte header, restores core state, party records, 62 variable descriptor payloads, ten 248h-byte blocks, runtime flags, and post-load derived state.
- `07070..0719B` — **300 bytes**: ten numbered 32h-byte label/resource slots with fallback strings, resource creation, draw placement, and text placement.
- `0719C..071B7` — **28 bytes**: ten-resource destructor over `12B30..12B54`.
- `071B8..07375` — **446 bytes**: randomized tiled background, edge/corner variants, numbered resource overlay, page publish/toggle, and final page selection.
- `07378..0739C` — **37 bytes**: FCB delete preparation and DOS INT 21h/AH=13 boundary.
- `073A0..075C8` — **553 bytes**: wildcard file-copy loop with 2Ch-byte local find state, full-file allocation/read, destination replacement/create/write, exact error cleanup, and find-next continuation.
- `07938..079DF` — **168 bytes**: save pathname construction/open and backward transfer into the `06CAD` shared tail.

### Newly proven non-code/data

`0706E..0706F`, `07376..07377`, `0739D..0739F`, and `075C9..075CB` total **10 bytes** of decoded compiler no-op alignment. A full object-1 direct relative branch/call-target scan found no incoming target to these bytes.

### Fidelity notes

`06CAD` is not a callable standalone function: `07938` opens the save file and jumps backward into it with EBX and ESI both carrying the handle. The loader restores the saved level only after forcing `C6C8=-1`, and it reconstructs per-party bits 4/2/1 from signed class bytes after the raw 920h-byte party read. The wildcard copier preserves the original asymmetry: source reads are not short-read checked, while destination writes must equal the full file size or the routine returns failure after freeing/closing resources.

The zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.

### Next high-value targets

1. Continue the sequential application path at `075CC`.
2. Reconcile startup at `353A0` as a CODE.2-specific path.
3. Continue harvesting independently proved relocated CODE.1 runtime families.'''
p.write_text(t.rstrip()+'\n\n'+section+'\n')

led=Path('Code2/RESIDUAL_LEDGER.md');r=led.read_text()
needle='<!-- RESIDUAL-RANGES-END -->'
assert needle in r
insert='''0706E..0706F | alignment | 2
07376..07377 | alignment | 2
0739D..0739F | alignment | 3
075C9..075CB | alignment | 3
'''
r=r.replace(needle,insert+needle)
r=r.replace('**Total proven non-code/data:** **295 bytes**.','**Total proven non-code/data:** **305 bytes**.')
r=r.rstrip()+'''\n\nBatch 10 evidence: `0706E..0706F`, `07376..07377`, `0739D..0739F`, and `075C9..075CB` decode as explicit compiler no-op alignment between directly reconstructed routine boundaries. A full object-1 direct relative branch/call target scan found no incoming target to any of these bytes. The known zero islands remain unresolved.\n'''
led.write_text(r)

add=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md');a=add.read_text()
checkpoint='''## CODE.2 Batch 10 checkpoint

Batch 10 added **2,493 translated bytes** plus **10 proven non-code/data bytes**. Cumulative accounting is **36,959 translated + 305 non-code = 37,264 / 239,698 = 15.5462% (15.55%)**; unresolved is **202,434 bytes**.

The sequential frontier reaches `075C8`; `075C9..075CB` is proven alignment and the next routine starts at `075CC`. `07938..079DF` was also translated because it owns the backward-jump shared load tail at `06CAD..0706D`. Preserve that ownership relationship: `06CAD` is not an independent callable function. The wildcard copier at `073A0` ignores source short-read results but requires the destination write count to equal the complete file size. Known zero islands remain unresolved.'''
add.write_text(a.rstrip()+'\n\n'+checkpoint+'\n')
