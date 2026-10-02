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

# Residual pass 32: fourth small batch-08 inter-function alignment cluster.
# Exact asserted functions end at 648C and 6631; the following entries begin at
# 6490 and 6634. Batch 08 explicitly records alignment as excluded from code bytes.
new_rows=[
    (0x0648D,0x0648F,'alignment'),
    (0x06632,0x06633,'alignment'),
]
for nr in new_rows:
    assert all(nr[1] < a or nr[0] > b for a,b,_ in rows),nr
    rows.append(nr)
rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)

new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('explicitly classified residual below: **692 bytes**','explicitly classified residual below: **697 bytes**')
ledger=ledger.replace('alignment/zero/padding: **458 bytes**','alignment/zero/padding: **463 bytes**')
ledger=ledger.replace('residual still requiring classification: **345 bytes**','residual still requiring classification: **340 bytes**')
ledger=ledger.replace('Do not add these 692 bytes','Do not add these 697 bytes')
ledger=ledger.replace('Completion requires the remaining 345 bytes','Completion requires the remaining 340 bytes')
assert sum(b-a+1 for a,b,_ in rows)==697
assert sum(b-a+1 for a,b,c in rows if c=='table')==122
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==463
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==112
ledger_path.write_text(ledger)

p=Path('Code1/PROGRESS.md');t=p.read_text()
section='''\n\n## 2026-10-02 residual classification pass 32\n\nVerified translated code remains **69,365 / 70,402 = 98.53%**. The next small batch-08 inter-function cluster is classified: `648D..648F` and `6632..6633` are alignment bytes between exact asserted endpoints and the next aligned entries. This adds **5 classified bytes**. The ledger now classifies **697 bytes**, leaving **340 bytes unresolved**.\n'''
if '## 2026-10-02 residual classification pass 32' not in t:t+=section
p.write_text(t)

p=Path('Code1/DECOMPILATION_CONTINUITY.md');t=p.read_text()
section='''\n\n## 2026-10-02 residual pass 32 checkpoint\n\nBatch-08 alignment checkpoint 4: `648D..648F` and `6632..6633` (**5 bytes**) are classified from preserved exact function boundaries and the batch's `alignment excluded` note. Classified residual is now **697 bytes**; **340 bytes remain unresolved**. Verified translated code remains **69,365 bytes (98.53%)**.\n'''
if '## 2026-10-02 residual pass 32 checkpoint' not in t:t+=section
p.write_text(t)

p=Path('Code1/STAGING_LEDGER.md');t=p.read_text()
section='''\n\n## Residual pass 32\nSmall batch-08 checkpoint 4: classified `648D..648F` and `6632..6633` as alignment. `RESIDUAL_LEDGER.md` now holds 697 classified bytes; 340 conservative residual bytes remain. Verified translated code stays 69,365 and active staging remains zero.\n'''
if '## Residual pass 32' not in t:t+=section
p.write_text(t)
