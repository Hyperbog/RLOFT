from pathlib import Path

root=Path('.')
cpp=root/'Code2/Code2.cpp'
ledger=root/'Code2/RESIDUAL_LEDGER.md'
progress=root/'Code2/PROGRESS.md'
addendum=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
staging=root/'Code2/batch49.staging.cpp'

c=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 144500u;' in c
assert 'constexpr u32 kVerifiedNonCodeBytes = 2716u;' in c
c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 144500u;',
            'constexpr u32 kVerifiedTranslatedBytes = 147003u;',1)
c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 2716u;',
            'constexpr u32 kVerifiedNonCodeBytes = 2849u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 141234u +
              (0x21880u-0x21704u) + (0x2195Bu-0x21880u) +
              (0x21ADBu-0x2195Cu) + (0x21B24u-0x21ADCu) +
              (0x21BEFu-0x21B44u) + (0x21DB1u-0x21BF0u) +
              (0x21F19u-0x21DB4u) + (0x21F6Eu-0x21F1Cu) +
              (0x223F1u-0x21F70u));'''
new='''static_assert(kVerifiedTranslatedBytes == 144500u +
              (0x22891u-0x223F4u) + (0x2294Du-0x22894u) +
              (0x22CE0u-0x229CCu) + (0x22D70u-0x22CE0u) +
              (0x22D8Bu-0x22D70u) + (0x22E3Eu-0x22D8Cu));'''
assert old in c
c=c.replace(old,new,1)
end='\n} // namespace code2\n'
assert c.endswith(end)
batch=staging.read_text().rstrip()+'\n'
c=c[:-len(end)]+'\n'+batch+end
cpp.write_text(c)

l=ledger.read_text()
marker='<!-- RESIDUAL-RANGES-END -->'
assert marker in l
ranges='''22891..22893 | alignment | 3
2294D..2294F | alignment | 3
22950..229A3 | table | 84
229A4..229CB | table | 40
22D8B..22D8B | alignment | 1
22E3E..22E3F | padding | 2
'''
assert '22891..22893 | alignment | 3' not in l
l=l.replace(marker,ranges+marker,1)
l=l.replace('**Total proven non-code/data:** **2,716 bytes**.',
            '**Total proven non-code/data:** **2,849 bytes**.',1)
evidence='''

Batch 49 evidence: `22950..229A3` is the exact 21-entry CS dword table consumed by the bounded `ECX <= 14h` indirect jump at `22A2B`; its entries are `22A33,22C02,22C28,22C48,22C6E,22C02,22C28,22C48,22C6E,22C02,22C28,22C48,22C6E,22C02,22C28,22C48,22C6E,22C94,22CB4,22CD4,22B95`. `229A4..229CB` is the exact ten-entry CS table consumed at `22B34` after the record-type byte is decremented and bounded to `0..9`; its targets are `22B3C,22BA6,22BF5,22BC1,22BF5,22BF5,22BF5,22BF5,22BF5,22BE3`. `22891..22893` and `2294D..2294F` decode as `LEA EAX,[EAX+0]`, `22D8B` is `NOP`, and `22E3E..22E3F` is the two-byte zero pad between the directly reconstructed `22D8C` return and the independent `22E40` entry. A full object-1 direct relative CALL/JMP/Jcc target scan found no incoming target to any separator or pad byte. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.'''
l=l.rstrip()+evidence+'\n'
ledger.write_text(l)

p=progress.read_text()
p=p.replace('object%20accounting-61.42%25-red','object%20accounting-62.52%25-red',1)
p=p.replace('**Verified translated instruction bytes:** **144,500**','**Verified translated instruction bytes:** **147,003**',1)
p=p.replace('**Proven non-code/data bytes:** **2,716**','**Proven non-code/data bytes:** **2,849**',1)
p=p.replace('**Reconciled object bytes:** **147,216 / 239,698 = 61.4173% (61.42%)**',
            '**Reconciled object bytes:** **149,852 / 239,698 = 62.5170% (62.52%)**',1)
p=p.replace('**Unresolved object bytes:** **92,482**','**Unresolved object bytes:** **89,846**',1)
checkpoint='''

## CODE.2 Batch 49 checkpoint

Batch 49 added **2,503 translated instruction bytes** plus **133 proven non-code/data bytes**. Cumulative accounting is **147,003 translated + 2,849 non-code = 149,852 / 239,698 = 62.5170% (62.52%)**; unresolved is **89,846 bytes**.

The sequential frontier now reaches `22E3F`; the next routine begins at `22E40`. `22950..229A3` and `229A4..229CB` are the exact CS tables owned by `229CC`. Preserve `223F4`'s AL/CL/DL-only page toggle and its one-past-count record pointer when an ordinal is not found, `229CC`'s persistent `EBX -= 20h` mutation after a failed `223F4` probe, `22894`'s signed `IDIV` quotient/remainder and truncation-toward-zero scale arithmetic, and the queued-input ring's head advance before dispatch. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.'''
assert '## CODE.2 Batch 49 checkpoint' not in p
p=p.rstrip()+checkpoint+'\n'
progress.write_text(p)

a=addendum.read_text()
assert '## CODE.2 Batch 49 checkpoint' not in a
a=a.rstrip()+'''\n\n## CODE.2 Batch 49 checkpoint\nBatch 49 adds **2,503 translated bytes** and **133 proven non-code/data bytes**, bringing cumulative accounting to **147,003 translated + 2,849 non-code = 149,852 / 239,698 = 62.5170% (62.52%)**. The sequential frontier is now `22E40`. Preserve the partial-register page toggle and one-past-count ownership in `223F4`, both exact `229CC` CS tables, the failed-probe `EBX -= 20h` carry-forward, `22894`'s signed division behavior, and the queue head update before `229CC`/`12E70`/`02DB0` dispatch.\n'''
addendum.write_text(a)
