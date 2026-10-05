from pathlib import Path

a=Path('Code2/FINAL_SEMANTIC_AUDIT.md')
t=a.read_text()
marker='\n## Current result\n'
if marker not in t: raise SystemExit('missing current result')
head,_=t.split(marker,1)
cp='''

## Checkpoint 11 — `0D550..0E577`

**4,136 bytes.** Fresh immutable-object disassembly closes reconstruction Batch 19 as one ownership-complete semantic family. Span SHA-256: `1b4ee3a5f7408be3bacc636835222d8784dbb7b92e701a680103e7688c47565b`. Mandatory overlap `[0x0D510,0x0D610)` SHA-256: `139640a02784b4e05e87119fbe559947bfcb2c2f2556736eac74e91e9f3c53f8`.

**PASS with no new semantic correction.** The CODE.1 defect checklist was applied to cdecl slot consumption, signed divisions/comparisons, partial-width party fields, table dispatch, shared error tails, and early-return state. `0D550` retains its three-argument entry contract and address-shared error/sound exits. `0DDA4` keeps the three track-indexed experience/class/level fields separate rather than collapsing them into one derived state. The six-entry table at `0DC64..0DC7B` and eight-entry table at `0E008..0E027` remain CS data owned by their dispatchers, with surrounding alignment retained exactly.

`0E0F8` preserves the encoded three-attempt placement rule rather than an unbounded search or normalized retry loop. `0E284` preserves its five 32-bit argument slots, signed/width-sensitive party-record accesses, and the type-18 early revival return that bypasses the ordinary continuation. The audit also rechecked the intervening alignment/table islands recorded in the residual ledger (`0DBD3`, `0DC64..0DC7B`, `0DDA1..0DDA3`, `0E008..0E027`, `0E0F5..0E0F7`, and `0E282..0E283`) against fresh bytes. No new call-order, signedness, aliasing, wrap, or global-reload drift was found. Next executable frontier: `0E578`.
'''
cur='''
## Current result

- Audited object bytes: **58,728 / 239,698 = 24.5008%**
- Closed frontier: **`0E577`**
- Next frontier: **`0E578`**
- Semantic defect classes corrected so far: **6**
- Object byte accounting remains **239,698 / 239,698 = 100.0000%** with **0 unresolved bytes**.
'''
a.write_text(head+cp+'\n'+cur)

p=Path('Code2/PROGRESS.md'); s=p.read_text()
s=s.replace('semantic%20audit-22.78%25-yellow','semantic%20audit-24.50%25-yellow')
s=s.replace('**Semantic audit:** **54,592 / 239,698 = 22.7753%**, closed through object offset `0x0D54F`; **6 semantic defect classes** found so far.','**Semantic audit:** **58,728 / 239,698 = 24.5008%**, closed through object offset `0x0E577`; **6 semantic defect classes** found so far.')
p.write_text(s)

r=Path('Code2/RESUME_CHECKPOINT.md'); q=r.read_text()
q += '''\n\n## Semantic audit checkpoint 11 handoff\n- Object accounting remains **239,698 / 239,698**.\n- Semantic audit: **58,728 / 239,698 = 24.5008%**, closed through **`0E577`**.\n- Semantic defect classes remain **6**; checkpoint 11 found no new defect.\n- Span `0D550..0E577` SHA-256: `1b4ee3a5f7408be3bacc636835222d8784dbb7b92e701a680103e7688c47565b`.\n- Mandatory overlap `[0x0D510,0x0D610)` SHA-256: `139640a02784b4e05e87119fbe559947bfcb2c2f2556736eac74e91e9f3c53f8`.\n- Next semantic-audit frontier: **`0E578`**.\n'''
r.write_text(q)
