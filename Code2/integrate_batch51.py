from pathlib import Path
import base64,zlib

root=Path('.')
cpp=root/'Code2/Code2.cpp'
ledger=root/'Code2/RESIDUAL_LEDGER.md'
progress=root/'Code2/PROGRESS.md'
addendum=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
payload=root/'Code2/batch51.zb64'

c=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 151447u;' in c
assert 'constexpr u32 kVerifiedNonCodeBytes = 2889u;' in c
c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 151447u;',
            'constexpr u32 kVerifiedTranslatedBytes = 154218u;',1)
c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 2889u;',
            'constexpr u32 kVerifiedNonCodeBytes = 2894u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 147003u +
              (0x22E8Du-0x22E40u) + (0x22F40u-0x22E90u) +
              (0x23021u-0x22F40u) + (0x231BEu-0x23024u) +
              (0x233A5u-0x231C0u) + (0x23435u-0x233A8u) +
              (0x237E3u-0x23438u) + (0x2388Cu-0x237E4u) +
              (0x23B35u-0x2388Cu) + (0x23D84u-0x23B38u) +
              (0x23DE5u-0x23D84u) + (0x23E1Bu-0x23DF0u) +
              (0x23E56u-0x23E1Cu) + (0x23EAAu-0x23E58u) +
              (0x23EE4u-0x23EACu) + (0x23F33u-0x23EE4u) +
              (0x23F4Eu-0x23F34u) + (0x23FC1u-0x23F50u));'''
new='''static_assert(kVerifiedTranslatedBytes == 151447u +
              (0x240A5u-0x23FC4u) +
              (0x243DEu-0x240A8u) +
              (0x24478u-0x243E0u) +
              (0x24A9Cu-0x24478u));'''
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
ranges='''240A5..240A7 | alignment | 3
243DE..243DF | alignment | 2
'''
assert '240A5..240A7 | alignment | 3' not in l
l=l.replace(marker,ranges+marker,1)
l=l.replace('**Total proven non-code/data:** **2,889 bytes**.',
            '**Total proven non-code/data:** **2,894 bytes**.',1)
evidence='''

Batch 51 evidence: `240A5..240A7` decodes as `LEA EAX,[EAX+0]` between the directly reconstructed `24060` return and the `240A8` entry; `243DE..243DF` decodes as `MOV EAX,EAX` between the `240A8` return and the `243E0` entry. A full object-1 direct relative CALL/JMP/Jcc target scan found no incoming target to either separator. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.'''
l=l.rstrip()+evidence+'\n'
ledger.write_text(l)

p=progress.read_text()
p=p.replace('object%20accounting-64.39%25-red','object%20accounting-65.55%25-red',1)
p=p.replace('**Verified translated instruction bytes:** **151,447**','**Verified translated instruction bytes:** **154,218**',1)
p=p.replace('**Proven non-code/data bytes:** **2,889**','**Proven non-code/data bytes:** **2,894**',1)
p=p.replace('**Reconciled object bytes:** **154,336 / 239,698 = 64.3877% (64.39%)**',
            '**Reconciled object bytes:** **157,112 / 239,698 = 65.5458% (65.55%)**',1)
p=p.replace('**Unresolved object bytes:** **85,362**','**Unresolved object bytes:** **82,586**',1)
checkpoint='''

## CODE.2 Batch 51 checkpoint

Batch 51 added **2,771 translated instruction bytes** plus **5 proven alignment bytes**. Cumulative accounting is **154,218 translated + 2,894 non-code = 157,112 / 239,698 = 65.5458% (65.55%)**; unresolved is **82,586 bytes**.

The sequential frontier now reaches `24A9B`; the next routine begins at `24A9C`. Preserve `240A8`'s role-dependent admissibility and default-one behavior, `243E0`'s empty-slot global gate, and `24478`'s asymmetric class/rank arbitration, repeated actor-role predicates, low-two-bit item class tests, per-side cooldown dword gates, and selector-0/1 return ownership. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.'''
assert '## CODE.2 Batch 51 checkpoint' not in p
p=p.rstrip()+checkpoint+'\n'
progress.write_text(p)

a=addendum.read_text()
assert '## CODE.2 Batch 51 checkpoint' not in a
a=a.rstrip()+'''\n\n## CODE.2 Batch 51 checkpoint\nBatch 51 adds **2,771 translated bytes** and **5 proven alignment bytes**, bringing cumulative accounting to **154,218 translated + 2,894 non-code = 157,112 / 239,698 = 65.5458% (65.55%)**. The sequential frontier is now `24A9C`. Preserve `240A8` role-dependent admissibility, `243E0` empty-slot gating, and `24478` asymmetric class/rank arbitration plus per-side cooldown and selector-0/1 return ownership.\n'''
addendum.write_text(a)
