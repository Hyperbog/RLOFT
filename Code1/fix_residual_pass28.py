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

# Residual pass 28: first small batch-08 inter-function alignment cluster.
# Preserved batch-08 integration evidence defines the code endpoints and states that
# alignment was excluded from the integrated ranges.  Each gap below ends exactly at
# the next 4-byte-aligned function entry in integrate_mid_runtime_batch_08.py:
#   6304..631E -> 6320, 6320..633A -> 633C,
#   633C..6356 -> 6358, 6358..63A1 -> 63A4.
new_rows=[
    (0x0631F,0x0631F,'alignment'),
    (0x0633B,0x0633B,'alignment'),
    (0x06357,0x06357,'alignment'),
    (0x063A2,0x063A3,'alignment'),
]
for nr in new_rows:
    assert all(nr[1] < a or nr[0] > b for a,b,_ in rows),nr
    rows.append(nr)
rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)

new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('explicitly classified residual below: **677 bytes**','explicitly classified residual below: **682 bytes**')
ledger=ledger.replace('alignment/zero/padding: **443 bytes**','alignment/zero/padding: **448 bytes**')
ledger=ledger.replace('residual still requiring classification: **73 bytes**','residual still requiring classification: **68 bytes**')
ledger=ledger.replace('Do not add these 677 bytes','Do not add these 682 bytes')
ledger=ledger.replace('Completion requires the remaining 73 bytes','Completion requires the remaining 68 bytes')
assert sum(b-a+1 for a,b,_ in rows)==682
assert sum(b-a+1 for a,b,c in rows if c=='table')==122
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==448
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==112
ledger_path.write_text(ledger)

p=Path('Code1/PROGRESS.md');t=p.read_text()
section='''\n\n## 2026-10-02 residual classification pass 28\n\nVerified translated code remains **69,652 / 70,402 = 98.93%**. The first small batch-08 inter-function cluster is now classified from preserved integration evidence: `631F`, `633B`, `6357`, and `63A2..63A3` are the alignment bytes between exact asserted function endpoints and the next 4-byte-aligned entries. This adds **5 classified bytes** without changing translated-code coverage. `RESIDUAL_LEDGER.md` now classifies **682 bytes**, leaving **68 bytes unresolved**.\n'''
if '## 2026-10-02 residual classification pass 28' not in t:t+=section
p.write_text(t)

p=Path('Code1/DECOMPILATION_CONTINUITY.md');t=p.read_text()
section='''\n\n## 2026-10-02 residual pass 28 checkpoint\n\nSmall-checkpoint policy applied. Batch-08 preserved function boundaries plus its explicit `alignment excluded` integration note classify `631F`, `633B`, `6357`, and `63A2..63A3` as alignment (**5 bytes**). Classified residual is now **682 bytes** and **68 bytes remain unresolved**. Verified translated code remains **69,652 bytes (98.93%)**.\n'''
if '## 2026-10-02 residual pass 28 checkpoint' not in t:t+=section
p.write_text(t)

p=Path('Code1/STAGING_LEDGER.md');t=p.read_text()
section='''\n\n## Residual pass 28\nSmall batch-08 checkpoint: classified `631F`, `633B`, `6357`, and `63A2..63A3` as alignment from preserved exact function boundaries and the batch's `alignment excluded` note. `RESIDUAL_LEDGER.md` now holds 682 classified bytes; 68 conservative residual bytes remain. Verified translated code stays 69,652 and active staging remains zero.\n'''
if '## Residual pass 28' not in t:t+=section
p.write_text(t)
