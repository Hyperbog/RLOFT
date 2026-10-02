from pathlib import Path
import re

# Residual pass 41: close the complete batch-12 A558..A6EF image-helper family
# by the same arithmetic-complement rule used in pass 37.  The preserved batch-
# 12 integrator names A558..A6EF as an integrated span (not merely a selected
# code subset) and gives four exact instruction intervals:
#   A558..A57B  36 bytes
#   A57C..A5C5  74 bytes
#   A5C8..A674 173 bytes
#   A678..A6EF 120 bytes
# The inclusive family contains 408 bytes; the exact translated intervals total
# 403.  Therefore A5C6..A5C7 and A675..A677 are the complete 5-byte complement.
# The preserved evidence does not identify their narrower padding/data subtype,
# so record them only as generic proven non-code.
new_noncode=[(0x0A5C6,0x0A5C7),(0x0A675,0x0A677)]
assert sum(b-a+1 for a,b in new_noncode)==5
assert 0x0A6F0-0x0A558 == 408
assert (0x0A57C-0x0A558)+(0x0A5C6-0x0A57C)+(0x0A675-0x0A5C8)+(0x0A6F0-0x0A678) == 403

cpp=Path('Code1/Code1.cpp').read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes=69360u;' in cpp
assert 'static_assert(0xA57Cu-0xA558u==36u);' in cpp
assert 'static_assert(0xA5C6u-0xA57Cu==74u);' in cpp
assert 'static_assert(0xA675u-0xA5C8u==173u);' in cpp
assert 'static_assert(0xA6F0u-0xA678u==120u);' in cpp

ledger_path=Path('Code1/RESIDUAL_LEDGER.md')
ledger=ledger_path.read_text()
assert '- explicitly classified residual below: **932 bytes**' in ledger
assert '  - embedded tables: **196 bytes**' in ledger
assert '  - alignment/zero/padding: **479 bytes**' in ledger
assert '  - proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **257 bytes**' in ledger
assert '- residual still requiring classification: **110 bytes**' in ledger

block_re=re.compile(r'(<!-- RESIDUAL-RANGES-BEGIN -->\n)(.*?)(\n<!-- RESIDUAL-RANGES-END -->)',re.S)
m=block_re.search(ledger); assert m
rows=[]
for raw in m.group(2).splitlines():
    mm=re.fullmatch(r'([0-9A-F]{5})\.\.([0-9A-F]{5}) \| (alignment|table|noncode) \| ([0-9]+)',raw.strip()); assert mm,raw
    a,b=int(mm.group(1),16),int(mm.group(2),16)
    assert b-a+1==int(mm.group(4))
    rows.append((a,b,mm.group(3)))
assert sum(b-a+1 for a,b,_ in rows)==932
for a,b in new_noncode:
    assert all(b<x or a>y for x,y,_ in rows),(a,b)
    rows.append((a,b,'noncode'))
rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)
new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('- explicitly classified residual below: **932 bytes**','- explicitly classified residual below: **937 bytes**',1)
ledger=ledger.replace('  - proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **257 bytes**','  - proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **262 bytes**',1)
ledger=ledger.replace('- residual still requiring classification: **110 bytes**','- residual still requiring classification: **105 bytes**',1)
ledger=ledger.replace('Do not add these 932 bytes','Do not add these 937 bytes')
ledger=ledger.replace('Completion requires the remaining 110 bytes','Completion requires the remaining 105 bytes')

remaining=[
(0x000EB,0x000EB),(0x01462,0x01463),(0x04977,0x04977),(0x06054,0x0605F),
(0x087C2,0x087C3),(0x08E1F,0x08E1F),(0x09BD7,0x09BDF),(0x09C98,0x09C9F),
(0x09E89,0x09E8B),(0x09EAE,0x09EAF),(0x0A00E,0x0A00F),(0x0A059,0x0A05B),
(0x0A21A,0x0A21B),(0x0AE0B,0x0AE0B),(0x0BE61,0x0BE6F),(0x0CC19,0x0CC1B),
(0x0D8CE,0x0D8CF),(0x0DDD8,0x0DDDF),(0x0E3B3,0x0E3B3),(0x0F742,0x0F74F),
(0x0F7B3,0x0F7B7),(0x0F8BD,0x0F8C3),(0x0FC97,0x0FC97),
]
assert sum(b-a+1 for a,b in remaining)==105
section='''\n\n## Remaining exact unresolved islands after pass 41\n\nThe repository-wide code/non-code union now leaves exactly **105 bytes in 23 islands**. These remain unresolved rather than being labeled from alignment appearance alone:\n\n'''+ '\n'.join(f'- `{a:05X}..{b:05X}`: **{b-a+1} byte'+('' if b==a else 's')+'**.' for a,b in remaining) + '\n'
assert '## Remaining exact unresolved islands after pass 41' not in ledger
ledger += section
assert sum(b-a+1 for a,b,_ in rows)==937
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==479
assert sum(b-a+1 for a,b,c in rows if c=='table')==196
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==262
ledger_path.write_text(ledger)

checkpoint='''\n\n## 2026-10-02 residual classification pass 41\n\nThe preserved batch-12 integrator closes **5 additional bytes** by exact arithmetic. Its complete `A558..A6EF` image-helper family contains **408 bytes**. The four exact translated intervals `A558..A57B` (**36**), `A57C..A5C5` (**74**), `A5C8..A674` (**173**), and `A678..A6EF` (**120**) total **403 bytes**, proving `A5C6..A5C7` and `A675..A677` are the complete five-byte exclusion. They are recorded conservatively as generic proven non-code because the preserved evidence does not uniquely recover padding-vs-data subtype.\n\nClassified residual rises from **932** to **937 bytes**; unresolved residual falls from **110 bytes / 25 islands** to **105 bytes / 23 islands**. Verified translated code remains **69,360 / 70,402 = 98.52%** and the badge remains red.\n'''
for name in ('Code1/PROGRESS.md','Code1/STAGING_LEDGER.md','Code1/DECOMPILATION_CONTINUITY.md'):
    p=Path(name); text=p.read_text()
    assert 'residual classification pass 41' not in text.lower()
    p.write_text(text+checkpoint)

print('residual pass41: +5 generic non-code bytes; classified=937; unresolved=105; verified scalar unchanged')
