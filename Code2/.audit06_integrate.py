from pathlib import Path

p=Path('Code2/Code2.cpp'); t=p.read_text()
a='if(c!=0u){const u8 v=*dg(m,base+row*10u+i+0x6Au)'; b='if(row!=c){const u8 v=*dg(m,base+row*10u+i+0x6Au)'
c='if(a!=0u){const u8 v=*dg(m,base+row*10u+i+0xBAu)'; d='if(row!=a){const u8 v=*dg(m,base+row*10u+i+0xBAu)'
e='if(load_le16(dg(m,0x218FCu+slot_off))==load_le16(dg(m,0x218FEu+slot_off))){if(remaining)--*remaining;continue;}'
f='if(load_le16(dg(m,0x218FCu+slot_off))==load_le16(dg(m,0x218FEu+slot_off)))continue;'
for old,new in ((a,b),(c,d),(e,f)):
    if old not in t: raise SystemExit('Code2 target missing: '+old)
    t=t.replace(old,new,1)
p.write_text(t)

p=Path('Code2/FINAL_SEMANTIC_AUDIT.md'); t=p.read_text(); marker='## CODE.1 cross-reference policy\n'
block='''## Checkpoint 6 — `03634..0518B`\n\n**7,000 bytes.** Expanded audit across two original reconstruction batches. Fresh immutable-object disassembly covered map-mode setup/resource loading/nibble-grid logic (`03634..0461B`) and the complete four-party simulation/update loop plus classifier (`0461C..0518B`). Span SHA-256: `8fb9e029a196bcfae47729e08ad90d08bfc26cd8671f5b7f006d74cf65abe9ae`. Mandatory overlap `[0x035F4,0x036F4)` SHA-256: `c0a5b070819dc0380fc634dc1f8445dd684f6e699958fad502ff9c1fa7962645`.\n\nTwo new semantic defect classes were corrected. First, `043F4` had normalized `050DC` classifier indices into booleans. The binary compares the current row directly against each returned index, so the `+6A` grid is processed when `row != classifier(mode=1)` and `+BA` when `row != classifier(mode=0)`; `FFFFFFFFh` naturally excludes no row. Second, `044D0` incorrectly decremented `remaining` when a slot was already complete on entry. Binary equality at `4546..4554` jumps directly to the next slot; the decrement at `45F7..45FB` occurs only after the transfer-loop path reaches equality.\n\nThe remainder passed the high-risk audit. Signed map offsets/dimensions, variable resource-table construction, nibble-width truncation, full 16.16 arguments to `2C8E4`, level/proximity predicates, ordered movement attempts, and redraw flags were retained. The large `0461C` loop preserves its three distinct cleanup paths, signed tick/deadline comparisons, 16-bit current/target fields, and repeated classifier calls. `050DC` was re-disassembled through its RET at `0518B` and returns exact indices 0/1/2 or `FFFFFFFFh`. `0518C` is the next independent routine.\n\n'''
if marker not in t or '## Checkpoint 6 — `03634..0518B`' in t: raise SystemExit('audit marker state')
t=t.replace(marker,block+marker,1)
old='- Audited object bytes: **13,876 / 239,698 = 5.7890%**.\n- Semantic corrections found so far: **2 defect classes**.\n- New corrections in checkpoint 5: **0**.\n- Current closed audited frontier: **`03633`**.\n- Next frontier: **`03634`**.'
new='- Audited object bytes: **20,876 / 239,698 = 8.7093%**.\n- Semantic corrections found so far: **4 defect classes**.\n- New corrections in checkpoint 6: **2 defect classes**.\n- Current closed audited frontier: **`0518B`**.\n- Next frontier: **`0518C`**.'
if old not in t: raise SystemExit('audit result target')
p.write_text(t.replace(old,new,1))

p=Path('Code2/PROGRESS.md'); t=p.read_text().replace('semantic%20audit-5.79%25-yellow','semantic%20audit-8.71%25-yellow',1)
old='**Semantic audit:** **13,876 / 239,698 = 5.7890%**, closed through object offset `0x03633`; **2 semantic defect classes** found so far.'
new='**Semantic audit:** **20,876 / 239,698 = 8.7093%**, closed through object offset `0x0518B`; **4 semantic defect classes** found so far.'
if old not in t: raise SystemExit('progress target')
p.write_text(t.replace(old,new,1))

p=Path('Code2/RESUME_CHECKPOINT.md'); t=p.read_text()
if '## Semantic audit checkpoint 6 handoff' in t: raise SystemExit('resume already updated')
t+='''\n\n## Semantic audit checkpoint 6 handoff\n- Object accounting remains **239,698 / 239,698**.\n- Semantic audit: **20,876 / 239,698 = 8.7093%**, closed through **`0518B`**.\n- Semantic defect classes: **4**; checkpoint 6 corrected two new classes in `043F4` and `044D0`.\n- Span SHA-256: `8fb9e029a196bcfae47729e08ad90d08bfc26cd8671f5b7f006d74cf65abe9ae`.\n- Overlap `[0x035F4,0x036F4)` SHA-256: `c0a5b070819dc0380fc634dc1f8445dd684f6e699958fad502ff9c1fa7962645`.\n- Next semantic-audit frontier: **`0518C`**.\n'''
p.write_text(t)
