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

# Reconstruct batch 09 directly from its preserved integrator. Its asserted
# intervals total exactly the declared 3,324 verified bytes. The two selected
# spans total 3,390 bytes and the preserved batch record explicitly states that
# alignment/data islands were excluded; therefore their 66-byte complement is
# proven non-code without inferring instruction contents.
script=Path('Code1/integrate_mid_graphics_batch_09.py').read_text()
sa=re.compile(r'static_assert\(0x([0-9A-Fa-f]+)u-0x([0-9A-Fa-f]+)u==([0-9]+)u\)')
code=[]
for end,start,n in sa.findall(script):
    a,b,n=int(start,16),int(end,16),int(n)
    if b-a==n: code.append((a,b))
selected=[(0x06854,0x071C8),(0x072A0,0x0766A)]
code=[(a,b) for a,b in code if any(x<=a and b<=y for x,y in selected)]
merged=[]
for a,b in sorted(code):
    if merged and a<=merged[-1][1]: merged[-1][1]=max(merged[-1][1],b)
    else: merged.append([a,b])
assert sum(b-a for a,b in merged)==3324
assert sum(y-x for x,y in selected)==3390
complement=[]
for x,y in selected:
    p=x
    for a,b in merged:
        if b<=x or a>=y: continue
        aa=max(a,x); bb=min(b,y)
        if p<aa: complement.append((p,aa-1))
        p=max(p,bb)
    if p<y: complement.append((p,y-1))
assert sum(b-a+1 for a,b in complement)==66
existing={(a,b) for a,b,_ in rows}
for a,b in complement:
    assert (a,b) not in existing,(hex(a),hex(b))
    rows.append((a,b,'noncode'))
rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)
new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('explicitly classified residual below: **608 bytes**','explicitly classified residual below: **674 bytes**')
ledger=ledger.replace('proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **43 bytes**','proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **109 bytes**')
ledger=ledger.replace('residual still requiring classification: **142 bytes**','residual still requiring classification: **76 bytes**')
ledger=ledger.replace('Do not add these 608 bytes','Do not add these 674 bytes')
ledger=ledger.replace('Completion requires the remaining 142 bytes','Completion requires the remaining 76 bytes')
assert sum(b-a+1 for a,b,_ in rows)==674
assert sum(b-a+1 for a,b,c in rows if c=='table')==122
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==443
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==109
ledger_path.write_text(ledger)

p=Path('Code1/PROGRESS.md'); t=p.read_text()
section='''\n\n## 2026-10-02 residual classification pass 26\n\nVerified translated code remains **69,652 / 70,402 = 98.93%**. The preserved batch-09 integrator closes exactly: its two selected spans (`6854..71C7` and `72A0..7669`) contain **3,390 bytes**, its exact static-assert intervals contain the declared **3,324 verified code bytes**, and its preserved checkpoint explicitly states that alignment/data islands were excluded. The **66-byte complement** is therefore classified as conservative non-code. No complement byte was previously present in the residual ledger. `RESIDUAL_LEDGER.md` now classifies **674 bytes** (122 table + 443 alignment/zero/padding + 109 generic non-code), leaving **76 bytes unresolved**. No translated-code coverage was added.\n'''
if '## 2026-10-02 residual classification pass 26' not in t:t+=section
p.write_text(t)

p=Path('Code1/DECOMPILATION_CONTINUITY.md'); t=p.read_text()
section='''\n\n## 2026-10-02 residual pass 26 checkpoint\n\nBatch 09 is arithmetic-closed from its preserved integrator: 3,390 selected bytes = 3,324 exact asserted verified code + 66 explicitly excluded alignment/data bytes. Those 66 bytes are now `noncode` in `RESIDUAL_LEDGER.md`. Classified residual is **674 bytes**, leaving **76 conservative residual bytes**; verified translated code remains **69,652 bytes (98.93%)**. Next priority is batch-12 reconciliation: decode its preserved compressed integrator as text only (do not execute it), verify its exact asserted intervals and exclusion language, and classify only an arithmetic-closed complement.\n'''
if '## 2026-10-02 residual pass 26 checkpoint' not in t:t+=section
p.write_text(t)

p=Path('Code1/STAGING_LEDGER.md'); t=p.read_text()
section='''\n\n## Residual pass 26\nThe batch-09 preserved integrator proves a 66-byte non-code complement. `RESIDUAL_LEDGER.md` now holds 674 classified bytes; 76 conservative residual bytes remain. Verified translated code stays 69,652 and active staging remains zero.\n'''
if '## Residual pass 26' not in t:t+=section
p.write_text(t)
