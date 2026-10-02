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
assert sum(b-a+1 for a,b,_ in rows)==727
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==479
assert sum(b-a+1 for a,b,c in rows if c=='table')==122
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==126

# Pass 38 uses preserved historical integration evidence, not visual/alignment guesses.
# bcb06766... documents the entry JMP at CC1C and explicitly identifies CC1E..CC95
# as 120 bytes of embedded Watcom run-time text/data skipped by that jump.
# c6fa0ed1... explicitly identifies DF8E..DFB2 and E02B..E04F as two identical
# radix digit alphabets (37 bytes each), data rather than executable instructions.
new_rows=[
    (0x0CC1E,0x0CC95,'noncode'),
    (0x0DF8E,0x0DFB2,'table'),
    (0x0E02B,0x0E04F,'table'),
]
for nr in new_rows:
    assert all(nr[1] < a or nr[0] > b for a,b,_ in rows),nr
    rows.append(nr)
rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)

# Exact closure checks for the two historical regions.
assert 0x0CD97-0x0CC1C == 379
assert (0x0CC1E-0x0CC1C) + (0x0CD97-0x0CC96) == 259
assert 0x0CC96-0x0CC1E == 120
assert 0x0E0C8-0x0DDE0 == 744
assert 663 + 1 + 6 + 37 + 37 == 744

new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('explicitly classified residual below: **727 bytes**','explicitly classified residual below: **921 bytes**')
ledger=ledger.replace('embedded tables: **122 bytes**','embedded tables: **196 bytes**')
ledger=ledger.replace('proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **126 bytes**','proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **246 bytes**')
ledger=ledger.replace('residual still requiring classification: **310 bytes**','residual still requiring classification: **116 bytes**')
ledger=ledger.replace('Do not add these 727 bytes','Do not add these 921 bytes')
ledger=ledger.replace('Completion requires the remaining 310 bytes','Completion requires the remaining 116 bytes')
needle='- `09EA3..09EA6`: preserved four-byte prefix of the batch-12 lookup data island; `09EA7` and following bytes were subsequently verified as code and are not classified here.\n'
assert needle in ledger
ledger=ledger.replace(needle,needle+'- `0DF8E..0DFB2` and `0E02B..0E04F`: two independently embedded 37-byte radix digit alphabets used by the conversion helpers.\n',1)
marker='`noncode` rows are arithmetic-closed batch exclusions whose exact non-code status is proven but whose narrower padding-vs-data subtype is not explicit in preserved evidence.'
assert marker in ledger
ledger=ledger.replace(marker,'`0CC1E..0CC95` is the 120-byte embedded Watcom run-time identification text/data block skipped by the encoded `CC1C` entry jump.\n\n'+marker,1)
assert sum(b-a+1 for a,b,_ in rows)==921
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==479
assert sum(b-a+1 for a,b,c in rows if c=='table')==196
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==246
ledger_path.write_text(ledger)

section='''\n\n## 2026-10-02 residual classification pass 38\n\nHistorical integration evidence closes **194 additional non-code bytes** without promoting any unverified instructions. Commit `bcb06766a868cec3bf7c8b93eda2faa128088046` records the encoded `CC1C` short jump to `CC96` and explicitly identifies `CC1E..CC95` as **120 bytes of embedded Watcom run-time text/data**, not executable instructions. This exactly reconciles the startup inventory: `CC1C..CD96` is 379 bytes total, partitioned into **259 translated bytes** (`CC1C..CC1D` plus `CC96..CD96`) and **120 data bytes**.\n\nCommit `c6fa0ed1974736c6c124b3f600f2b7d36fc2ed94` explicitly identifies `DF8E..DFB2` and `E02B..E04F` as two identical embedded radix digit alphabets, **37 bytes each**. Together with the already-classified `DE6F` and `DEA1..DEA6`, these **74 data bytes** close the full `DDE0..E0C7` inventory exactly: **663 translated + 81 non-code = 744 bytes**.\n\nClassified residual therefore rises from **727** to **921 bytes** (479 alignment/zero/padding, 196 table/lookup data, 246 generic proven non-code), leaving **116 bytes unresolved**. Verified translated code remains **69,365 / 70,402 = 98.53%** and the badge remains red.\n'''
for name in ('Code1/PROGRESS.md','Code1/STAGING_LEDGER.md','Code1/DECOMPILATION_CONTINUITY.md'):
    p=Path(name); text=p.read_text()
    assert 'residual classification pass 38' not in text.lower()
    p.write_text(text+section)

cpp=Path('Code1/Code1.cpp').read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes=69365u;' in cpp
assert 'translate_cc1c_ccdd' in cpp
assert 'function_dfb3' in cpp and 'function_e050' in cpp
print('residual pass38: +194 classified non-code/data bytes; unresolved 116; verified scalar unchanged')
