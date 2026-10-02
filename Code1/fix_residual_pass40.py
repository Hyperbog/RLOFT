from pathlib import Path
import re

# Residual pass 40: classify only arithmetic-closed complements of preserved,
# explicitly bounded deferred regions / verified runtime families.
# No subtype is guessed: these are generic non-code exclusions.
new_noncode=[
    (0x007E2,0x007E3),
    (0x080BA,0x080BB),
    (0x0826E,0x0826F),
    (0x08336,0x08337),
    (0x0DC8D,0x0DC8F),
]
assert sum(b-a+1 for a,b in new_noncode)==11

# Exact arithmetic closure from repository-preserved evidence:
# - batch-14 checkpoint identified 07E2..10A1 as the unresolved early-game region;
#   batch 16 later verified 07E4..10A1 (2238 / 2240 bytes), leaving 07E2..07E3.
assert 0x10A2-0x07E2 == 2240
assert 0x10A2-0x07E4 == 2238
# - batch 10 bounded the deferred second interrupt/state-machine block as 80BA..826F;
#   batch 16 verified 80BC..826D (434 / 438), leaving two bytes at each edge.
assert 0x8270-0x80BA == 438
assert 0x826E-0x80BC == 434
# - batch 10 bounded the deferred 8336..8397 interrupt block; batch 16 verified
#   8338..8397 (96 / 98), leaving 8336..8337.
assert 0x8398-0x8336 == 98
assert 0x8398-0x8338 == 96
# - the declared DAD9..DDD7 allocator/decoder/transform family is 767 bytes;
#   its three exact verified assertions total 764, leaving only DC8D..DC8F.
assert 0xDDD8-0xDAD9 == 767
assert (0xDB80-0xDAD9)+(0xDC8D-0xDB80)+(0xDDD8-0xDC90)==764

cpp=Path('Code1/Code1.cpp').read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes=69360u;' in cpp
assert 'static_assert(0x10A2u-0x07E4u==2238u);' in cpp
assert 'static_assert(0x826Eu-0x80BCu==434u);' in cpp
assert 'static_assert(0x8398u-0x8338u==96u);' in cpp
assert 'static_assert(0xDB80u-0xDAD9u==167u); static_assert(0xDC8Du-0xDB80u==269u); static_assert(0xDDD8u-0xDC90u==328u);' in cpp

ledger_path=Path('Code1/RESIDUAL_LEDGER.md')
ledger=ledger_path.read_text()
assert '- explicitly classified residual below: **921 bytes**' in ledger
assert '  - embedded tables: **196 bytes**' in ledger
assert '  - alignment/zero/padding: **479 bytes**' in ledger
assert '  - proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **246 bytes**' in ledger
assert '- residual still requiring classification: **121 bytes**' in ledger

block_re=re.compile(r'(<!-- RESIDUAL-RANGES-BEGIN -->\n)(.*?)(\n<!-- RESIDUAL-RANGES-END -->)',re.S)
m=block_re.search(ledger); assert m
rows=[]
for raw in m.group(2).splitlines():
    mm=re.fullmatch(r'([0-9A-F]{5})\.\.([0-9A-F]{5}) \| (alignment|table|noncode) \| ([0-9]+)',raw.strip()); assert mm,raw
    a,b=int(mm.group(1),16),int(mm.group(2),16)
    assert b-a+1==int(mm.group(4))
    rows.append((a,b,mm.group(3)))
assert sum(b-a+1 for a,b,_ in rows)==921
for a,b in new_noncode:
    assert all(b<x or a>y for x,y,_ in rows),(a,b)
    rows.append((a,b,'noncode'))
rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)
new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('- explicitly classified residual below: **921 bytes**','- explicitly classified residual below: **932 bytes**',1)
ledger=ledger.replace('  - proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **246 bytes**','  - proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **257 bytes**',1)
ledger=ledger.replace('- residual still requiring classification: **121 bytes**','- residual still requiring classification: **110 bytes**',1)
ledger=ledger.replace('Do not add these 921 bytes','Do not add these 932 bytes')
ledger=ledger.replace('Completion requires the remaining 121 bytes','Completion requires the remaining 110 bytes')

remaining=[
(0x000EB,0x000EB),(0x01462,0x01463),(0x04977,0x04977),(0x06054,0x0605F),
(0x087C2,0x087C3),(0x08E1F,0x08E1F),(0x09BD7,0x09BDF),(0x09C98,0x09C9F),
(0x09E89,0x09E8B),(0x09EAE,0x09EAF),(0x0A00E,0x0A00F),(0x0A059,0x0A05B),
(0x0A21A,0x0A21B),(0x0A5C6,0x0A5C7),(0x0A675,0x0A677),(0x0AE0B,0x0AE0B),
(0x0BE61,0x0BE6F),(0x0CC19,0x0CC1B),(0x0D8CE,0x0D8CF),(0x0DDD8,0x0DDDF),
(0x0E3B3,0x0E3B3),(0x0F742,0x0F74F),(0x0F7B3,0x0F7B7),(0x0F8BD,0x0F8C3),(0x0FC97,0x0FC97),
]
assert sum(b-a+1 for a,b in remaining)==110
section='''\n\n## Remaining exact unresolved islands after pass 40\n\nThe repository-wide code/non-code union now leaves exactly **110 bytes in 25 islands**. These remain unresolved rather than being labeled from alignment appearance alone:\n\n'''+ '\n'.join(f'- `{a:05X}..{b:05X}`: **{b-a+1} byte'+('' if b==a else 's')+'**.' for a,b in remaining) + '\n'
assert '## Remaining exact unresolved islands after pass 40' not in ledger
ledger += section
assert sum(b-a+1 for a,b,_ in rows)==932
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==479
assert sum(b-a+1 for a,b,c in rows if c=='table')==196
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==257
ledger_path.write_text(ledger)

checkpoint='''\n\n## 2026-10-02 residual classification pass 40\n\nA repository-history/reference audit closed **11 additional bytes** using arithmetic-bounded regions that were explicitly preserved before their later translations. The batch-14 continuation named `07E2..10A1` as the remaining early-game candidate; batch 16 subsequently verified `07E4..10A1`, leaving `07E2..07E3` outside instructions. Batch 10 bounded the deferred second interrupt/state-machine block as `80BA..826F`; batch 16 verified `80BC..826D`, leaving `80BA..80BB` and `826E..826F`. Batch 10 likewise bounded `8336..8397`; batch 16 verified `8338..8397`, leaving `8336..8337`. Finally, the declared `DAD9..DDD7` allocator/decoder/transform family spans 767 bytes while its three exact verified assertions total 764, proving `DC8D..DC8F` is the only excluded island.\n\nAll **11 bytes** are recorded as generic proven non-code because preserved evidence closes them arithmetically but does not uniquely identify their narrower padding/data subtype. Classified residual rises from **921** to **932 bytes**; unresolved residual falls from **121** to **110 bytes**. Verified translated code remains **69,360 / 70,402 = 98.52%** and the badge remains red.\n'''
for name in ('Code1/PROGRESS.md','Code1/STAGING_LEDGER.md','Code1/DECOMPILATION_CONTINUITY.md'):
    p=Path(name); text=p.read_text()
    assert 'residual classification pass 40' not in text.lower()
    p.write_text(text+checkpoint)

print('residual pass40: +11 generic non-code bytes; classified=932; unresolved=110; verified scalar unchanged')
