from pathlib import Path

a=Path('Code2/FINAL_SEMANTIC_AUDIT.md')
t=a.read_text()
marker='\n## Current result\n'
if marker not in t: raise SystemExit('missing current-result marker')
head,tail=t.split(marker,1)
cp='''

## Checkpoint 9 — `080AC..0A790`

**9,941 bytes.** Throughput was increased across reconstruction batches 12–15 while retaining ownership-complete boundaries. Fresh immutable-object disassembly closed the left-panel dispatcher and status/event sweeper, moving-record construction/compaction and collision/effect resolution, opposite-buffer hit testing and selection, party effect-slot insertion/status application, and the complete party-effect animation/resolution/modal-apply family through `0A790`. Span SHA-256: `ea8476146e7851dfd335577d0bfbf538a889d793175ef89edb47ee73f75d7aa5`. Mandatory overlap `[0x0806C,0x0816C)` SHA-256: `db9a0033708da1ed5d9ec2436f792e75bab21a28228588a3347d46f09ddd1520`.

**PASS with no new semantic correction.** The audit explicitly rechecked the CODE.1 defect classes and the cdecl live-stack pattern that has already reproduced in CODE.2. `080AC` preserves the two distinct `1A1F4` polls, signed IDIV menu-row selection, per-action capture-refresh asymmetry, saved `D680h`, and action-specific state restoration. `0871C/0893C` preserve XOR status-bit clearing, signed deadline comparisons, address-distinct expiration side effects, and the second fresh timer read used when reinstalling refresh state. `08A80` retains 16-bit high-word tile arithmetic/truncation, owner-bit packing, unchecked sound-table indexing, signed owner range tests, angle normalization/orientation packing, and the collision loop's flag-controlled exit. `08C88` deliberately reports an out-of-range signed index but continues into the compaction copy.

The moving-record collision families preserve unsigned radius-box arithmetic, owner-derived multipliers, mode-specific filters, signed half-damage behavior, exact type-specific bit changes, and the proximity/orientation sound gates. The `0934C..09367` bytes remain a seven-entry CS sound-offset table. `09418` retains signed pointer-coordinate handling and its mode-dependent return contract. The opposite-buffer selector keeps the encoded low-byte/low-word toggle distinctions, descending order search, and the deliberate fall-through to record[count] when an ordering word is absent. `096E0` was rechecked at each six-argument call site for cdecl slot order; the mode and party-slot arguments remain in the encoded positions.

`09790..09B49` was audited as one complete status/equipment-effect transaction: signed old equipment words, F218-derived byte values, A5/80 special cases, the two address-distinct F238 refresh paths, redraw/sound ordering, and optional timed-event installation all match the immutable instructions. Batch 15 (`09B4C..0A790`) was then carried through its complete animation, encoded effect-resolution, modal apply, and four-party timed-state update paths. Signed party-state tests, byte/word truncation, random/result scaling, message-only failure returns, and timer-factor multiplication were checked without finding another semantic drift. `0A791..0A793` is alignment before the next independent family; next frontier is `0A791`.
'''
# replace prior current result with checkpoint + new current result
newtail='''## Current result\n\n- Audited object bytes: **42,881 / 239,698 = 17.8896%**\n- Closed frontier: **`0A790`**\n- Next frontier: **`0A791`**\n- Semantic defect classes corrected so far: **6**\n- Object byte accounting remains **239,698 / 239,698 = 100.0000%** with **0 unresolved bytes**.\n'''
a.write_text(head+cp+'\n'+newtail)

p=Path('Code2/PROGRESS.md'); s=p.read_text()
s=s.replace('semantic%20audit-13.74%25-yellow','semantic%20audit-17.89%25-yellow')
s=s.replace('**Semantic audit:** **32,940 / 239,698 = 13.7423%**, closed through object offset `0x080AB`; **6 semantic defect classes** found so far.','**Semantic audit:** **42,881 / 239,698 = 17.8896%**, closed through object offset `0x0A790`; **6 semantic defect classes** found so far.')
p.write_text(s)

r=Path('Code2/RESUME_CHECKPOINT.md'); q=r.read_text()
q=q.replace('`080AC`','`0A791`').replace('32,940 / 239,698 = 13.7423%','42,881 / 239,698 = 17.8896%').replace('13.7423%','17.8896%')
r.write_text(q)
