from pathlib import Path
import base64,zlib

root=Path('.')
cpp=root/'Code2/Code2.cpp'
ledger=root/'Code2/RESIDUAL_LEDGER.md'
progress=root/'Code2/PROGRESS.md'
addendum=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
payload=root/'Code2/batch52.zb64'

c=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 154218u;' in c
assert 'constexpr u32 kVerifiedNonCodeBytes = 2894u;' in c
c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 154218u;',
            'constexpr u32 kVerifiedTranslatedBytes = 156694u;',1)
c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 2894u;',
            'constexpr u32 kVerifiedNonCodeBytes = 2962u;',1)
old_main='''static_assert(kVerifiedTranslatedBytes == 151447u +
              (0x240A5u-0x23FC4u) +
              (0x243DEu-0x240A8u) +
              (0x24478u-0x243E0u) +
              (0x24A9Cu-0x24478u));'''
new_main='''static_assert(kVerifiedTranslatedBytes == 154156u +
              (0x24AB2u-0x24A60u) + (0x24BDEu-0x24ADCu) +
              (0x24C3Du-0x24BE0u) + (0x24CA3u-0x24C40u) +
              (0x24E99u-0x24CA4u) + (0x24FBEu-0x24E9Cu) +
              (0x2504Du-0x24FC0u) + (0x25143u-0x25050u) +
              (0x25168u-0x25144u) + (0x2520Bu-0x25168u) +
              (0x2527Du-0x2520Cu) + (0x2531Fu-0x25280u) +
              (0x2538Bu-0x25320u) + (0x253F9u-0x2538Cu) +
              (0x2548Cu-0x253FCu));'''
assert old_main in c
c=c.replace(old_main,new_main,1)

old_comment='''// Batch 51: 23FC4..24A9B item admissibility, actor/slot compatibility, and
// paired-hand arbitration. Watcom alignment at 240A5..240A7 and 243DE..243DF
// is ledgered separately. The next routine begins at 24A9C.'''
new_comment='''// Batch 51: 23FC4..24A5D item admissibility, actor/slot compatibility, and
// paired-hand arbitration. Watcom alignment at 240A5..240A7, 243DE..243DF,
// and 24A5E..24A5F is ledgered separately. The next routine begins at 24A60.
// Batch 52 repairs the prior overextended 24478 byte scalar; semantics were not
// changed, only its true instruction boundary and accounting.'''
assert old_comment in c
c=c.replace(old_comment,new_comment,1)
assert 'static_assert(0x24A9Cu-0x24478u==1572u);' in c
c=c.replace('static_assert(0x24A9Cu-0x24478u==1572u);',
            'static_assert(0x24A5Eu-0x24478u==1510u);',1)
old_sum='''static_assert((0x240A5u-0x23FC4u)+(0x243DEu-0x240A8u)+
              (0x24478u-0x243E0u)+(0x24A9Cu-0x24478u)==2771u);'''
new_sum='''static_assert((0x240A5u-0x23FC4u)+(0x243DEu-0x240A8u)+
              (0x24478u-0x243E0u)+(0x24A5Eu-0x24478u)==2709u);'''
assert old_sum in c
c=c.replace(old_sum,new_sum,1)

end='\n} // namespace code2\n'
assert c.endswith(end)
batch=zlib.decompress(base64.b64decode(payload.read_text())).decode().rstrip()+'\n'
c=c[:-len(end)]+'\n'+batch+end
cpp.write_text(c)

l=ledger.read_text()
marker='<!-- RESIDUAL-RANGES-END -->'
assert marker in l
ranges='''24A5E..24A5F | alignment | 2
24AB2..24AB3 | alignment | 2
24AB4..24ADB | table | 40
24BDE..24BDF | alignment | 2
24C3D..24C3F | alignment | 3
24CA3..24CA3 | alignment | 1
24E99..24E9B | alignment | 3
24FBE..24FBF | alignment | 2
2504D..2504F | alignment | 3
25143..25143 | alignment | 1
2520B..2520B | alignment | 1
2527D..2527F | alignment | 3
2531F..2531F | alignment | 1
2538B..2538B | alignment | 1
253F9..253FB | alignment | 3
'''
assert '24A5E..24A5F | alignment | 2' not in l
l=l.replace(marker,ranges+marker,1)
l=l.replace('**Total proven non-code/data:** **2,894 bytes**.',
            '**Total proven non-code/data:** **2,962 bytes**.',1)
evidence='''

Batch 52 evidence and Batch-51 boundary repair: direct disassembly shows `24478` returns at `24A5D`; `24A5E..24A5F` is the following `MOV EAX,EAX` separator and `24A60` is an independent callable routine whose loop tail continues through the RET at `24AB1`. The previous Batch-51 scalar therefore overreached by 62 bytes; 60 return-path bytes are restored to instruction ownership here and two bytes are reclassified as alignment. `24AB4..24ADB` is the exact ten-entry CS dword table consumed by `JMP DWORD PTR CS:[ESI*4+14AB4h]` at `24B71`; stored offsets are `14B79,14B87,14B95,14BA3,14BDA,14BDA,14BB1,14BDA,14BDA,14BBF`, mapping to object addresses `24B79,24B87,24B95,24BA3,24BDA,24BDA,24BB1,24BDA,24BDA,24BBF`. `24AB2..24AB3`, `24BDE..24BDF`, `24C3D..24C3F`, `24CA3`, `24E99..24E9B`, `24FBE..24FBF`, `2504D..2504F`, `25143`, `2520B`, `2527D..2527F`, `2531F`, `2538B`, and `253F9..253FB` decode as explicit Watcom NOP/LEA/MOV-EAX,EAX separators. A full object-1 direct relative CALL/JMP/Jcc target scan found no incoming target to any separator byte. The 42-entry table beginning at `2548C` is deliberately left unresolved with its owning `25534` dispatcher for the next fold. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.'''
l=l.rstrip()+evidence+'\n'
ledger.write_text(l)

p=progress.read_text()
p=p.replace('object%20accounting-65.55%25-red','object%20accounting-66.61%25-red',1)
p=p.replace('**Verified translated instruction bytes:** **154,218**','**Verified translated instruction bytes:** **156,694**',1)
p=p.replace('**Proven non-code/data bytes:** **2,894**','**Proven non-code/data bytes:** **2,962**',1)
p=p.replace('**Reconciled object bytes:** **157,112 / 239,698 = 65.5458% (65.55%)**',
            '**Reconciled object bytes:** **159,656 / 239,698 = 66.6071% (66.61%)**',1)
p=p.replace('**Unresolved object bytes:** **82,586**','**Unresolved object bytes:** **80,042**',1)
old51='''## CODE.2 Batch 51 checkpoint

Batch 51 added **2,771 translated instruction bytes** plus **5 proven alignment bytes**. Cumulative accounting is **154,218 translated + 2,894 non-code = 157,112 / 239,698 = 65.5458% (65.55%)**; unresolved is **82,586 bytes**.

The sequential frontier now reaches `24A9B`; the next routine begins at `24A9C`. Preserve `240A8`'s role-dependent admissibility and default-one behavior, `243E0`'s empty-slot global gate, and `24478`'s asymmetric class/rank arbitration, repeated actor-role predicates, low-two-bit item class tests, per-side cooldown dword gates, and selector-0/1 return ownership. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.'''
new51='''## CODE.2 Batch 51 checkpoint

Batch 51 is corrected to **2,709 translated instruction bytes** plus **7 proven alignment bytes**. Its corrected cumulative accounting is **154,156 translated + 2,896 non-code = 157,052 / 239,698 = 65.5208% (65.52%)**; unresolved at that checkpoint is **82,646 bytes**.

The corrected sequential frontier reaches `24A5F`; the next routine begins at `24A60`. The repair follows direct control flow: `24478` returns at `24A5D`, `24A5E..24A5F` is alignment, and `24A60` is a separate routine. Preserve `240A8`'s role-dependent admissibility and default-one behavior, `243E0`'s empty-slot global gate, and `24478`'s asymmetric class/rank arbitration, repeated actor-role predicates, low-two-bit item class tests, per-side cooldown dword gates, and selector-0/1 return ownership. Known zero islands remain unresolved. Semantic audit remains not started.'''
assert old51 in p
p=p.replace(old51,new51,1)
checkpoint='''

## CODE.2 Batch 52 checkpoint

Batch 52 adds **2,538 translated instruction bytes** plus **66 proven table/alignment bytes** after the Batch-51 boundary repair. Cumulative accounting is **156,694 translated + 2,962 non-code = 159,656 / 239,698 = 66.6071% (66.61%)**; unresolved is **80,042 bytes**.

The sequential frontier now reaches `2548B`; the next object is the **42-entry CS dispatch table at `2548C`**, retained with its owning `25534` dispatcher. Preserve `24A60`'s independent actor search and its tail through `24AB1`, the exact ten-entry `24AB4` table, `24CA4`'s release-then-press selection loop and AL-only page toggle, `24E9C`'s bit extraction using the encoded shift count, `25168`'s cursor increment/read ordering, `25280`'s signed live record-count reloads, and recursive `2538C` counting. Known zero islands remain unresolved. Semantic audit remains not started.'''
assert '## CODE.2 Batch 52 checkpoint' not in p
p=p.rstrip()+checkpoint+'\n'
progress.write_text(p)

a=addendum.read_text()
old51a='''## CODE.2 Batch 51 checkpoint
Batch 51 adds **2,771 translated bytes** and **5 proven alignment bytes**, bringing cumulative accounting to **154,218 translated + 2,894 non-code = 157,112 / 239,698 = 65.5458% (65.55%)**. The sequential frontier is now `24A9C`. Preserve `240A8` role-dependent admissibility, `243E0` empty-slot gating, and `24478` asymmetric class/rank arbitration plus per-side cooldown and selector-0/1 return ownership.'''
new51a='''## CODE.2 Batch 51 checkpoint
Batch 51 is corrected to **2,709 translated bytes** and **7 proven alignment bytes**, giving corrected cumulative accounting of **154,156 translated + 2,896 non-code = 157,052 / 239,698 = 65.5208% (65.52%)**. The corrected frontier is `24A60`: `24478` returns at `24A5D`, `24A5E..24A5F` is alignment, and `24A60` is a separate callable routine. Preserve `240A8` role-dependent admissibility, `243E0` empty-slot gating, and `24478` asymmetric class/rank arbitration plus per-side cooldown and selector-0/1 return ownership.'''
assert old51a in a
a=a.replace(old51a,new51a,1)
assert '## CODE.2 Batch 52 checkpoint' not in a
a=a.rstrip()+'''\n\n## CODE.2 Batch 52 checkpoint\nBatch 52 adds **2,538 translated bytes** and **66 proven table/alignment bytes** after repairing the Batch-51 boundary, bringing cumulative accounting to **156,694 translated + 2,962 non-code = 159,656 / 239,698 = 66.6071% (66.61%)**. The sequential frontier is now `2548C`; the exact 42-entry table beginning there remains with its owning `25534` dispatcher. Preserve `24A60`'s tail through `24AB1`, the exact ten-entry `24AB4` table, `24CA4` release/press input ownership, `24E9C` bit-shift behavior, `25168` cursor ordering, and recursive `2538C`.\n'''
addendum.write_text(a)
