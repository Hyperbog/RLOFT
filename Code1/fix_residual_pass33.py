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

# Residual pass 33: fifth small batch-08 inter-function alignment cluster.
# Exact asserted functions end at 6671, 66A6, 6742 and 67C6; the next entries
# begin at 6674, 66A8, 6744 and 67C8. Batch 08 records alignment as excluded.
new_rows=[
    (0x06672,0x06673,'alignment'),
    (0x066A7,0x066A7,'alignment'),
    (0x06743,0x06743,'alignment'),
    (0x067C7,0x067C7,'alignment'),
]
for nr in new_rows:
    assert all(nr[1] < a or nr[0] > b for a,b,_ in rows),nr
    rows.append(nr)
rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)

new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('explicitly classified residual below: **697 bytes**','explicitly classified residual below: **702 bytes**')
ledger=ledger.replace('alignment/zero/padding: **463 bytes**','alignment/zero/padding: **468 bytes**')
ledger=ledger.replace('residual still requiring classification: **340 bytes**','residual still requiring classification: **335 bytes**')
ledger=ledger.replace('Do not add these 697 bytes','Do not add these 702 bytes')
ledger=ledger.replace('Completion requires the remaining 340 bytes','Completion requires the remaining 335 bytes')
assert sum(b-a+1 for a,b,_ in rows)==702
assert sum(b-a+1 for a,b,c in rows if c=='table')==122
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==468
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==112
ledger_path.write_text(ledger)

p=Path('Code1/PROGRESS.md');t=p.read_text()
section='''\n\n## 2026-10-02 residual classification pass 33\n\nVerified translated code remains **69,365 / 70,402 = 98.53%**. The next small batch-08 inter-function cluster is classified: `6672..6673`, `66A7`, `6743`, and `67C7` are alignment bytes between exact asserted endpoints and the next aligned entries. This adds **5 classified bytes**. The ledger now classifies **702 bytes**, leaving **335 bytes unresolved**.\n'''
if '## 2026-10-02 residual classification pass 33' not in t:t+=section
p.write_text(t)

p=Path('Code1/DECOMPILATION_CONTINUITY.md');t=p.read_text()
section='''\n\n## 2026-10-02 residual pass 33 checkpoint\n\nBatch-08 alignment checkpoint 5: `6672..6673`, `66A7`, `6743`, and `67C7` (**5 bytes**) are classified from preserved exact function boundaries and the batch's `alignment excluded` note. Classified residual is now **702 bytes**; **335 bytes remain unresolved**. Verified translated code remains **69,365 bytes (98.53%)**.\n'''
if '## 2026-10-02 residual pass 33 checkpoint' not in t:t+=section
p.write_text(t)

p=Path('Code1/STAGING_LEDGER.md');t=p.read_text()
section='''\n\n## Residual pass 33\nSmall batch-08 checkpoint 5: classified `6672..6673`, `66A7`, `6743`, and `67C7` as alignment. `RESIDUAL_LEDGER.md` now holds 702 classified bytes; 335 conservative residual bytes remain. Verified translated code stays 69,365 and active staging remains zero.\n'''
if '## Residual pass 33' not in t:t+=section
p.write_text(t)
