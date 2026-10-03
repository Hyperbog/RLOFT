from pathlib import Path
import re

cpp=Path('Code2/Code2.cpp')
s=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 20666u;' in s
assert 'constexpr u32 kVerifiedNonCodeBytes = 172u;' in s
assert 'void function_03634' not in s
frag=Path('Code2/batch06_part1.inc').read_text()+Path('Code2/batch06_part2.inc').read_text()
frag=frag.replace('    void (*call_1c40c)(u32,u32,u32,u32,u32)=nullptr;','    void (*call_1c40c)(u32,u32,u32,u32,u32)=nullptr;\n    void (*call_1a5b0)(u32,u32,u32)=nullptr;')
frag=frag.replace('a4.call_1a5b0','b.call_1a5b0')
marker='\n} // namespace code2\n'
assert s.count(marker)==1
s=s.replace('constexpr u32 kVerifiedTranslatedBytes = 20666u;','constexpr u32 kVerifiedTranslatedBytes = 24697u;')
s=s.replace('constexpr u32 kVerifiedNonCodeBytes = 172u;','constexpr u32 kVerifiedNonCodeBytes = 190u;')
pat=re.compile(r'static_assert\(kVerifiedTranslatedBytes ==.*?\);',re.S)
m=pat.search(s)
assert m
new='''static_assert(kVerifiedTranslatedBytes == 20666u +
              (0x0380Du-0x03634u) + (0x0381Fu-0x03810u) +
              (0x03836u-0x03820u) + (0x03A64u-0x03838u) +
              (0x03C03u-0x03A78u) + (0x03DE3u-0x03C04u) +
              (0x03EA4u-0x03DE4u) + (0x03F75u-0x03EA4u) +
              (0x04040u-0x03F78u) + (0x040FFu-0x04040u) +
              (0x04225u-0x04100u) + (0x043F1u-0x04228u) +
              (0x044CFu-0x043F4u) + (0x0461Au-0x044D0u));'''
s=s[:m.start()]+new+s[m.end():]
s=s.replace(marker,'\n'+frag+marker)
cpp.write_text(s)

p=Path('Code2/PROGRESS.md');t=p.read_text()
t=t.replace('object%20accounting-8.69%25-red','object%20accounting-10.38%25-red')
t=t.replace('**Verified translated instruction bytes:** **20,666**','**Verified translated instruction bytes:** **24,697**')
t=t.replace('**Proven non-code/data bytes:** **172**','**Proven non-code/data bytes:** **190**')
t=t.replace('**Reconciled object bytes:** **20,838 / 239,698 = 8.6934% (8.69%)**','**Reconciled object bytes:** **24,887 / 239,698 = 10.3826% (10.38%)**')
t=t.replace('**Unresolved object bytes:** **218,860**','**Unresolved object bytes:** **214,811**')
append='''## Batch 06 — map-mode setup, resource tables, nibble grids, and proximity helpers

Source identity and object-1 SHA-256 were reverified before this batch. Added **4,031 translated instruction bytes** and **18 directly proven alignment bytes**, bringing cumulative accounting to **24,697 translated + 190 non-code = 24,887 / 239,698 = 10.3826% (10.38%)**.

### Newly translated ranges

- `03634..0380C` — **473 bytes**: map-mode setup/teardown loop, including the exact `759C[i]=i` 256-byte initialization, viewport clamps, map rebuild/redraw, page toggles, and owned-buffer cleanup.
- `03810..0381E` — **15 bytes**: release the `608C` allocation.
- `03820..03835` — **22 bytes**: linear bump allocator over `608C + 6094`.
- `03838..03A63` — **556 bytes**: resource/table loader, allocation layout, scalar/table reads, variable sub-block reads, and final handle close.
- `03A78..03C02` — **395 bytes**: running low/high-nibble clamp over the `2194A` grid.
- `03C04..03DE2` — **479 bytes**: companion clamp over `2199A`, including type-dependent `219A1/219A2` initialization.
- `03DE4..03EA3` — **192 bytes**: `62B0`-controlled nibble enable/disable sweep.
- `03EA4..03F74` — **209 bytes**: `6350`-controlled companion sweep.
- `03F78..0403F` — **200 bytes**: modal two-line prompt/input-release cycle and page restore.
- `04040..040FE` — **191 bytes**: 360-direction fixed-point probe using full coordinates for `2C8E4`.
- `04100..04224` — **293 bytes**: special-level/proximity gate over active records and tile flags.
- `04228..043F0` — **457 bytes**: three-type distant-record relocation search with exact four candidate coordinate mutations/restoration.
- `043F4..044CE` — **219 bytes**: nibble deficit accounting across two 8x10 grids.
- `044D0..04619` — **330 bytes**: capacity-driven nibble consumption across four slots, including repeated same-slot consumption and output publication.

### Newly proven non-code/data

`0381F`, `03836..03837`, `03C03`, `03DE3`, `03F75..03F77`, `040FF`, `04225..04227`, `043F1..043F3`, `044CF`, and `0461A..0461B` total **18 bytes** of decoded compiler no-op alignment. A full object-1 direct branch/call target scan found no incoming target to these bytes.

The zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved; they are not counted from appearance alone.

### Fidelity notes

`0363E..0364C` writes each byte's own index into `759C`; it is not a zero fill. `04040` passes the evolving full 16.16 coordinate dwords to `2C8E4`. `044D0` loops within a slot and may consume multiple high-nibble units before advancing; reducing it to one unit per slot changes behavior.

### Next high-value targets

1. Continue the sequential application path at `0461C`.
2. Reconcile startup at `353A0` as a CODE.2-specific path.
3. Continue harvesting independently proved relocated CODE.1 runtime families.
'''
assert '## Batch 06 —' not in t
t=t.rstrip()+'\n\n'+append
p.write_text(t)

led=Path('Code2/RESIDUAL_LEDGER.md');u=led.read_text()
ins='''0381F..0381F | alignment | 1
03836..03837 | alignment | 2
03C03..03C03 | alignment | 1
03DE3..03DE3 | alignment | 1
03F75..03F77 | alignment | 3
040FF..040FF | alignment | 1
04225..04227 | alignment | 3
043F1..043F3 | alignment | 3
044CF..044CF | alignment | 1
0461A..0461B | alignment | 2
'''
assert '0381F..0381F' not in u
u=u.replace('<!-- RESIDUAL-RANGES-END -->',ins+'<!-- RESIDUAL-RANGES-END -->')
u=u.replace('**Total proven non-code/data:** **172 bytes**.','**Total proven non-code/data:** **190 bytes**.')
u=u.rstrip()+'''\n\nBatch 06 evidence: the added ranges decode as explicit compiler no-op alignment instructions between directly reconstructed function boundaries; a full object-1 direct relative control-flow target scan found no incoming target to any byte in these ranges. The zero islands `0380D..0380F` and `03A64..03A77` remain unresolved, as does `006B9..006BF`.\n'''
led.write_text(u)

cont=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md');c=cont.read_text()
assert 'Batch 06 checkpoint' not in c
c=c.rstrip()+'''\n\n## CODE.2 Batch 06 checkpoint

Batch 06 added **4,031 translated bytes** plus **18 proven non-code/data bytes**. Cumulative accounting is **24,697 translated + 190 non-code = 24,887 / 239,698 = 10.3826% (10.38%)**; unresolved is **214,811 bytes**.

The sequential application frontier now reaches `04619`; `0461A..0461B` is proven alignment and the next routine begins at `0461C`. Preserve three important details from this batch: `759C` is initialized with byte indices, `04040` passes full fixed-point dwords into `2C8E4`, and `044D0` may consume repeatedly within one slot. Zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.
'''
cont.write_text(c.rstrip()+'\n')
