from pathlib import Path
import re

# Residual pass 42: recover exact padding classifications from the repository's
# older uppercase CODE1 reconstruction tree.  These are address-specific comments
# in files that explicitly describe themselves as direct reconstructions of CODE.1;
# they are not inferred merely from alignment appearance.
#
# Historical blob evidence used:
# cbae45e9... CODE1/UiEntry10A4.cpp: 1462..1463 MOV EAX,EAX padding.
# 089a32e3... CODE1/CharacterTable5F4C.h: 6054..605F twelve zero bytes.
# e1ad9141... CODE1/Code1Service9AE8.h: 9BD7..9BDF and 9C98..9C9F zero/data padding.
# c0c5025b... CODE1/Code1Vga9CA0.h: 9E89..9E8B, 9EAE..9EAF,
#              A00E..A00F, A059..A05B alignment.
# 4ec435c3... CODE1/Code1VgaDrawA05C.h: A21A..A21B alignment.
# c8e3c403... CODE1/Code1VgaClippedAA20.h: AE0B alignment.
# 4dc22f85... CODE1/AudioControlBB80.h: BE61..BE6F zero/data padding.

ledger_path=Path('Code1/RESIDUAL_LEDGER.md')
ledger=ledger_path.read_text()
block_re=re.compile(r'(<!-- RESIDUAL-RANGES-BEGIN -->\n)(.*?)(\n<!-- RESIDUAL-RANGES-END -->)',re.S)
m=block_re.search(ledger); assert m
rows=[]
for raw in m.group(2).splitlines():
    mm=re.fullmatch(r'([0-9A-F]{5})\.\.([0-9A-F]{5}) \| (alignment|table|noncode) \| ([0-9]+)',raw.strip()); assert mm,raw
    a,b=int(mm.group(1),16),int(mm.group(2),16); n=int(mm.group(4)); assert b-a+1==n
    rows.append((a,b,mm.group(3)))
assert sum(b-a+1 for a,b,_ in rows)==937
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==479
assert sum(b-a+1 for a,b,c in rows if c=='table')==196
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==262

new_alignment=[
    (0x01462,0x01463),
    (0x06054,0x0605F),
    (0x09BD7,0x09BDF),
    (0x09C98,0x09C9F),
    (0x09E89,0x09E8B),
    (0x09EAE,0x09EAF),
    (0x0A00E,0x0A00F),
    (0x0A059,0x0A05B),
    (0x0A21A,0x0A21B),
    (0x0AE0B,0x0AE0B),
    (0x0BE61,0x0BE6F),
]
assert sum(b-a+1 for a,b in new_alignment)==59
for a,b in new_alignment:
    assert all(b < x or a > y for x,y,_ in rows),(hex(a),hex(b))
    rows.append((a,b,'alignment'))
rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)

new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('- explicitly classified residual below: **937 bytes**','- explicitly classified residual below: **996 bytes**',1)
ledger=ledger.replace('  - alignment/zero/padding: **479 bytes**','  - alignment/zero/padding: **538 bytes**',1)
ledger=ledger.replace('- residual still requiring classification: **105 bytes**','- residual still requiring classification: **46 bytes**',1)
assert '## Remaining exact unresolved islands after pass 41' in ledger
remaining=[
 (0x000EB,0x000EB),(0x04977,0x04977),(0x087C2,0x087C3),(0x08E1F,0x08E1F),
 (0x0CC19,0x0CC1B),(0x0D8CE,0x0D8CF),(0x0DDD8,0x0DDDF),(0x0E3B3,0x0E3B3),
 (0x0F742,0x0F74F),(0x0F7B3,0x0F7B7),(0x0F8BD,0x0F8C3),(0x0FC97,0x0FC97),
]
assert sum(b-a+1 for a,b in remaining)==46
section='''\n\n## 2026-10-02 residual classification pass 42\n\nA repository-wide history scan recovered the older uppercase `CODE1/` reconstruction tree, which contains exact address-specific padding annotations derived during its direct CODE.1 reconstruction. Unlike broad batch prose, these annotations name the residual bytes themselves. Pass 42 therefore classifies **59 bytes** as alignment/zero/data padding: `1462..1463` (explicit `MOV EAX,EAX` padding), `6054..605F` (twelve zero bytes), `9BD7..9BDF` and `9C98..9C9F` (zero/data padding), `9E89..9E8B`, `9EAE..9EAF`, `A00E..A00F`, `A059..A05B`, `A21A..A21B`, `AE0B` (explicit alignment), and `BE61..BE6F` (zero/data padding).\n\nThe supporting historical blobs are `cbae45e9c4e3dd5350797f67090a4438326e9eea`, `089a32e37d448c797f7cbabe24ff140f6c08b021`, `e1ad914147727192a8449520139129654477b96a`, `c0c5025b20873cc1c6b1b81557827e55e95d569c`, `4ec435c37f31bfe89a8e711ef6a709848ff3a5bf`, `c8e3c403350b386b21fbbbc9c17517cd0fdc7cfc`, and `4dc22f8546cbd2d02c6b8070be1040a18008c74c`. Their boundaries agree with the currently integrated routines. This does not resurrect the discredited blanket assumption that every old recovery-batch gap is padding; only individually named ranges are accepted.\n\nClassified residual rises from **937** to **996 bytes** (196 table, 538 alignment/zero/padding, 262 generic proven non-code), leaving **46 bytes in 12 islands** unresolved. Verified translated code remains **69,360 / 70,402 = 98.52%** and the badge remains red.\n'''
# Replace the stale remaining-islands heading/list with an explicit pass-42 list while preserving history above it.
ledger += section
ledger += '\n## Remaining exact unresolved islands after pass 42\n\nThe repository-wide code/non-code union now leaves exactly **46 bytes in 12 islands**. These remain unresolved rather than being labeled from alignment appearance alone:\n\n'
ledger += '\n'.join(f'- `{a:05X}..{b:05X}`: **{b-a+1} byte'+('' if a==b else 's')+'**.' for a,b in remaining) + '\n'
assert sum(b-a+1 for a,b,_ in rows)==996
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==538
ledger_path.write_text(ledger)

checkpoint='''\n\n## 2026-10-02 residual classification pass 42\n\nRepository-wide history recovery found exact address-level padding annotations in the older uppercase `CODE1/` direct-reconstruction tree. Individually named ranges totaling **59 bytes** are now classified without using alignment appearance as inference: `1462..1463`, `6054..605F`, `9BD7..9BDF`, `9C98..9C9F`, `9E89..9E8B`, `9EAE..9EAF`, `A00E..A00F`, `A059..A05B`, `A21A..A21B`, `AE0B`, and `BE61..BE6F`. Classified non-code is **996 bytes** and **46 bytes / 12 islands remain unresolved**. Verified translated code is unchanged at **69,360 / 70,402 = 98.52%**; badge remains red. Only exact range annotations were accepted; broad historical claims remain insufficient.\n'''
for name in ('Code1/PROGRESS.md','Code1/STAGING_LEDGER.md','Code1/DECOMPILATION_CONTINUITY.md'):
    p=Path(name); text=p.read_text()
    assert 'residual classification pass 42' not in text.lower()
    p.write_text(text+checkpoint)

cpp=Path('Code1/Code1.cpp').read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes=69360u;' in cpp
print('pass42: +59 exact historical padding bytes; classified=996 unresolved=46 translated=69360')
