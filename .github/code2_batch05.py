from pathlib import Path
import re
cpp=Path('Code2/Code2.cpp')
s=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 15532u;' in s
assert 'constexpr u32 kVerifiedNonCodeBytes = 94u;' in s
assert 'void function_021d8' not in s
frag=Path('Code2/batch05_part1.inc').read_text()+Path('Code2/batch05_part2.inc').read_text()
marker='\n} // namespace code2\n'
assert s.count(marker)==1
s=s.replace('constexpr u32 kVerifiedTranslatedBytes = 15532u;','constexpr u32 kVerifiedTranslatedBytes = 20666u;')
s=s.replace('constexpr u32 kVerifiedNonCodeBytes = 94u;','constexpr u32 kVerifiedNonCodeBytes = 172u;')
pat=re.compile(r'static_assert\(kVerifiedTranslatedBytes ==.*?\);',re.S)
m=pat.search(s)
assert m
new='''static_assert(kVerifiedTranslatedBytes == 15532u +\n              (0x02353u-0x021D8u) + (0x023B6u-0x02354u) +\n              (0x02473u-0x023B8u) + (0x026E6u-0x02474u) +\n              (0x02A34u-0x026E8u) + (0x02AE5u-0x02A34u) +\n              (0x02D6Fu-0x02AE8u) + (0x034AFu-0x02DB0u) +\n              (0x03631u-0x034B0u));'''
s=s[:m.start()]+new+s[m.end():]
s=s.replace(marker,'\n'+frag+marker)
cpp.write_text(s)

p=Path('Code2/PROGRESS.md');t=p.read_text()
t=t.replace('object%20accounting-6.52%25-red','object%20accounting-8.69%25-red')
t=t.replace('**Verified translated instruction bytes:** **15,532**','**Verified translated instruction bytes:** **20,666**')
t=t.replace('**Proven non-code/data bytes:** **94**','**Proven non-code/data bytes:** **172**')
t=t.replace('**Reconciled object bytes:** **15,626 / 239,698 = 6.5190% (6.52%)**','**Reconciled object bytes:** **20,838 / 239,698 = 8.6934% (8.69%)**')
t=t.replace('**Unresolved object bytes:** **224,072**','**Unresolved object bytes:** **218,860**')
append='''\n\n## Batch 05 — modal text deletion, map export/load, and control panel\n\nSource identity and object-1 SHA-256 were reverified before this batch. Added **5,134 translated instruction bytes** and **78 directly proven non-code/data bytes**, bringing cumulative accounting to **20,666 translated + 172 non-code = 20,838 / 239,698 = 8.6934% (8.69%)**.\n\n### Newly translated ranges\n\n- `021D8..02352` — **379 bytes**: mouse-driven packed-text hit testing and deletion; preserves strict rectangle tests and the encoded forward-copy/clear arithmetic.\n- `02354..023B5` — **98 bytes**: 17h status-frame predicate with exact bit acceptance.\n- `023B8..02472` — **187 bytes**: packed-record coordinate lookup and two-byte A..Z/A.. label generation.\n- `02474..026E5` — **626 bytes**: fixed-width packed-text serializer in character and block-write modes.\n- `026E8..02A33` — **844 bytes**: three-plane ASCII map exporter plus packed-text append/flush behavior.\n- `02A34..02AE4` — **177 bytes**: numbered export-name preparation/open/export/close path.\n- `02AE8..02D6E` — **647 bytes**: numbered map/resource loader, live-level packed-text preservation, record-table allocation, map/text reads, and cleanup.\n- `02DB0..034AE` — **1,791 bytes**: sixteen-way control-panel dispatcher driven by the CS table at `02D70`; includes text add/delete actions, level cycling, export, toggle, and exit paths.\n- `034B0..03630` — **385 bytes**: priority-ordered viewport navigation bits, signed clamps, redraw, and page flip.\n\n### Newly proven non-code/data\n\n`02353`, `023B6..023B7`, `02473`, `026E6..026E7`, `02AE5..02AE7`, `02D6F`, `034AF`, and `03631..03633` are decoded compiler alignment forms with no incoming direct branch/call target. `02D70..02DAF` is a **64-byte sixteen-entry CS jump table** used by `02DB0`; every target was reconciled. These ranges total **78 bytes**.\n\n`006B9..006BF` remains unresolved.\n\n### Fidelity notes\n\nThe control-panel rectangle sentinel is entry 12, while action 11 deliberately uses entry 11 coordinates. Actions 12..15 are not duplicate cases: they synthesize navigation-mask bits 10h..80h from the first four rectangles. The map loader preserves the current packed-text buffer only for the live level, and its signed `< 7FFh` final-size test is retained.\n\n### Next high-value targets\n\n1. Continue the application path at `03634`.\n2. Reconcile startup at `353A0` as a CODE.2-specific path.\n3. Continue harvesting independently proved relocated CODE.1 runtime families.\n'''
assert '## Batch 05 —' not in t
t=t.rstrip()+append+'\n';p.write_text(t)

led=Path('Code2/RESIDUAL_LEDGER.md');u=led.read_text()
ins='''02353..02353 | alignment | 1\n023B6..023B7 | alignment | 2\n02473..02473 | alignment | 1\n026E6..026E7 | alignment | 2\n02AE5..02AE7 | alignment | 3\n02D6F..02D6F | alignment | 1\n02D70..02DAF | table | 64\n034AF..034AF | alignment | 1\n03631..03633 | alignment | 3\n'''
assert '02D70..02DAF' not in u
u=u.replace('<!-- RESIDUAL-RANGES-END -->',ins+'<!-- RESIDUAL-RANGES-END -->')
u=u.replace('**Total proven non-code/data:** **94 bytes**.','**Total proven non-code/data:** **172 bytes**.')
u=u.rstrip()+'''\n\nBatch 05 evidence: the eight alignment ranges decode as explicit compiler no-op separators and have no incoming target in a full object-1 direct branch/call scan. `02D70..02DAF` is the sixteen-entry CS jump table consumed by the indirect dispatcher at `02E51`; all sixteen dword targets were decoded and reconciled. `006B9..006BF` remains unresolved.\n'''
led.write_text(u)

cont=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md');c=cont.read_text()
assert 'Batch 05 checkpoint' not in c
c=c.rstrip()+'''\n\n## CODE.2 Batch 05 checkpoint\n\nBatch 05 added **5,134 translated bytes** plus **78 proven non-code/data bytes**. Cumulative accounting is **20,666 translated + 172 non-code = 20,838 / 239,698 = 8.6934% (8.69%)**; unresolved is **218,860 bytes**.\n\nThe sequential application frontier now reaches `03630`; `03631..03633` is proven alignment and the next routine begins at `03634`. The `02DB0..034AE` control-panel dispatcher uses the directly decoded 16-entry table at `02D70..02DAF`; entry 12 is the rectangle sentinel and action 11 intentionally addresses entry 11. Continue at `03634`, while keeping `006B9..006BF` unresolved until separately proven.\n'''
cont.write_text(c+'\n')
