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

# Residual pass 31: third small batch-08 inter-function alignment cluster.
# The preserved batch-08 integration explicitly excludes alignment. Exact asserted
# functions end at 63F0 and 6408; the next encoded entries begin at 63F4 and 640C.
new_rows=[
    (0x063F1,0x063F3,'alignment'),
    (0x06409,0x0640B,'alignment'),
]
for nr in new_rows:
    assert all(nr[1] < a or nr[0] > b for a,b,_ in rows),nr
    rows.append(nr)
rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)

new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('explicitly classified residual below: **686 bytes**','explicitly classified residual below: **692 bytes**')
ledger=ledger.replace('alignment/zero/padding: **452 bytes**','alignment/zero/padding: **458 bytes**')
ledger=ledger.replace('residual still requiring classification: **351 bytes**','residual still requiring classification: **345 bytes**')
ledger=ledger.replace('Do not add these 686 bytes','Do not add these 692 bytes')
ledger=ledger.replace('Completion requires the remaining 351 bytes','Completion requires the remaining 345 bytes')
assert sum(b-a+1 for a,b,_ in rows)==692
assert sum(b-a+1 for a,b,c in rows if c=='table')==122
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==458
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==112
ledger_path.write_text(ledger)

p=Path('Code1/PROGRESS.md');t=p.read_text()
section='''\n\n## 2026-10-02 residual classification pass 31\n\nVerified translated code remains **69,365 / 70,402 = 98.53%**. The next small batch-08 inter-function cluster is classified: `63F1..63F3` and `6409..640B` are alignment bytes between exact asserted endpoints and the next aligned entries. This adds **6 classified bytes**. The ledger now classifies **692 bytes**, leaving **345 bytes unresolved**.\n'''
if '## 2026-10-02 residual classification pass 31' not in t:t+=section
p.write_text(t)

p=Path('Code1/DECOMPILATION_CONTINUITY.md');t=p.read_text()
section='''\n\n## 2026-10-02 residual pass 31 checkpoint\n\nBatch-08 alignment checkpoint 3: `63F1..63F3` and `6409..640B` (**6 bytes**) are classified from preserved exact function boundaries and the batch's `alignment excluded` note. Classified residual is now **692 bytes**; **345 bytes remain unresolved**. Verified translated code remains **69,365 bytes (98.53%)**.\n'''
if '## 2026-10-02 residual pass 31 checkpoint' not in t:t+=section
p.write_text(t)

p=Path('Code1/STAGING_LEDGER.md');t=p.read_text()
section='''\n\n## Residual pass 31\nSmall batch-08 checkpoint 3: classified `63F1..63F3` and `6409..640B` as alignment. `RESIDUAL_LEDGER.md` now holds 692 classified bytes; 345 conservative residual bytes remain. Verified translated code stays 69,365 and active staging remains zero.\n'''
if '## Residual pass 31' not in t:t+=section
p.write_text(t)
