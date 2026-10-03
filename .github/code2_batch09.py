from pathlib import Path
import re

cpp=Path('Code2/Code2.cpp')
s=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 31278u;' in s
assert 'constexpr u32 kVerifiedNonCodeBytes = 246u;' in s
assert 'void function_06008' not in s
frag=Path('Code2/batch09.inc').read_text()
marker='\n} // namespace code2\n'
assert s.count(marker)==1
s=s.replace('constexpr u32 kVerifiedTranslatedBytes = 31278u;','constexpr u32 kVerifiedTranslatedBytes = 34466u;')
s=s.replace('constexpr u32 kVerifiedNonCodeBytes = 246u;','constexpr u32 kVerifiedNonCodeBytes = 295u;')
pat=re.compile(r'static_assert\(kVerifiedTranslatedBytes ==.*?\);',re.S)
m=pat.search(s)
assert m
new='''static_assert(kVerifiedTranslatedBytes == 31278u +\n              (0x062ABu-0x06008u) + (0x06352u-0x062ACu) +\n              (0x06441u-0x06354u) + (0x06520u-0x06444u) +\n              (0x066E6u-0x06548u) + (0x06C67u-0x066E8u) +\n              (0x06CADu-0x06C68u));'''
s=s[:m.start()]+new+s[m.end():]
s=s.replace(marker,'\n'+frag+marker)
cpp.write_text(s)

p=Path('Code2/PROGRESS.md');t=p.read_text()
t=t.replace('object%20accounting-13.15%25-red','object%20accounting-14.50%25-red')
t=t.replace('**Verified translated instruction bytes:** **31,278**','**Verified translated instruction bytes:** **34,466**')
t=t.replace('**Proven non-code/data bytes:** **246**','**Proven non-code/data bytes:** **295**')
t=t.replace('**Reconciled object bytes:** **31,524 / 239,698 = 13.1515% (13.15%)**','**Reconciled object bytes:** **34,761 / 239,698 = 14.5020% (14.50%)**')
t=t.replace('**Unresolved object bytes:** **208,174**','**Unresolved object bytes:** **204,937**')
append='''## Batch 09 — settings panel, interaction dispatcher, save serializer, and tile probe\n\nSource identity and object-1 SHA-256 were reverified before this batch. Added **3,188 translated instruction bytes** and **49 directly proven non-code/data bytes**, bringing cumulative accounting to **34,466 translated + 295 non-code = 34,761 / 239,698 = 14.5020% (14.50%)**.\n\n### Newly translated ranges\n\n- `06008..062AA` — **675 bytes**: full settings-panel background/border construction, static labels, ten retained subimage buffers, initial render/page flip.\n- `062AC..06351` — **166 bytes**: 0..9 mouse-driven selector with the original shared epilogue behavior represented semantically.\n- `06354..06440` — **237 bytes**: 0..14 selector, record byte publication through `F050+6C`, and dependent redraw calls.\n- `06444..0651F` — **220 bytes**: 0..128 slider plus the encoded signed divide-by-128 truncation used to derive `C02D`.\n- `06548..066E5` — **414 bytes**: ten-way settings interaction dispatcher and nine-handle teardown.\n- `066E8..06C66` — **1,407 bytes**: ordered save serializer with exact short-write aborts, dynamic record blocks, 62 variable descriptor payloads, and final close/result path.\n- `06C68..06CAC` — **69 bytes**: position record probe publishing the `CF6C` boolean.\n\n### Newly proven non-code/data\n\n`062AB`, `06352..06353`, `06441..06443`, `066E6..066E7`, and `06C67` are explicit compiler no-op separators with no incoming direct relative branch/call target. `06520..06547` is the **40-byte ten-entry CS jump table** consumed by the indirect dispatcher at `065B1`; its targets are `065B9`, `065C3`, `065DB`, `065F3`, `0660B`, `06623`, `0662D`, `06652`, `06693`, and `0669A`.\n\nThe zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n\n### Fidelity notes\n\n`062AC` and `06354` branch into shared epilogue bytes belonging to adjacent address ranges; their C++ entries preserve the resulting return semantics without double-counting those bytes. In `06548`, rows above `78h` and the five-pixel inter-column gutter return directly to input polling, while an out-of-range signed quotient still takes the redraw/page-flip path. The exit teardown releases only handles `12B30..12B50`; the tenth handle at `12B54` is deliberately not folded into that loop. The save serializer preserves every short-write failure as close-and-return-1 and models the `30700` value as a stack-local write through a distinct boundary form.\n\n### Next high-value targets\n\n1. Continue the sequential application path at `06CAD`.\n2. Reconcile startup at `353A0` as a CODE.2-specific path.\n3. Continue harvesting independently proved relocated CODE.1 runtime families.\n'''
assert '## Batch 09 —' not in t
t=t.rstrip()+'\n\n'+append.rstrip()+'\n'
p.write_text(t)

led=Path('Code2/RESIDUAL_LEDGER.md');u=led.read_text()
ins='''062AB..062AB | alignment | 1\n06352..06353 | alignment | 2\n06441..06443 | alignment | 3\n06520..06547 | table | 40\n066E6..066E7 | alignment | 2\n06C67..06C67 | alignment | 1\n'''
assert '06520..06547' not in u
u=u.replace('<!-- RESIDUAL-RANGES-END -->',ins+'<!-- RESIDUAL-RANGES-END -->')
u=u.replace('**Total proven non-code/data:** **246 bytes**.','**Total proven non-code/data:** **295 bytes**.')
u=u.rstrip()+'''\n\nBatch 09 evidence: the five alignment ranges decode as explicit compiler no-op separators and a full object-1 direct relative control-flow target scan found no incoming target to any of them. `06520..06547` is the ten-entry CS jump table selected by `JMP DWORD PTR CS:[EBX*4+6520h]` at `065B1`; all ten table destinations were directly disassembled and reconciled. The known zero islands remain unresolved.\n'''
led.write_text(u.rstrip()+'\n')

cont=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md');c=cont.read_text()
assert 'Batch 09 checkpoint' not in c
c=c.rstrip()+'''\n\n## CODE.2 Batch 09 checkpoint\n\nBatch 09 added **3,188 translated bytes** plus **49 proven non-code/data bytes**. Cumulative accounting is **34,466 translated + 295 non-code = 34,761 / 239,698 = 14.5020% (14.50%)**; unresolved is **204,937 bytes**.\n\nThe sequential application frontier now reaches `06CAC`; `06C67` is proven alignment and the next routine begins at `06CAD`. Preserve the `06548` polling/redraw split, the nine-handle teardown (`12B30..12B50`, not `12B54`), and the save serializer's exact close-on-short-write behavior. The ten-entry CS table at `06520..06547` is fully reconciled. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
cont.write_text(c.rstrip()+'\n')
