from pathlib import Path
import base64,zlib,hashlib,re

root=Path('.')
cpp=root/'Code2/Code2.cpp'
prog=root/'Code2/PROGRESS.md'
ledger=root/'Code2/RESIDUAL_LEDGER.md'
cont=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
payload=root/'Code2/batch48.zb64'

c=cpp.read_text()
p=prog.read_text()
l=ledger.read_text()
a=cont.read_text()

assert 'constexpr u32 kVerifiedTranslatedBytes = 141234u;' in c
assert 'constexpr u32 kVerifiedNonCodeBytes = 2670u;' in c
assert 'static_assert(0x21702u-0x214B8u==586u);' in c
assert 'void function_21704(' not in c

src=zlib.decompress(base64.b64decode(payload.read_text().strip())).decode()
assert hashlib.sha256(src.encode()).hexdigest()=='275871f20eb28bb95b834bc94eb3b954d8e30cec1d04a5a5f21015f6563dff42'
assert 'static_assert(0x223F1u-0x21F70u==1153u);' in src
assert src.count('function_21f70')==1

c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 141234u;',
            'constexpr u32 kVerifiedTranslatedBytes = 144500u;',1)
c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 2670u;',
            'constexpr u32 kVerifiedNonCodeBytes = 2716u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 138937u +
              (0x210AFu-0x20E08u) + (0x211D8u-0x210B0u) +
              (0x214B8u-0x211D8u) + (0x21702u-0x214B8u));'''
new='''static_assert(kVerifiedTranslatedBytes == 141234u +
              (0x21880u-0x21704u) + (0x2195Bu-0x21880u) +
              (0x21ADBu-0x2195Cu) + (0x21B24u-0x21ADCu) +
              (0x21BEFu-0x21B44u) + (0x21DB1u-0x21BF0u) +
              (0x21F19u-0x21DB4u) + (0x21F6Eu-0x21F1Cu) +
              (0x223F1u-0x21F70u));'''
assert old in c
c=c.replace(old,new,1)
needle='\n} // namespace code2'
assert c.endswith(needle) or needle in c[-80:]
pos=c.rfind(needle)
c=c[:pos]+'\n'+src.rstrip()+'\n'+c[pos:]
cpp.write_text(c)

p=re.sub(r'object%20accounting-[0-9.]+%25-red','object%20accounting-61.42%25-red',p,count=1)
p=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*',
         '**Verified translated instruction bytes:** **144,500**',p,count=1)
p=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*',
         '**Proven non-code/data bytes:** **2,716**',p,count=1)
p=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[^\n]+',
         '**Reconciled object bytes:** **147,216 / 239,698 = 61.4173% (61.42%)**',p,count=1)
p=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*',
         '**Unresolved object bytes:** **92,482**',p,count=1)
checkpoint='''

## CODE.2 Batch 48 checkpoint

Batch 48 added **3,266 translated bytes** plus **46 proven non-code/data bytes**. Cumulative accounting is **144,500 translated + 2,716 non-code = 147,216 / 239,698 = 61.4173% (61.42%)**; unresolved is **92,482 bytes**.

The sequential frontier now reaches `223F3`; the next routine begins at `223F4`. `21B24..21B43` is the exact eight-entry CS jump table owned by `21B44`. Preserve `2195C`'s low-word-only `8000h` fixed-point overwrite, `21BF0`'s raw arithmetic `SAR` before later truncation-toward-zero division idioms, and `21F70`'s constant-nonzero `280E8h` test that makes the encoded `28110` branch dead. The long `21F70` orientation loops also retain their encoded unreachable inner arms instead of being normalized into a cleaner angle helper. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.
'''
assert '## CODE.2 Batch 48 checkpoint' not in p
p=p.rstrip()+checkpoint+'\n'
prog.write_text(p)

ranges='''2195B..2195B | alignment | 1
21ADB..21ADB | alignment | 1
21B24..21B43 | table | 32
21BEF..21BEF | alignment | 1
21DB1..21DB3 | alignment | 3
21F19..21F1B | alignment | 3
21F6E..21F6F | alignment | 2
223F1..223F3 | alignment | 3
'''
marker='<!-- RESIDUAL-RANGES-END -->'
assert marker in l and '21B24..21B43 | table | 32' not in l
l=l.replace(marker,ranges+marker,1)
l=l.replace('**Total proven non-code/data:** **2,670 bytes**.',
            '**Total proven non-code/data:** **2,716 bytes**.',1)
evidence='''

Batch 48 evidence: `21B24..21B43` is the exact eight-entry CS dword jump table consumed by `JMP DWORD PTR CS:[EAX*4+21B24h]` at `21B83`; its targets are `21B8B,21B9B,21BAB,21BEA,21BEA,21BBB,21BEA,21BCB`. `2195B`, `21ADB`, `21BEF`, `21DB1..21DB3`, `21F19..21F1B`, `21F6E..21F6F`, and `223F1..223F3` decode as explicit compiler NOP/LEA/MOV-EAX,EAX separators between directly reconstructed boundaries. A full object-1 direct relative call/jump target scan found no incoming target to any separator byte. The older zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.
'''
l=l.rstrip()+evidence+'\n'
ledger.write_text(l)

cont_checkpoint='''

## CODE.2 Batch 48 checkpoint
Batch 48 adds **3,266 translated bytes** and **46 proven non-code/data bytes**, bringing cumulative accounting to **144,500 translated + 2,716 non-code = 147,216 / 239,698 = 61.4173% (61.42%)**. The sequential frontier is now `223F4`. Preserve `2195C`'s low-word fixed-point overwrite, the mixed raw-SAR versus truncation-toward-zero arithmetic in `21BF0`, the exact eight-entry `21B24` table, and the intentionally dead constant-nonzero branch in `21F70`.
'''
assert '## CODE.2 Batch 48 checkpoint' not in a
a=a.rstrip()+cont_checkpoint+'\n'
cont.write_text(a)
