from pathlib import Path
import re

ledger_path=Path('Code1/RESIDUAL_LEDGER.md')
ledger=ledger_path.read_text()
new_ranges=[
 (0x0AEA6,0x0AEA7),(0x0AF4B,0x0AF4B),(0x0AF63,0x0AF63),
 (0x0B4D2,0x0B4D3),(0x0B51B,0x0B51B),(0x0B596,0x0B597),
 (0x0B5E1,0x0B5E3),(0x0B6BE,0x0B6BF),(0x0B6EB,0x0B6EB),
 (0x0B75B,0x0B75B),(0x0B76B,0x0B76B),
 (0x0CEE9,0x0CEEF),(0x0CF41,0x0CF43),
 (0x0E187,0x0E18F),(0x0E207,0x0E207),(0x0E27F,0x0E27F),
]
block_re=re.compile(r'(<!-- RESIDUAL-RANGES-BEGIN -->\n)(.*?)(\n<!-- RESIDUAL-RANGES-END -->)',re.S)
m=block_re.search(ledger); assert m
rows=[]
for raw in m.group(2).splitlines():
 mm=re.fullmatch(r'([0-9A-F]{5})\.\.([0-9A-F]{5}) \| (alignment|table) \| ([0-9]+)',raw.strip()); assert mm,raw
 rows.append((int(mm.group(1),16),int(mm.group(2),16),mm.group(3)))
existing={(a,b) for a,b,_ in rows}
for a,b in new_ranges:
 assert (a,b) not in existing,(a,b)
 rows.append((a,b,'alignment'))
rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)
new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('explicitly classified residual below: **462 bytes**','explicitly classified residual below: **500 bytes**')
ledger=ledger.replace('alignment/zero/padding: **344 bytes**','alignment/zero/padding: **382 bytes**')
ledger=ledger.replace('residual still requiring classification: **288 bytes**','residual still requiring classification: **250 bytes**')
ledger=ledger.replace('Do not add these 462 bytes','Do not add these 500 bytes')
ledger=ledger.replace('Completion requires the remaining 288 bytes','Completion requires the remaining 250 bytes')
assert len(rows)==147,len(rows)
assert sum(b-a+1 for a,b,_ in rows)==500
assert sum(b-a+1 for a,b,c in rows if c=='table')==118
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==382
ledger_path.write_text(ledger)

p=Path('Code1/PROGRESS.md'); t=p.read_text()
section='''\n\n## 2026-10-02 residual classification pass 23\n\nVerified translated code remains **69,652 / 70,402 = 98.93%**. Preserved repository evidence explicitly classifies **38 additional residual bytes** as non-code: batch-13 excluded islands `AEA6..AEA7`, `AF4B`, `AF63`, `B4D2..B4D3`, `B51B`, `B596..B597`, `B5E1..B5E3`, `B6BE..B6BF`, `B6EB`, `B75B`, `B76B`; startup `CEE9..CEEF` seven zero bytes and `CF41..CF43` alignment NOP; runtime staging `E187..E18F` nine zero bytes, `E207` single NOP, and `E27F` zero. `RESIDUAL_LEDGER.md` now classifies **500 bytes** (118 table + 382 alignment/zero/padding), leaving **250 bytes** unresolved. No code byte was promoted and the badge remains red.\n'''
if '## 2026-10-02 residual classification pass 23' not in t:t+=section
p.write_text(t)

p=Path('Code1/DECOMPILATION_CONTINUITY.md'); t=p.read_text()
section='''\n\n## 2026-10-02 residual pass 23 checkpoint\n\nPreserved batch/staging records supplied explicit non-code evidence for 38 more bytes: `AEA6..AEA7`, `AF4B`, `AF63`, `B4D2..B4D3`, `B51B`, `B596..B597`, `B5E1..B5E3`, `B6BE..B6BF`, `B6EB`, `B75B`, `B76B`, `CEE9..CEEF`, `CF41..CF43`, `E187..E18F`, `E207`, and `E27F`. Classified non-code is now **500 bytes**; **250 conservative residual bytes remain**. Verified translated code remains **69,652 bytes (98.93%)**. `C79F` was deliberately not classified in this pass because the older exclusion record describes the formerly unresolved self-modifying cluster, not the exact semantic role of that one byte after batch 18.\n'''
if '## 2026-10-02 residual pass 23 checkpoint' not in t:t+=section
p.write_text(t)

p=Path('Code1/STAGING_LEDGER.md'); t=p.read_text()
section='''\n\n## Residual pass 23\nPreserved batch/staging evidence classified 38 additional non-code bytes. `RESIDUAL_LEDGER.md` now holds 500 classified bytes; 250 conservative residual bytes remain. Verified translated code stays 69,652 and active staging remains zero.\n'''
if '## Residual pass 23' not in t:t+=section
p.write_text(t)
