from pathlib import Path

cpp=Path('Code2/Code2.cpp')
s=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 11405u;' in s
assert 'constexpr u32 kVerifiedNonCodeBytes = 77u;' in s
assert 'function_011a8' not in s
s=s.replace('constexpr u32 kVerifiedTranslatedBytes = 11405u;','constexpr u32 kVerifiedTranslatedBytes = 15532u;')
s=s.replace('constexpr u32 kVerifiedNonCodeBytes = 77u;','constexpr u32 kVerifiedNonCodeBytes = 94u;')
old='''              (0x00CB0u-0x00BFCu) + (0x010C3u-0x00CB0u) +
              (0x011A5u-0x010C4u));'''
new='''              (0x00CB0u-0x00BFCu) + (0x010C3u-0x00CB0u) +
              (0x011A5u-0x010C4u) +
              (0x01227u-0x011A8u) + (0x01399u-0x01228u) +
              (0x01557u-0x0139Cu) + (0x01831u-0x01558u) +
              (0x01C08u-0x01834u) + (0x01C28u-0x01C08u) +
              (0x01C4Du-0x01C28u) + (0x01CA8u-0x01C50u) +
              (0x01CE2u-0x01CA8u) + (0x01F6Bu-0x01CE4u) +
              (0x02079u-0x01F6Cu) + (0x021D8u-0x0207Cu));'''
assert old in s
s=s.replace(old,new)
marker='\n} // namespace code2\n'
assert s.count(marker)==1
frag=Path('Code2/batch04_append.inc').read_text().rstrip()+"\n"
s=s.replace(marker,'\n'+frag+marker)
cpp.write_text(s)

ledger=Path('Code2/RESIDUAL_LEDGER.md')
l=ledger.read_text()
assert '**Total proven non-code/data:** **77 bytes**.' in l
insert='''01227..01227 | alignment | 1
01399..0139B | alignment | 3
01557..01557 | alignment | 1
01831..01833 | alignment | 3
01C4D..01C4F | alignment | 3
01CE2..01CE3 | alignment | 2
01F6B..01F6B | alignment | 1
02079..0207B | alignment | 3
'''
l=l.replace('<!-- RESIDUAL-RANGES-END -->',insert+'<!-- RESIDUAL-RANGES-END -->')
l=l.replace('**Total proven non-code/data:** **77 bytes**.','**Total proven non-code/data:** **94 bytes**.')
l += '\nBatch 04 evidence: the eight added ranges decode as compiler no-op alignment forms between directly reconciled routine boundaries. A full object-1 direct branch/call-target scan found no incoming target to any byte in these ranges. `006B9..006BF` remains unresolved.\n'
ledger.write_text(l)

p=Path('Code2/PROGRESS.md')
t=p.read_text()
t=t.replace('object%20accounting-4.79%25-red','object%20accounting-6.52%25-red')
t=t.replace('**Verified translated instruction bytes:** **11,405**','**Verified translated instruction bytes:** **15,532**')
t=t.replace('**Proven non-code/data bytes:** **77**','**Proven non-code/data bytes:** **94**')
t=t.replace('**Reconciled object bytes:** **11,482 / 239,698 = 4.7902% (4.79%)**','**Reconciled object bytes:** **15,626 / 239,698 = 6.5190% (6.52%)**')
t=t.replace('**Unresolved object bytes:** **228,216**','**Unresolved object bytes:** **224,072**')
assert '## Batch 04' not in t
t += '''\n\n## Batch 04 — text/map UI and packed editor subsystem\n\nSource identity and object-1 SHA-256 were reverified before this batch. Added **4,127 translated instruction bytes** and **17 directly proven alignment bytes**, bringing cumulative accounting to **15,532 translated + 94 non-code = 15,626 / 239,698 = 6.5190% (6.52%)**.\n\n### Newly translated ranges\n\n- `011A8..01226` — **127 bytes**: fixed viewport clear/draw helper with `258DC` reloaded across opaque draw calls.\n- `01228..01398` — **369 bytes**: packed 5760h text-record clipping/drawing, signed lengths, CR line breaks, and font-byte advances.\n- `0139C..01556` — **443 bytes**: two-pass rectangular map redraw plus optional packed-text/frame overlay.\n- `01558..01830` — **729 bytes**: per-cell map classification table generator, including player markers, terrain 93h, object occupancy, and all eight neighbor probes.\n- `01834..01C07` — **980 bytes**: map/UI initialization and palette-driven strip/icon rendering.\n- `01C08..01C27` — **32 bytes**: packed text-list terminator pointer.\n- `01C28..01C4C` — **37 bytes**: packed text-list used extent.\n- `01C50..01CA7` — **88 bytes**: bounded text emission helper.\n- `01CA8..01CE1` — **58 bytes**: final-line text width helper.\n- `01CE4..01F6A` — **647 bytes**: in-place packed text editor with exact accepted-character tests, backspace/CR handling, capacity checks, and redraw gating.\n- `01F6C..02078` — **269 bytes**: lower-panel/map redraw and page flip.\n- `0207C..021D7` — **348 bytes**: pointer-driven modal text-insertion loop with signed hit tests and 1..4 line clamping.\n\n### Newly proven non-code/data\n\n`01227`, `01399..0139B`, `01557`, `01831..01833`, `01C4D..01C4F`, `01CE2..01CE3`, `01F6B`, and `02079..0207B` total **17 bytes** of decoded no-op alignment. None is a direct branch/call target in a full object-1 target scan.\n\n### Fidelity notes\n\nThis batch preserves global reloads of `258DC` and map dimensions across opaque calls instead of caching them. The editor redraw flag changes only after an actual mutation; invalid input and backspace on an empty record do not force a redraw. The packed-list extent routine remains address-distinct and duplicates the binary traversal rather than being normalized into a call to `01C08`.\n\n### Next high-value targets\n\n1. Continue the application/UI path at `021D8`.\n2. Reconcile startup at `353A0` as a CODE.2-specific path.\n3. Continue harvesting independently proved relocated CODE.1 runtime families.\n'''
p.write_text(t)

a=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md')
u=a.read_text()
assert '## CODE.2 Batch 04 checkpoint' not in u
u += '''\n\n## CODE.2 Batch 04 checkpoint\n\nBatch 04 added **4,127 translated bytes** plus **17 proven alignment bytes**. Cumulative accounting is **15,532 translated + 94 non-code = 15,626 / 239,698 = 6.5190% (6.52%)**; unresolved is **224,072 bytes**.\n\nThe application frontier now reaches `021D7`. The new block covers the 5760h packed text list, viewport/map redraw, cell classification at 1760h, UI initialization, in-place text editing, and pointer-driven insertion. Preserve the batch's explicit `258DC`/dimension reloads across opaque callbacks and the editor's mutation-only redraw behavior. Continue at `021D8`; `006B9..006BF` is still unresolved.\n'''
a.write_text(u)

assert 15532+94+224072==239698
assert 11405+4127==15532
assert 77+17==94
