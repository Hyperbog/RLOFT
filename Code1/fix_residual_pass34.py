from pathlib import Path
import re

ledger_path=Path('Code1/RESIDUAL_LEDGER.md')
ledger=ledger_path.read_text()
block_re=re.compile(r'(<!-- RESIDUAL-RANGES-BEGIN -->\n)(.*?)(\n<!-- RESIDUAL-RANGES-END -->)',re.S)
m=block_re.search(ledger); assert m
rows=[]
for raw in m.group(2).splitlines():
    mm=re.fullmatch(r'([0-9A-F]{5})\.\.([0-9A-F]{5}) \| (alignment|table|noncode) \| ([0-9]+)',raw.strip()); assert mm,raw
    a,b=int(mm.group(1),16),int(mm.group(2),16)
    assert b-a+1==int(mm.group(4))
    rows.append((a,b,mm.group(3)))

# Residual pass 34: final batch-08 inter-function alignment cluster.
# Exact asserted functions end at 67F1 and 6822; the following entries begin at
# 67F4 and 6824. Batch 08 records alignment as excluded from translated code.
new_rows=[
    (0x067F2,0x067F3,'alignment'),
    (0x06823,0x06823,'alignment'),
]
for nr in new_rows:
    assert all(nr[1] < a or nr[0] > b for a,b,_ in rows),nr
    rows.append(nr)
rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)

new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('explicitly classified residual below: **702 bytes**','explicitly classified residual below: **705 bytes**')
ledger=ledger.replace('alignment/zero/padding: **468 bytes**','alignment/zero/padding: **471 bytes**')
ledger=ledger.replace('residual still requiring classification: **335 bytes**','residual still requiring classification: **332 bytes**')
ledger=ledger.replace('Do not add these 702 bytes','Do not add these 705 bytes')
ledger=ledger.replace('Completion requires the remaining 335 bytes','Completion requires the remaining 332 bytes')
assert sum(b-a+1 for a,b,_ in rows)==705
assert sum(b-a+1 for a,b,c in rows if c=='table')==122
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==471
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==112
ledger_path.write_text(ledger)

p=Path('Code1/PROGRESS.md');t=p.read_text()
section='''\n\n## 2026-10-02 residual classification pass 34\n\nVerified translated code remains **69,365 / 70,402 = 98.53%**. The final batch-08 inter-function complement bytes are classified: `67F2..67F3` and `6823` are alignment between exact asserted endpoints and the next aligned entries. This adds **3 classified bytes**. All **62 bytes** in batch 08 outside its 47 exact asserted code intervals are now classified non-code/alignment. The global residual ledger now classifies **705 bytes**, leaving **332 bytes unresolved**.\n'''
if '## 2026-10-02 residual classification pass 34' not in t:t+=section
p.write_text(t)

p=Path('Code1/DECOMPILATION_CONTINUITY.md');t=p.read_text()
section='''\n\n## 2026-10-02 residual pass 34 checkpoint\n\nBatch-08 alignment checkpoint 6 closes the batch: `67F2..67F3` and `6823` (**3 bytes**) are classified from preserved exact function boundaries and the batch's `alignment excluded` note. All 62 complement bytes inside batch 08's advertised spans are now classified. Global classified residual is **705 bytes**; **332 bytes remain unresolved**. Verified translated code remains **69,365 bytes (98.53%)**.\n'''
if '## 2026-10-02 residual pass 34 checkpoint' not in t:t+=section
p.write_text(t)

p=Path('Code1/STAGING_LEDGER.md');t=p.read_text()
section='''\n\n## Residual pass 34\nFinal batch-08 checkpoint: classified `67F2..67F3` and `6823` as alignment. The batch-08 advertised spans are now fully partitioned into 3,384 translated bytes and 62 classified non-code/alignment bytes. `RESIDUAL_LEDGER.md` now holds 705 classified bytes; 332 conservative residual bytes remain. Verified translated code stays 69,365 and active staging remains zero.\n'''
if '## Residual pass 34' not in t:t+=section
p.write_text(t)
