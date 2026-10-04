from pathlib import Path
import base64,zlib

root=Path('.')
cpp=root/'Code2/Code2.cpp'
ledger=root/'Code2/RESIDUAL_LEDGER.md'
progress=root/'Code2/PROGRESS.md'
addendum=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
payload=root/'Code2/batch50.zb64'

c=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 147003u;' in c
assert 'constexpr u32 kVerifiedNonCodeBytes = 2849u;' in c
c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 147003u;',
            'constexpr u32 kVerifiedTranslatedBytes = 151447u;',1)
c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 2849u;',
            'constexpr u32 kVerifiedNonCodeBytes = 2889u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 144500u +
              (0x22891u-0x223F4u) + (0x2294Du-0x22894u) +
              (0x22CE0u-0x229CCu) + (0x22D70u-0x22CE0u) +
              (0x22D8Bu-0x22D70u) + (0x22E3Eu-0x22D8Cu));'''
new='''static_assert(kVerifiedTranslatedBytes == 147003u +
              (0x22E8Du-0x22E40u) + (0x22F40u-0x22E90u) +
              (0x23021u-0x22F40u) + (0x231BEu-0x23024u) +
              (0x233A5u-0x231C0u) + (0x23435u-0x233A8u) +
              (0x237E3u-0x23438u) + (0x2388Cu-0x237E4u) +
              (0x23B35u-0x2388Cu) + (0x23D84u-0x23B38u) +
              (0x23DE5u-0x23D84u) + (0x23E1Bu-0x23DF0u) +
              (0x23E56u-0x23E1Cu) + (0x23EAAu-0x23E58u) +
              (0x23EE4u-0x23EACu) + (0x23F33u-0x23EE4u) +
              (0x23F4Eu-0x23F34u) + (0x23FC1u-0x23F50u));'''
assert old in c
c=c.replace(old,new,1)
end='\n} // namespace code2\n'
assert c.endswith(end)
batch=zlib.decompress(base64.b64decode(payload.read_text())).decode().rstrip()+'\n'
c=c[:-len(end)]+'\n'+batch+end
cpp.write_text(c)

l=ledger.read_text()
marker='<!-- RESIDUAL-RANGES-END -->'
assert marker in l
ranges='''22E8D..22E8F | alignment | 3
23021..23023 | alignment | 3
231BE..231BF | alignment | 2
233A5..233A7 | alignment | 3
23435..23437 | alignment | 3
237E3..237E3 | alignment | 1
23B35..23B37 | alignment | 3
23DE5..23DEF | padding | 11
23E1B..23E1B | alignment | 1
23E56..23E57 | alignment | 2
23EAA..23EAB | alignment | 2
23F33..23F33 | alignment | 1
23F4E..23F4F | alignment | 2
23FC1..23FC3 | alignment | 3
'''
assert '22E8D..22E8F | alignment | 3' not in l
l=l.replace(marker,ranges+marker,1)
l=l.replace('**Total proven non-code/data:** **2,849 bytes**.',
            '**Total proven non-code/data:** **2,889 bytes**.',1)
evidence='''

Batch 50 evidence: `22E8D..22E8F`, `23021..23023`, `231BE..231BF`, `233A5..233A7`, `23435..23437`, `237E3`, `23B35..23B37`, `23E1B`, `23E56..23E57`, `23EAA..23EAB`, `23F33`, `23F4E..23F4F`, and `23FC1..23FC3` decode as explicit Watcom NOP/LEA/MOV-EAX,EAX separators between directly reconstructed routine boundaries. `23DE5..23DEF` is the exact eleven-byte zero pad between the `23D84` return at `23DE4` and the independently decoded `23DF0` entry; a full object-1 direct relative CALL/JMP/Jcc target scan found no incoming target to any Batch 50 residual byte, and a full little-endian absolute-address scan found no direct reference to any byte of the zero pad. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.'''
l=l.rstrip()+evidence+'\n'
ledger.write_text(l)

p=progress.read_text()
p=p.replace('object%20accounting-62.52%25-red','object%20accounting-64.39%25-red',1)
p=p.replace('**Verified translated instruction bytes:** **147,003**','**Verified translated instruction bytes:** **151,447**',1)
p=p.replace('**Proven non-code/data bytes:** **2,849**','**Proven non-code/data bytes:** **2,889**',1)
p=p.replace('**Reconciled object bytes:** **149,852 / 239,698 = 62.5170% (62.52%)**',
            '**Reconciled object bytes:** **154,336 / 239,698 = 64.3877% (64.39%)**',1)
p=p.replace('**Unresolved object bytes:** **89,846**','**Unresolved object bytes:** **85,362**',1)
checkpoint='''

## CODE.2 Batch 50 checkpoint

Batch 50 added **4,444 translated instruction bytes** plus **40 proven non-code/data bytes**. Cumulative accounting is **151,447 translated + 2,889 non-code = 154,336 / 239,698 = 64.3877% (64.39%)**; unresolved is **85,362 bytes**.

The sequential frontier now reaches `23FC3`; the next routine begins at `23FC4`. Preserve `23024`'s equality-only six-row termination (not an `>=` precheck), `231C0`'s two packed-nibble list passes and DEC/JNE ownership, `23438`'s signed panel arithmetic plus high-nibble wrap decrement, `2388C`'s signed quotient/remainder angle path and record rollback, and `23B38`'s stale-record read/modify/write flag behavior plus collision retry loop. `23DE5..23DEF` is proven padding; the older zero islands remain unresolved. Semantic audit remains not started.'''
assert '## CODE.2 Batch 50 checkpoint' not in p
p=p.rstrip()+checkpoint+'\n'
progress.write_text(p)

a=addendum.read_text()
assert '## CODE.2 Batch 50 checkpoint' not in a
a=a.rstrip()+'''\n\n## CODE.2 Batch 50 checkpoint\nBatch 50 adds **4,444 translated bytes** and **40 proven non-code/data bytes**, bringing cumulative accounting to **151,447 translated + 2,889 non-code = 154,336 / 239,698 = 64.3877% (64.39%)**. The sequential frontier is now `23FC4`. Preserve `23024`'s equality-only list stop, `231C0`'s packed-nibble DEC/JNE loops, `23438`'s signed panel arithmetic and nibble wrap decrement, `2388C`'s rollback/angle/timer paths, and `23B38`'s partial flag writes and collision retry loop.\n'''
addendum.write_text(a)
