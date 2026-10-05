from pathlib import Path

a=Path('Code2/FINAL_SEMANTIC_AUDIT.md')
t=a.read_text()
marker='## CODE.1 cross-reference policy\n'
block='''## Checkpoint 5 — `02D70..03633`\n\n**2,244 bytes.** Fresh disassembly from the immutable object closed the 16-entry CS dispatch table, its `02DB0..034AE` control-panel dispatcher, the `034AF` separator, the `034B0..03630` navigation/redraw helper, and the three-byte `03631..03633` alignment. Span SHA-256: `34d1aab387aaafb288749013481c975620f3d0ed2819a3cdb03d04b72a3ca373`. The mandatory overlap was regenerated across the prior frontier; `[0x02D30,0x02DF0)` hashes to `11b1b378f281c2685f0fad8f9046c8c7af5a83cfe35d2f3f8641c9fa2280171`.\n\nPASS with no new semantic correction. The table entries were decoded directly as `2E59` x4, `2E82`, `2F5E`, `303A`, `3125`, `3210`, `32B4`, `337A`, `343D`, and `2E6C` x4. The dispatcher preserves inclusive rectangle bounds, the entry-12 sentinel behavior, the flag-2 remap of hits 0..3 to actions 12..15, and the encoded action-bit shifts. The modal actions retain their distinct `298F0`/`2975C` image offsets and copy dimensions, and the level-navigation paths keep signed clamps and the original page-flip/redraw ordering.\n\nThe `034B0` helper was checked specifically for first-set-bit priority, signed `-40` / `-4` bounds, the asymmetric dimension-derived limits (`height-0x2B`, `width-0x2C`, `height-0x2F`, `width-0x30`), 32-bit wrapping arithmetic on the stored offsets, and reloads of `12740h/12744h` before the redraw call. No CODE.1 defect class reproduced here. `03634` is the next independent routine.\n\n'''
if marker not in t: raise SystemExit('audit marker missing')
if '## Checkpoint 5 — `02D70..03633`' in t: raise SystemExit('checkpoint 5 already present')
t=t.replace(marker,block+marker,1)
old='''- Audited object bytes: **11,632 / 239,698 = 4.8528%**.\n- Semantic corrections found so far: **2 defect classes**.\n- New correction in checkpoint 4: **1 repeated cdecl argument-order defect affecting 10 call sites**.\n- Current closed audited frontier: **`02D6F`**.\n- Next frontier: **`02D70`**.'''
new='''- Audited object bytes: **13,876 / 239,698 = 5.7890%**.\n- Semantic corrections found so far: **2 defect classes**.\n- New corrections in checkpoint 5: **0**.\n- Current closed audited frontier: **`03633`**.\n- Next frontier: **`03634`**.'''
if old not in t: raise SystemExit('audit result target missing')
a.write_text(t.replace(old,new,1))

p=Path('Code2/PROGRESS.md')
u=p.read_text()
u=u.replace('semantic%20audit-4.85%25-yellow','semantic%20audit-5.79%25-yellow',1)
old='**Semantic audit:** **11,632 / 239,698 = 4.8528%**, closed through object offset `0x02D6F`; **2 semantic defect classes** found so far.'
new='**Semantic audit:** **13,876 / 239,698 = 5.7890%**, closed through object offset `0x03633`; **2 semantic defect classes** found so far.'
if old not in u: raise SystemExit('progress target missing')
p.write_text(u.replace(old,new,1))

r=Path('Code2/RESUME_CHECKPOINT.md')
v=r.read_text()
block='''\n\n## Semantic audit checkpoint 5 handoff\n- Object accounting remains closed at **235,022 translated + 4,676 proven non-code/data = 239,698 / 239,698**.\n- Semantic audit: **13,876 / 239,698 = 5.7890%**, closed through **`03633`**.\n- Semantic defect classes found so far: **2**; checkpoint 5 found no new correction.\n- Checkpoint-5 span `02D70..03633` SHA-256: `34d1aab387aaafb288749013481c975620f3d0ed2819a3cdb03d04b72a3ca373`.\n- Mandatory prior-frontier overlap `[0x02D30,0x02DF0)` SHA-256: `11b1b378f281c2685f0fad8f9046c8c7af5a83cfe35d2f3f8641c9fa2280171`.\n- Next semantic-audit frontier: **`03634`**.\n'''
if '## Semantic audit checkpoint 5 handoff' in v: raise SystemExit('resume checkpoint 5 already present')
r.write_text(v+block)
