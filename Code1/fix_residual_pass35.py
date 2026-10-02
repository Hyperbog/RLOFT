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
assert sum(b-a+1 for a,b,_ in rows)==705
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==471
assert sum(b-a+1 for a,b,c in rows if c=='table')==122
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==112

# RuntimeGapE0D0.staging.cpp explicitly records E0C8..E0CF as eight zeros.
new=(0x0E0C8,0x0E0CF,'alignment')
assert all(new[1] < a or new[0] > b for a,b,_ in rows)
rows.append(new); rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)

new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('explicitly classified residual below: **705 bytes**','explicitly classified residual below: **713 bytes**')
ledger=ledger.replace('alignment/zero/padding: **471 bytes**','alignment/zero/padding: **479 bytes**')
ledger=ledger.replace('residual still requiring classification: **332 bytes**','residual still requiring classification: **324 bytes**')
ledger=ledger.replace('Do not add these 705 bytes','Do not add these 713 bytes')
ledger=ledger.replace('remaining 332 bytes','remaining 324 bytes')
assert sum(b-a+1 for a,b,_ in rows)==713
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==479
assert sum(b-a+1 for a,b,c in rows if c=='table')==122
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==112
ledger_path.write_text(ledger)

# Repair the inventory: these wrappers were already integrated and counted by
# integrate_e174_wrappers.py (+57), but the current inventory omitted their row.
p=Path('Code1/PROGRESS.md'); t=p.read_text()
needle='- Runtime object clone `E0D0..E173`: **164 bytes**.\n'
row='- Runtime 7240 forwarding wrappers `E174..E186`, `E1F4..E206`, `E26C..E27E`: **57 bytes** (already included in the verified scalar).\n'
assert needle in t
if row not in t:t=t.replace(needle,needle+row)
section='''\n\n## 2026-10-02 residual classification pass 35\n\nPreserved `RuntimeGapE0D0.staging.cpp` evidence explicitly identifies `E0C8..E0CF` as **eight zero bytes**. They are now classified as alignment/zero-fill residual, increasing classified non-code from **705** to **713 bytes** and reducing unresolved residual from **332** to **324 bytes**. Verified translated code remains **69,365 / 70,402 = 98.53%**. The same audit confirmed that the three 19-byte 7240 wrappers at `E174..E186`, `E1F4..E206`, and `E26C..E27E` were already integrated and historically counted (+57), so no duplicate coverage credit was added; their inventory row is restored for clarity.\n'''
if '## 2026-10-02 residual classification pass 35' not in t:t+=section
p.write_text(t)

for name,title in [('Code1/STAGING_LEDGER.md','Residual pass 35'),('Code1/DECOMPILATION_CONTINUITY.md','2026-10-02 residual pass 35 checkpoint')]:
    p=Path(name); t=p.read_text()
    if title not in t:
        t+=f'''\n\n## {title}\nPreserved `RuntimeGapE0D0.staging.cpp` proves `E0C8..E0CF` are eight zero bytes; they are now classified non-code. Classified residual is **713 bytes** and **324 bytes remain unresolved**. Verified translated code remains **69,365 bytes (98.53%)**. The audit also confirmed that `E174..E186`, `E1F4..E206`, and `E26C..E27E` were already integrated/countable in the historical +57 wrapper fold, preventing a duplicate count.\n'''
    p.write_text(t)

cpp=Path('Code1/Code1.cpp').read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes=69365u;' in cpp
for token in ['function_e174','function_e1f4','function_e26c','static_assert(0xE187u-0xE174u==19u)','static_assert(0xE207u-0xE1F4u==19u)','static_assert(0xE27Fu-0xE26Cu==19u)']:
    assert token in cpp
print('residual pass35: +8 classified zero bytes; verified scalar unchanged')
