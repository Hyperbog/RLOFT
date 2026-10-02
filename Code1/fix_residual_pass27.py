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

# Batch 10 exact arithmetic for the selected 767C..80B9 region.
# The preserved integrator declares this region selected while excluding documented
# alignment/data and interrupt islands.  Exact static_assert intervals identify the
# batch-10 code; batch 20 later proved 7F50..7F5B as code and batch 16 later proved
# 7F5C..7F89 as interrupt-handler code.  After subtracting both later code ranges and
# all already-classified ledger bytes, exactly 7F8A..7F8B remains.  That two-byte
# complement is therefore proven excluded non-code, not inferred from byte appearance.
script=Path('Code1/integrate_mid_ui_batch_10.py').read_text()
sa=re.compile(r'static_assert\(0x([0-9A-Fa-f]+)u-0x([0-9A-Fa-f]+)u==([0-9]+)u\)')
code=[]
for end,start,n in sa.findall(script):
    a,b,n=int(start,16),int(end,16),int(n)
    if b-a==n and 0x0767C<=a and b<=0x080BA:
        code.append((a,b))
merged=[]
for a,b in sorted(code):
    if merged and a<=merged[-1][1]: merged[-1][1]=max(merged[-1][1],b)
    else: merged.append([a,b])
assert sum(b-a for a,b in merged)==2518
span=[(0x0767C,0x080BA)]

def subtract(parts,cuts):
    out=[]
    for a,b in parts:
        cur=[(a,b)]
        for c,d in cuts:
            nxt=[]
            for x,y in cur:
                if d<=x or c>=y:
                    nxt.append((x,y)); continue
                if x<c: nxt.append((x,c))
                if d<y: nxt.append((d,y))
            cur=nxt
        out.extend(cur)
    return out

complement=subtract(span,merged)
assert sum(b-a for a,b in complement)==104
later_verified=[(0x07F50,0x07F5C),(0x07F5C,0x07F8A)]
remaining_after_later=subtract(complement,later_verified)
assert sum(b-a for a,b in remaining_after_later)==46
classified=[]
for a,b,_ in rows: classified.append((a,b+1))
novel=subtract(remaining_after_later,classified)
assert novel==[(0x07F8A,0x07F8C)],novel

rows.append((0x07F8A,0x07F8B,'noncode'))
rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)
new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('explicitly classified residual below: **674 bytes**','explicitly classified residual below: **676 bytes**')
ledger=ledger.replace('proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **109 bytes**','proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **111 bytes**')
ledger=ledger.replace('residual still requiring classification: **76 bytes**','residual still requiring classification: **74 bytes**')
ledger=ledger.replace('Do not add these 674 bytes','Do not add these 676 bytes')
ledger=ledger.replace('Completion requires the remaining 76 bytes','Completion requires the remaining 74 bytes')
assert sum(b-a+1 for a,b,_ in rows)==676
assert sum(b-a+1 for a,b,c in rows if c=='table')==122
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==443
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==111
ledger_path.write_text(ledger)

p=Path('Code1/PROGRESS.md'); t=p.read_text()
section='''\n\n## 2026-10-02 residual classification pass 27\n\nVerified translated code remains **69,652 / 70,402 = 98.93%**. Batch-10 reconciliation was corrected using its exact asserted intervals rather than the imprecise prose endpoint around `8490..8491`. In selected span `767C..80B9`, batch 10 asserted **2,518 code bytes** and excluded alignment/data/interrupt islands. Later verification accounts for `7F50..7F5B` (batch 20) and `7F5C..7F89` (batch 16) as code. Subtracting those later code bytes and all existing ledger classifications leaves exactly **`7F8A..7F8B` (2 bytes)** as the batch-10 excluded non-code complement. The ledger now classifies **676 bytes**, leaving **74 bytes unresolved**.\n'''
if '## 2026-10-02 residual classification pass 27' not in t:t+=section
p.write_text(t)

p=Path('Code1/DECOMPILATION_CONTINUITY.md'); t=p.read_text()
section='''\n\n## 2026-10-02 residual pass 27 checkpoint\n\nCorrected batch-10 arithmetic proves `7F8A..7F8B` as excluded non-code after subtracting later verified code `7F50..7F89` and all already-classified alignment/data islands. The exact batch-10 asserts, not its slightly short prose endpoint at `848F`, control the reconciliation. Classified non-code is now **676 bytes** and **74 conservative residual bytes remain**; verified translated code remains **69,652 bytes (98.93%)**. Batch-12's six remaining local gaps (20 bytes total) still lack explicit exclusion evidence and remain unresolved.\n'''
if '## 2026-10-02 residual pass 27 checkpoint' not in t:t+=section
p.write_text(t)

p=Path('Code1/STAGING_LEDGER.md'); t=p.read_text()
section='''\n\n## Residual pass 27\nCorrected batch-10 arithmetic classifies `7F8A..7F8B` as non-code. `RESIDUAL_LEDGER.md` now holds 676 classified bytes; 74 conservative residual bytes remain. Verified translated code stays 69,652 and active staging remains zero.\n'''
if '## Residual pass 27' not in t:t+=section
p.write_text(t)
