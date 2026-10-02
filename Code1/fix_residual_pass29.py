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

# Residual pass 29: second small batch-08 inter-function alignment cluster.
# Preserved batch-08 evidence marks alignment as excluded. Exact asserted code ends at
# 63B6 and 63D4, with the next entries at 63B8 and 63D8 respectively.
new_rows=[
    (0x063B7,0x063B7,'alignment'),
    (0x063D5,0x063D7,'alignment'),
]
for nr in new_rows:
    assert all(nr[1] < a or nr[0] > b for a,b,_ in rows),nr
    rows.append(nr)
rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)

new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('explicitly classified residual below: **682 bytes**','explicitly classified residual below: **686 bytes**')
ledger=ledger.replace('alignment/zero/padding: **448 bytes**','alignment/zero/padding: **452 bytes**')
ledger=ledger.replace('residual still requiring classification: **68 bytes**','residual still requiring classification: **64 bytes**')
ledger=ledger.replace('Do not add these 682 bytes','Do not add these 686 bytes')
ledger=ledger.replace('Completion requires the remaining 68 bytes','Completion requires the remaining 64 bytes')
assert sum(b-a+1 for a,b,_ in rows)==686
assert sum(b-a+1 for a,b,c in rows if c=='table')==122
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==452
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==112
ledger_path.write_text(ledger)

p=Path('Code1/PROGRESS.md');t=p.read_text()
section='''\n\n## 2026-10-02 residual classification pass 29\n\nVerified translated code remains **69,652 / 70,402 = 98.93%**. The second small batch-08 inter-function cluster is classified: `63B7` and `63D5..63D7` are alignment bytes between exact asserted endpoints and the next aligned entries. This adds **4 classified bytes**. The ledger now classifies **686 bytes**, leaving **64 bytes unresolved**.\n'''
if '## 2026-10-02 residual classification pass 29' not in t:t+=section
p.write_text(t)

p=Path('Code1/DECOMPILATION_CONTINUITY.md');t=p.read_text()
section='''\n\n## 2026-10-02 residual pass 29 checkpoint\n\nBatch-08 alignment checkpoint 2: `63B7` and `63D5..63D7` (**4 bytes**) are classified from preserved exact function boundaries and the batch's `alignment excluded` note. Classified residual is now **686 bytes**; **64 bytes remain unresolved**. Verified translated code remains **69,652 bytes (98.93%)**.\n'''
if '## 2026-10-02 residual pass 29 checkpoint' not in t:t+=section
p.write_text(t)

p=Path('Code1/STAGING_LEDGER.md');t=p.read_text()
section='''\n\n## Residual pass 29\nSmall batch-08 checkpoint 2: classified `63B7` and `63D5..63D7` as alignment. `RESIDUAL_LEDGER.md` now holds 686 classified bytes; 64 conservative residual bytes remain. Verified translated code stays 69,652 and active staging remains zero.\n'''
if '## Residual pass 29' not in t:t+=section
p.write_text(t)
