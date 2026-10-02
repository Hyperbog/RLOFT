from pathlib import Path
import re

ledger_path=Path('Code1/RESIDUAL_LEDGER.md')
ledger=ledger_path.read_text()
block_re=re.compile(r'(<!-- RESIDUAL-RANGES-BEGIN -->\n)(.*?)(\n<!-- RESIDUAL-RANGES-END -->)',re.S)
m=block_re.search(ledger); assert m
rows=[]
for raw in m.group(2).splitlines():
    mm=re.fullmatch(r'([0-9A-F]{5})\.\.([0-9A-F]{5}) \| (alignment|table) \| ([0-9]+)',raw.strip()); assert mm,raw
    rows.append((int(mm.group(1),16),int(mm.group(2),16),mm.group(3)))
assert not any(a<=0x9EA3<=b or a<=0x9EA6<=b for a,b,_ in rows)
rows.append((0x9EA3,0x9EA6,'table'))
rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)
new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('explicitly classified residual below: **500 bytes**','explicitly classified residual below: **504 bytes**')
ledger=ledger.replace('embedded tables: **118 bytes**','embedded tables: **122 bytes**')
ledger=ledger.replace('residual still requiring classification: **250 bytes**','residual still requiring classification: **246 bytes**')
ledger=ledger.replace('Do not add these 500 bytes','Do not add these 504 bytes')
ledger=ledger.replace('Completion requires the remaining 250 bytes','Completion requires the remaining 246 bytes')
ledger=ledger.replace('- `091BC..091D7`: address table.','- `091BC..091D7`: address table.\n- `09EA3..09EA6`: preserved four-byte prefix of the batch-12 lookup data island; `09EA7` and following bytes were subsequently verified as code and are not classified here.')
assert len(rows)==148
assert sum(b-a+1 for a,b,_ in rows)==504
assert sum(b-a+1 for a,b,c in rows if c=='table')==122
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==382
ledger_path.write_text(ledger)

p=Path('Code1/PROGRESS.md'); t=p.read_text()
section='''\n\n## 2026-10-02 residual classification pass 24\n\nVerified translated code remains **69,652 / 70,402 = 98.93%**. Full Git-history reconciliation recovered a batch-12 note explicitly identifying `9EA3..9EA7` as lookup data at that checkpoint. Batch 20 later verified `9EA7..9EAD` as code, so only the still-unaccounted prefix **`9EA3..9EA6` (4 bytes)** is classified as table data. The curated non-code ledger is now **504 bytes** (122 table + 382 alignment/zero/padding), leaving **246 bytes** unresolved. `C79F` remains deliberately unclassified because its only historical hit is the stale pre-batch-18 exclusion of the formerly unresolved self-modifying blitter cluster.\n'''
if '## 2026-10-02 residual classification pass 24' not in t:t+=section
p.write_text(t)

p=Path('Code1/DECOMPILATION_CONTINUITY.md'); t=p.read_text()
section='''\n\n## 2026-10-02 residual pass 24 checkpoint\n\nHistorical batch-12 evidence proves `9EA3..9EA7` was lookup data at that stage, while batch 20 later verified `9EA7..9EAD` as code. Therefore only `9EA3..9EA6` is newly classified as table data. Classified non-code is **504 bytes** and **246 conservative residual bytes remain**; verified translated code remains **69,652 bytes (98.93%)**. Continue by reconciling preserved integration scripts and their exclusion lists, not by assuming that every small inter-function gap is padding.\n'''
if '## 2026-10-02 residual pass 24 checkpoint' not in t:t+=section
p.write_text(t)

p=Path('Code1/STAGING_LEDGER.md'); t=p.read_text()
section='''\n\n## Residual pass 24\nHistorical batch-12 evidence classifies `9EA3..9EA6` as lookup-table data after excluding the later-verified `9EA7..9EAD` code. `RESIDUAL_LEDGER.md` now holds 504 classified bytes; 246 conservative residual bytes remain. Verified translated code stays 69,652 and active staging remains zero.\n'''
if '## Residual pass 24' not in t:t+=section
p.write_text(t)
