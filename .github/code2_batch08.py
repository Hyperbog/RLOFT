from pathlib import Path
import re

cpp=Path('Code2/Code2.cpp')
s=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 27622u;' in s
assert 'constexpr u32 kVerifiedNonCodeBytes = 193u;' in s
assert 'void function_0518c' not in s
frag=Path('Code2/batch08.inc').read_text().rstrip()+'\n'
marker='\n} // namespace code2\n'
assert s.count(marker)==1
s=s.replace('constexpr u32 kVerifiedTranslatedBytes = 27622u;','constexpr u32 kVerifiedTranslatedBytes = 31278u;')
s=s.replace('constexpr u32 kVerifiedNonCodeBytes = 193u;','constexpr u32 kVerifiedNonCodeBytes = 246u;')
pat=re.compile(r'static_assert\(kVerifiedTranslatedBytes ==.*?\);',re.S)
m=pat.search(s)
assert m
new='''static_assert(kVerifiedTranslatedBytes == 27622u +\n              (0x05578u-0x0518Cu) + (0x05695u-0x05578u) +\n              (0x05983u-0x05698u) + (0x05A11u-0x05984u) +\n              (0x05AA3u-0x05A14u) + (0x05BDCu-0x05AA4u) +\n              (0x05C81u-0x05BDCu) + (0x05D34u-0x05C84u) +\n              (0x05DD0u-0x05D34u) + (0x05E38u-0x05DD0u) +\n              (0x05EC3u-0x05E38u) + (0x06008u-0x05EECu));'''
s=s[:m.start()]+new+s[m.end():]
s=s.replace(marker,'\n'+frag+marker)
cpp.write_text(s)

p=Path('Code2/PROGRESS.md');t=p.read_text()
t=t.replace('object%20accounting-11.60%25-red','object%20accounting-13.15%25-red')
t=t.replace('**Verified translated instruction bytes:** **27,622**','**Verified translated instruction bytes:** **31,278**')
t=t.replace('**Proven non-code/data bytes:** **193**','**Proven non-code/data bytes:** **246**')
t=t.replace('**Reconciled object bytes:** **27,815 / 239,698 = 11.6042% (11.60%)**','**Reconciled object bytes:** **31,524 / 239,698 = 13.1515% (13.15%)**')
t=t.replace('**Unresolved object bytes:** **211,883**','**Unresolved object bytes:** **208,174**')
append='''## Batch 08 — party/status UI buffers, interaction loops, settings, and status icons\n\nSource identity and object-1 SHA-256 were reverified before this batch. Added **3,656 translated instruction bytes** and **53 directly proven non-code/data bytes**, bringing cumulative accounting to **31,278 translated + 246 non-code = 31,524 / 239,698 = 13.1515% (13.15%)**.\n\n### Newly translated ranges\n\n- `0518C..05577` — **1,004 bytes**: six-row party/status buffer construction, palette remap setup, cell rendering, and page handoff.\n- `05578..05694` — **285 bytes**: status-panel palette/identity restoration.\n- `05698..05982` — **747 bytes**: mouse hit-test and quantity mutation over six rows, including signed row-byte accumulation and immediate return on decrementing an already-zero quantity.\n- `05984..05A10` — **141 bytes**: panel-open path and page flip.\n- `05A14..05AA2` — **143 bytes**: panel-close path with three-byte local state forwarding.\n- `05AA4..05BDB` — **312 bytes**: six-buffer allocation, pointer-driven editor loop, row-selection quotient/remainder gate, cleanup, and selected-row return.\n- `05BDC..05C80` — **165 bytes**: eight-byte capacity preparation and mode-dependent augmentation/zeroing.\n- `05C84..05D33` — **176 bytes**: mode-0 selection loop; continuation preserves the signed record-type gate held in ESI.\n- `05D34..05DCF` — **156 bytes**: mode-1 selection loop with its distinct continuation rule and optional `620A` shift.\n- `05DD0..05E37` — **104 bytes**: settings/resource replace-write path.\n- `05E38..05EC2` — **139 bytes**: settings/resource read path plus redraw and two propagated byte settings.\n- `05EEC..06007` — **284 bytes**: ten-way status-icon renderer driven by the CS jump table at `05EC4..05EEB`.\n\n### Newly proven non-code/data\n\n`05695..05697`, `05983`, `05A11..05A13`, `05AA3`, `05C81..05C83`, `05DCF`, and `05EC3` are explicit compiler no-op separators with no incoming direct branch/call target. `05EC4..05EEB` is a **40-byte, ten-entry CS jump table** consumed by the indirect jump at `05F34`; its targets are `05F3B`, `05F54`, `05F65`, `05F76`, `05F84`, `05F92`, `05FA5`, `05FB3`, `05FC1`, and `05FD7`. These ranges total **53 bytes**.\n\nThe known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n\n### Fidelity notes\n\n`05698` sums quantity bytes with **signed byte promotion** before comparing against the unsigned row limit. `05C84` does not simply rerun `050DC` as a loop predicate: after the first pass, ESI contains the signed record-type byte and that value controls the next continuation test. `05D34` has a different continuation shape and therefore remains separate. The three-byte values passed to `32468` are modeled as local byte arrays, preserving that the binary passes a pointer to three stack-local bytes rather than a packed integer.\n\n### Next high-value targets\n\n1. Continue the sequential application path at `06008`.\n2. Reconcile startup at `353A0` as a CODE.2-specific path.\n3. Continue harvesting independently proved relocated CODE.1 runtime families.\n'''
assert '## Batch 08 —' not in t
t=t.rstrip()+'\n\n'+append.rstrip()+'\n'
p.write_text(t)

led=Path('Code2/RESIDUAL_LEDGER.md');u=led.read_text()
ins='''05695..05697 | alignment | 3\n05983..05983 | alignment | 1\n05A11..05A13 | alignment | 3\n05AA3..05AA3 | alignment | 1\n05C81..05C83 | alignment | 3\n05DCF..05DCF | alignment | 1\n05EC3..05EC3 | alignment | 1\n05EC4..05EEB | table | 40\n'''
assert '05EC4..05EEB' not in u
u=u.replace('<!-- RESIDUAL-RANGES-END -->',ins+'<!-- RESIDUAL-RANGES-END -->')
u=u.replace('**Total proven non-code/data:** **193 bytes**.','**Total proven non-code/data:** **246 bytes**.')
u=u.rstrip()+'''\n\nBatch 08 evidence: the seven alignment ranges decode as explicit NOP/LEA no-op separators and a full object-1 direct relative control-flow scan found no incoming target to any byte in those ranges. `05EC4..05EEB` is the ten-entry CS jump table indexed by `ESI` at `05F34`; all ten dword targets were decoded and reconciled in `function_05eec`. The known zero islands remain unresolved.\n'''
led.write_text(u.rstrip()+'\n')

cont=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md');c=cont.read_text()
assert 'Batch 08 checkpoint' not in c
c=c.rstrip()+'''\n\n## CODE.2 Batch 08 checkpoint\n\nBatch 08 added **3,656 translated bytes** plus **53 proven non-code/data bytes**. Cumulative accounting is **31,278 translated + 246 non-code = 31,524 / 239,698 = 13.1515% (13.15%)**; unresolved is **208,174 bytes**.\n\nThe sequential application frontier now reaches `06007`; the next routine begins at `06008`. Preserve the signed quantity accumulation in `05698`, the ESI-held signed record-type continuation gate in `05C84`, the distinct `05D34` continuation rule, and the ten-entry jump table at `05EC4..05EEB`. Zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
cont.write_text(c.rstrip()+'\n')
