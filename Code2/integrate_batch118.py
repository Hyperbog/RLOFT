from pathlib import Path

# Batch 118 closes three long-standing zero/alignment islands from the early object.
ledger=Path('Code2/RESIDUAL_LEDGER.md')
l=ledger.read_text()
anchor='006FD..006FF | alignment | 3\n'
assert anchor in l
l=l.replace(anchor,'006B9..006BF | padding | 7\n'+anchor,1)
anchor='0381F..0381F | alignment | 1\n'
assert anchor in l
l=l.replace(anchor,'0380D..0380F | padding | 3\n'+anchor,1)
anchor='03C03..03C03 | alignment | 1\n'
assert anchor in l
l=l.replace(anchor,'03A64..03A75 | padding | 18\n03A76..03A77 | alignment | 2\n'+anchor,1)
l=l.replace('**Total proven non-code/data:** **4,471 bytes**.','**Total proven non-code/data:** **4,502 bytes**.',1)
l += '''\n\n## Batch 118 additions\n\n- `006B9..006BF` — **7 bytes**, zero padding between the directly reconstructed return at `006B8` and the independent entry at `006C0`.\n- `0380D..0380F` — **3 bytes**, zero padding between the unconditional transfer at `03808` and the independent entry at `03810`.\n- `03A64..03A75` — **18 bytes**, zero padding after the directly reconstructed return at `03A63`.\n- `03A76..03A77` — **2 bytes**, encoded `MOV EAX,EAX` alignment immediately before the independent `03A78` entry.\n\nFresh object-1 disassembly was used for all four boundaries. A full direct relative CALL/JMP/Jcc target scan found no incoming control-flow target to any byte in these ranges, and a full disassembly text scan found no exact immediate/data reference to their starting addresses. These are the three historical zero islands repeatedly left unresolved in Batches 03..27; they are now directly closed.\n\nCumulative proven non-code/data: **4,502 bytes**.\n'''
ledger.write_text(l)

progress=Path('Code2/PROGRESS.md')
p=progress.read_text()
p=p.replace('object%20accounting-95.12%25-red','object%20accounting-95.13%25-red',1)
p=p.replace('**Proven non-code/data bytes:** **4,472**','**Proven non-code/data bytes:** **4,502**',1)
p=p.replace('**Reconciled object bytes:** **228,004 / 239,698 = 95.1214% (95.12%)**','**Reconciled object bytes:** **228,034 / 239,698 = 95.1339% (95.13%)**',1)
p=p.replace('**Unresolved object bytes:** **11,694**','**Unresolved object bytes:** **11,664**',1)
p += '''\n\n## Batch 118 — historical zero-island closure\n\nBatch 118 adds no translated instructions; it directly proves **30 non-code/alignment bytes** in the three long-standing early-object islands: `006B9..006BF`, `0380D..0380F`, and `03A64..03A77`. Fresh CODE.2 object-1 disassembly proves the adjacent control-flow boundaries, and a full direct relative control-flow target scan found no incoming target into any classified byte. Cumulative accounting is **223,532 translated + 4,502 non-code = 228,034 / 239,698 = 95.1339% (95.13%)**; unresolved is **11,664 bytes**.\n'''
progress.write_text(p)

add=Path('Code2/CODE2_CONTINUITY_ADDENDUM.md')
a=add.read_text()
a += '''\n\n## CODE.2 Batch 118 checkpoint\n\nBatch 118 closes the three historical early-object zero islands without changing translated coverage: `006B9..006BF` (**7 zero-padding bytes**), `0380D..0380F` (**3 zero-padding bytes**), and `03A64..03A77` (**18 zero-padding + 2-byte MOV-EAX,EAX alignment**). Fresh object-1 disassembly and a full direct relative control-flow target scan prove the classifications. Cumulative accounting is **223,532 translated + 4,502 non-code = 228,034 / 239,698 = 95.1339% (95.13%)**; unresolved is **11,664 bytes**. Continue selecting unresolved islands from the residual complement; do not revisit these three historical gaps.\n'''
add.write_text(a)

resume=Path('Code2/RESUME_CHECKPOINT.md')
r=resume.read_text()
r=r.replace('Verified translated instruction bytes: **223,532**.','Verified translated instruction bytes: **223,532**.',1)
r=r.replace('Proven non-code/data bytes: **4,472**.','Proven non-code/data bytes: **4,502**.',1)
r=r.replace('Reconciled object bytes: **228,004 / 239,698 = 95.1214% (95.12%)**.','Reconciled object bytes: **228,034 / 239,698 = 95.1339% (95.13%)**.',1)
r=r.replace('Unresolved object bytes: **11,694**.','Unresolved object bytes: **11,664**.',1)
r += '''\n\n## Batch 118 resume note\nThe historical zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` are now proven non-code/alignment (**30 bytes total**) and must not be revisited. Translated coverage remains **223,532**; proven non-code is **4,502**; reconciled is **228,034 / 239,698 = 95.1339%**; unresolved is **11,664**.\n'''
resume.write_text(r)
