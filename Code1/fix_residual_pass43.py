from pathlib import Path
import re

ledger_path=Path('Code1/RESIDUAL_LEDGER.md')
ledger=ledger_path.read_text()
block_re=re.compile(r'(<!-- RESIDUAL-RANGES-BEGIN -->\n)(.*?)(\n<!-- RESIDUAL-RANGES-END -->)',re.S)
m=block_re.search(ledger); assert m
rows=[]
for raw in m.group(2).splitlines():
    mm=re.fullmatch(r'([0-9A-F]{5})\.\.([0-9A-F]{5}) \| (alignment|table|noncode) \| ([0-9]+)',raw.strip()); assert mm,raw
    a,b=int(mm.group(1),16),int(mm.group(2),16); cat=mm.group(3); n=int(mm.group(4))
    assert b-a+1==n
    rows.append((a,b,cat))
assert sum(b-a+1 for a,b,_ in rows)==996
assert not any(a<=0x000EB<=b for a,b,_ in rows)
rows.append((0x000EB,0x000EB,'alignment'))
rows.sort()
for x,y in zip(rows,rows[1:]): assert x[1] < y[0],(x,y)
new_block='\n'.join(f'{a:05X}..{b:05X} | {cat} | {b-a+1}' for a,b,cat in rows)
ledger=block_re.sub(lambda mm:mm.group(1)+new_block+mm.group(3),ledger)
ledger=ledger.replace('- explicitly classified residual below: **996 bytes**','- explicitly classified residual below: **997 bytes**',1)
ledger=ledger.replace('  - alignment/zero/padding: **538 bytes**','  - alignment/zero/padding: **539 bytes**',1)
ledger=ledger.replace('- residual still requiring classification: **46 bytes**','- residual still requiring classification: **45 bytes**',1)
section='''\n\n## 2026-10-02 residual classification pass 43\n\nA normalized historical-address sweep recovered one exact residual annotation that the earlier literal-string scan could miss because of address formatting: historical `CODE1/ObjectLayout0000.cpp` explicitly records `0x00EB is NOP alignment` after the directly reconstructed `004C..00EA` text-record routine. Pass 43 therefore classifies **1 byte**, `000EB`, as alignment. No other remaining island received an exact target-specific code/data/padding annotation in the normalized historical `CODE1/` sweep; broad runtime-tail labels are not sufficient to classify individual bytes.\n\nClassified residual rises from **996** to **997 bytes** (196 table, 539 alignment/zero/padding, 262 generic proven non-code), leaving **45 bytes in 11 islands** unresolved. Verified translated code remains **69,360 / 70,402 = 98.52%** and the badge remains red.\n\n## Remaining exact unresolved islands after pass 43\n\n- `04977..04977`: **1 byte**.\n- `087C2..087C3`: **2 bytes**.\n- `08E1F..08E1F`: **1 byte**.\n- `0CC19..0CC1B`: **3 bytes**.\n- `0D8CE..0D8CF`: **2 bytes**.\n- `0DDD8..0DDDF`: **8 bytes**.\n- `0E3B3..0E3B3`: **1 byte**.\n- `0F742..0F74F`: **14 bytes**.\n- `0F7B3..0F7B7`: **5 bytes**.\n- `0F8BD..0F8C3`: **7 bytes**.\n- `0FC97..0FC97`: **1 byte**.\n'''
assert '## 2026-10-02 residual classification pass 43' not in ledger
ledger += section
assert sum(b-a+1 for a,b,_ in rows)==997
assert sum(b-a+1 for a,b,c in rows if c=='alignment')==539
assert sum(b-a+1 for a,b,c in rows if c=='table')==196
assert sum(b-a+1 for a,b,c in rows if c=='noncode')==262
ledger_path.write_text(ledger)

p=Path('Code1/PROGRESS.md'); t=p.read_text()
section='''\n\n## 2026-10-02 residual classification pass 43\n\nVerified translated code remains **69,360 / 70,402 = 98.52%**. Historical `CODE1/ObjectLayout0000.cpp` explicitly identifies residual byte `000EB` as NOP alignment. The normalized historical-address sweep found no equivalent target-specific annotation for the other surviving islands, so only this one byte is promoted. `RESIDUAL_LEDGER.md` now classifies **997 bytes**, leaving **45 bytes in 11 islands** unresolved. The progress badge remains red.\n'''
assert '## 2026-10-02 residual classification pass 43' not in t
t += section; p.write_text(t)

p=Path('Code1/STAGING_LEDGER.md'); t=p.read_text()
section='''\n\n## Residual pass 43\nHistorical `CODE1/ObjectLayout0000.cpp` explicitly labels `000EB` as NOP alignment. One byte is newly classified; translated code remains 69,360 bytes and active staging remains zero. The residual queue is now 45 bytes in 11 islands.\n'''
assert '## Residual pass 43' not in t
t += section; p.write_text(t)

p=Path('Code1/DECOMPILATION_CONTINUITY.md'); t=p.read_text()
section='''\n\n## 2026-10-02 residual pass 43 checkpoint\n\nA normalized scan of the historical uppercase `CODE1/` reconstruction recovered direct byte-level evidence for `000EB`: `ObjectLayout0000.cpp` records it as a NOP alignment byte immediately after reconstructed `004C..00EA`. This raises classified residual to **997 bytes** and leaves **45 bytes in 11 islands** unresolved. Verified translated code remains **69,360 bytes (98.52%)**. The same sweep found no target-specific classification for the other islands; do not infer their status from adjacency or alignment appearance.\n'''
assert '## 2026-10-02 residual pass 43 checkpoint' not in t
t += section; p.write_text(t)

p=Path('.github/workflows/residual-audit.yml'); t=p.read_text()
t=t.replace("assert '- explicitly classified residual below: **996 bytes**' in ledger","assert '- explicitly classified residual below: **997 bytes**' in ledger",1)
t=t.replace("assert '  - alignment/zero/padding: **538 bytes**' in ledger","assert '  - alignment/zero/padding: **539 bytes**' in ledger",1)
t=t.replace("assert '- residual still requiring classification: **46 bytes**' in ledger","assert '- residual still requiring classification: **45 bytes**' in ledger",1)
t=t.replace("assert '## Remaining exact unresolved islands after pass 42' in ledger","assert '## Remaining exact unresolved islands after pass 43' in ledger",1)
t=t.replace('assert 69360 + 996 + 46 == 70402','assert 69360 + 997 + 45 == 70402',1)
t=t.replace('assert sum(b-a+1 for a,b,_ in rows)==996','assert sum(b-a+1 for a,b,_ in rows)==997',1)
t=t.replace("assert sum(b-a+1 for a,b,c in rows if c=='alignment')==538","assert sum(b-a+1 for a,b,c in rows if c=='alignment')==539",1)
t=t.replace("print('CODE.1 accounting verified: 69360 translated + 996 classified + 46 unresolved = 70402')","print('CODE.1 accounting verified: 69360 translated + 997 classified + 45 unresolved = 70402')",1)
assert '996 classified' not in t
p.write_text(t)
