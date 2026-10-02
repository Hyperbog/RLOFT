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
existing={(a,b) for a,b,_ in rows}

# Batch 13 arithmetic closure. AE0C..BE60 is 4,181 bytes; the declared verified
# code total is 4,129 and the already enumerated exclusions account for 48.
# The only remaining current assert-gap bytes in that span are these 4 bytes,
# so they are proven non-code even though the old prose exclusion list omitted them.
new_noncode=[
    (0x0BCB3,0x0BCB3),(0x0BD83,0x0BD83),(0x0BE26,0x0BE27),
]
assert sum(b-a+1 for a,b in new_noncode)==4

# Batch 14 exact asserted subranges sum to the complete declared 2,339-byte
# BE70..C79E/CBD4..CBEE contribution. Its integrator explicitly says the holes
# are alignment/data islands, so these 39 bytes are proven non-code.
b14=[
 (0x0BF35,0x0BF37),(0x0BFDF,0x0BFDF),(0x0C079,0x0C07B),
 (0x0C10F,0x0C10F),(0x0C17B,0x0C17B),(0x0C1AF,0x0C1AF),
 (0x0C227,0x0C227),(0x0C2C3,0x0C2CF),(0x0C49D,0x0C49F),
 (0x0C57B,0x0C57B),(0x0C62A,0x0C62B),(0x0C64D,0x0C64F),
 (0x0C65F,0x0C65F),(0x0C68E,0x0C68F),(0x0C6EE,0x0C6EF),
 (0x0C6F7,0x0C6F7),
]
assert sum(b-a+1 for a,b in b14)==39
new_noncode += b14

# Batch 18 exact four code ranges total 1,066 bytes. The enclosing C79F..CBD3
# span is 1,077 bytes and the integrator explicitly says alignment islands are
# excluded, proving these 11 bytes as alignment (including the formerly held C79F).
b18=[
 (0x0C79F,0x0C79F),(0x0C8C1,0x0C8C3),(0x0C9D2,0x0C9D3),
 (0x0CACA,0x0CACB),(0x0CBD1,0x0CBD3),
]
assert sum(b-a+1 for a,b in b18)==11

# Batch 11: parse the preserved integrator itself. Its exact static_assert code
# intervals sum to the declared 4,116 bytes. The three selected spans total
# 4,166 bytes; the script explicitly says alignment bytes are intentionally
# excluded (address tables are outside these three selected spans). Therefore
# the 50 complement bytes are proven alignment.
script=Path('Code1/integrate_mid_asset_batch_11.py').read_text()
sa=re.compile(r'static_assert\(0x([0-9A-Fa-f]+)u-0x([0-9A-Fa-f]+)u==([0-9]+)u\)')
code=[]
for end,start,n in sa.findall(script):
    a,b,n=int(start,16),int(end,16),int(n)
    if b-a==n: code.append((a,b))
selected=[(0x087C4,0x08E1F),(0x091D8,0x095A8),(0x095BC,0x09BD7)]
code_in=[]
for a,b in code:
    for x,y in selected:
        if x<=a and b<=y:
            code_in.append((a,b)); break
merged=[]
for a,b in sorted(code_in):
    if merged and a<=merged[-1][1]: merged[-1][1]=max(merged[-1][1],b)
    else: merged.append([a,b])
assert sum(b-a for a,b in merged)==4116
assert sum(y-x for x,y in selected)==4166
b11=[]
for x,y in selected:
    p=x
    for a,b in merged:
        if b<=x or a>=y: continue
        aa=max(a,x); bb=min(b,y)
        if p<aa:b11.append((p,aa-1))
        p=max(p,bb)
    if p<y:b11.append((p,y-1))
assert sum(b-a+1 for a,b in b11)==50, b11

for a,b in new_noncode:
    assert (a,b) not in existing,(hex(a),hex(b))
    rows.append((a,b,'noncode'))
for a,b in b18+b11:
    assert (a,b) not in existing,(hex(a),hex(b))
    rows.append((a,b,'alignment'))
rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)

new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('explicitly classified residual below: **504 bytes**','explicitly classified residual below: **608 bytes**')
ledger=ledger.replace('embedded tables: **122 bytes**\n  - alignment/zero/padding: **382 bytes**','embedded tables: **122 bytes**\n  - alignment/zero/padding: **443 bytes**\n  - proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **43 bytes**')
ledger=ledger.replace('residual still requiring classification: **246 bytes**','residual still requiring classification: **142 bytes**')
ledger=ledger.replace('Format: `START..END | category | inclusive byte count`.','Format: `START..END | category | inclusive byte count`. Categories are `alignment`, `table`, and conservative `noncode` where preserved batch arithmetic proves exclusion but does not uniquely recover the padding-vs-data subtype.')
ledger=ledger.replace('All other ranges in the machine-readable block are alignment, NOP/LEA/MOV-self padding, or explicitly documented zero/data-alignment bytes.','`noncode` rows are arithmetic-closed batch exclusions whose exact non-code status is proven but whose narrower padding-vs-data subtype is not explicit in preserved evidence. All `alignment` rows are NOP/LEA/MOV-self padding or explicitly documented zero/data-alignment bytes.')
ledger=ledger.replace('Do not add these 504 bytes','Do not add these 608 bytes')
ledger=ledger.replace('Completion requires the remaining 246 bytes','Completion requires the remaining 142 bytes')
assert sum(b-a+1 for a,b,_ in rows)==608
assert sum(b-a+1 for a,b,c in rows if c=='table')==122
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==443
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==43
ledger_path.write_text(ledger)

p=Path('Code1/PROGRESS.md'); t=p.read_text()
section='''\n\n## 2026-10-02 residual classification pass 25\n\nVerified translated code remains **69,652 / 70,402 = 98.93%**. Exact integrator arithmetic closed four preserved batches without assuming instruction bytes. Batch 13 proves 4 additional non-code bytes (`BCB3`, `BD83`, `BE26..BE27`); batch 14 proves 39 alignment/data bytes between its exact asserted low-continuation subranges; batch 18 proves 11 alignment bytes (`C79F`, `C8C1..C8C3`, `C9D2..C9D3`, `CACA..CACB`, `CBD1..CBD3`); and batch 11 proves 50 alignment bytes because its exact asserted intervals total the full declared 4,116 verified bytes across three 4,166-byte selected spans. `RESIDUAL_LEDGER.md` now classifies **608 bytes** (122 table + 443 alignment/zero/padding + 43 conservative generic non-code), leaving **142 bytes unresolved**. No translated-code count was increased.\n'''
if '## 2026-10-02 residual classification pass 25' not in t:t+=section
p.write_text(t)

p=Path('Code1/DECOMPILATION_CONTINUITY.md'); t=p.read_text()
section='''\n\n## 2026-10-02 residual pass 25 checkpoint\n\nIntegrator arithmetic, not padding inference, classified **104 more bytes**. Batch 11 contributes 50 alignment bytes; batch 13 contributes 4 generic non-code bytes omitted from its prose list but forced by its 4,181-byte span minus 4,129 verified bytes; batch 14 contributes 39 generic alignment/data bytes because its exact asserted low-continuation ranges sum to the declared code total; batch 18 contributes 11 alignment bytes as the complement of its four exact 1,066-byte code ranges inside `C79F..CBD3`. Classified non-code is now **608 bytes**, leaving **142 conservative residual bytes**. Verified translated code remains **69,652 bytes (98.93%)**. Do not use the stale batch-08 3,671-byte prose total for arithmetic classification; its named ranges do not reconcile to that number.\n'''
if '## 2026-10-02 residual pass 25 checkpoint' not in t:t+=section
p.write_text(t)

p=Path('Code1/STAGING_LEDGER.md'); t=p.read_text()
section='''\n\n## Residual pass 25\nArithmetic-closed integrator evidence classified 104 additional non-code bytes from batches 11, 13, 14, and 18. `RESIDUAL_LEDGER.md` now holds 608 classified bytes; 142 conservative residual bytes remain. Verified translated code stays 69,652 and active staging remains zero. The stale batch-08 prose total is not accepted as classification evidence.\n'''
if '## Residual pass 25' not in t:t+=section
p.write_text(t)
