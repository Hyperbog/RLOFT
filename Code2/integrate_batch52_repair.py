from pathlib import Path
import base64,zlib,re

root=Path('.')
cpp=root/'Code2/Code2.cpp'
ledger=root/'Code2/RESIDUAL_LEDGER.md'
progress=root/'Code2/PROGRESS.md'
addendum=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
payload=root/'Code2/batch52_repair.zb64'

# --- Code2.cpp: remove invalid shifted Batch 52, restore Batch 51, append corrected true Batch 52.
c=cpp.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 156694u;' in c
assert 'constexpr u32 kVerifiedNonCodeBytes = 2962u;' in c
assert '// Batch 52: repaired 24A60 boundary plus actor/resource selection' in c
assert 'function_24a60' in c and 'call_37c70' in c
c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 156694u;',
            'constexpr u32 kVerifiedTranslatedBytes = 159155u;',1)
c=c.replace('constexpr u32 kVerifiedNonCodeBytes = 2962u;',
            'constexpr u32 kVerifiedNonCodeBytes = 2917u;',1)

old51='''static_assert(kVerifiedTranslatedBytes == 154156u +
              (0x24AB2u-0x24A60u) + (0x24BDEu-0x24ADCu) +
              (0x24C3Du-0x24BE0u) + (0x24CA3u-0x24C40u) +
              (0x24E99u-0x24CA4u) + (0x24FBEu-0x24E9Cu) +
              (0x2504Du-0x24FC0u) + (0x25143u-0x25050u) +
              (0x25168u-0x25144u) + (0x2520Bu-0x25168u) +
              (0x2527Du-0x2520Cu) +
              (0x2531Fu-0x25280u) + (0x2538Bu-0x25320u) +
              (0x253F9u-0x2538Cu) + (0x2548Cu-0x253FCu));'''
# This is the bad Batch-52 aggregate at the global scalar site. We replace it
# with the correct batch-52 aggregate after restoring the batch-51 tail below.
newagg='''static_assert(kVerifiedTranslatedBytes == 154218u +
              (0x24B46u-0x24A9Cu) + (0x24BF1u-0x24B48u) +
              (0x24F5Eu-0x24BF4u) + (0x2519Fu-0x24F60u) +
              (0x253F6u-0x251A0u) + (0x254D3u-0x253F8u) +
              (0x256C4u-0x254D4u) + (0x25798u-0x256C4u) +
              (0x258F7u-0x25798u) + (0x25A0Du-0x258F8u) +
              (0x25ACAu-0x25A10u) + (0x25C36u-0x25ACCu) +
              (0x25CAAu-0x25C38u) + (0x25D24u-0x25CACu) +
              (0x25DFAu-0x25D24u));'''
assert old51 in c
c=c.replace(old51,newagg,1)

bad51='''static_assert((0x240A5u-0x23FC4u)+(0x243DEu-0x240A8u)+
              (0x24478u-0x243E0u)+(0x24A5Eu-0x24478u)==2709u);'''
good51='''static_assert((0x240A5u-0x23FC4u)+(0x243DEu-0x240A8u)+
              (0x24478u-0x243E0u)+(0x24A9Cu-0x24478u)==2771u);'''
assert bad51 in c
c=c.replace(bad51,good51,1)

marker='\n// -----------------------------------------------------------------------------\n// Batch 52: repaired 24A60 boundary plus actor/resource selection'
pos=c.find(marker)
assert pos!=-1
end='\n} // namespace code2\n'
assert c.endswith(end)
# Drop the entire invalid shifted Batch-52 source block.
c=c[:pos]+end
batch=zlib.decompress(base64.b64decode(payload.read_text())).decode().rstrip()+'\n'
c=c[:-len(end)]+'\n'+batch+end
assert 'function_24a60' not in c
assert 'call_37c70' not in c
assert 'function_24a9c' in c and 'function_25d24' in c
cpp.write_text(c)

# --- Residual ledger: remove invalid shifted classifications, add only true separators.
l=ledger.read_text()
bad_ranges='''24A5E..24A5F | alignment | 2
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
for line in bad_ranges.splitlines():
    assert line in l,line
    l=l.replace(line+'\n','',1)
# remove false evidence paragraph (currently last Batch-52 evidence paragraph)
needle='\nBatch 52 evidence and Batch-51 boundary repair:'
assert needle in l
l=l.split(needle,1)[0].rstrip()+'\n'
# The END marker remains earlier in the file; insert corrected ranges immediately before it.
ledger_marker='<!-- RESIDUAL-RANGES-END -->'
assert ledger_marker in l
ranges='''24B46..24B47 | alignment | 2
24BF1..24BF3 | alignment | 3
24F5E..24F5F | alignment | 2
2519F..2519F | alignment | 1
253F6..253F7 | alignment | 2
254D3..254D3 | alignment | 1
258F7..258F7 | alignment | 1
25A0D..25A0F | alignment | 3
25ACA..25ACB | alignment | 2
25C36..25C37 | alignment | 2
25CAA..25CAB | alignment | 2
25DFA..25DFB | alignment | 2
'''
for line in ranges.splitlines():
    assert line not in l,line
l=l.replace(ledger_marker,ranges+ledger_marker,1)
l=l.replace('**Total proven non-code/data:** **2,962 bytes**.',
            '**Total proven non-code/data:** **2,917 bytes**.',1)
evidence='''

Batch 52 integrity-repair evidence: the prior Batch 52 mistakenly analyzed object offsets `14A60..1548B` while labeling them `24A60..2548B`; fresh disassembly of immutable CODE.2 object 1 confirms that `24478` continues through its RET at `24A9B` and `24A9C` is the true next entry. The invalid shifted source and all sixteen associated residual-ledger classifications have been removed. `24B46..24B47`, `24BF1..24BF3`, `24F5E..24F5F`, `2519F`, `253F6..253F7`, `254D3`, `258F7`, `25A0D..25A0F`, `25ACA..25ACB`, `25C36..25C37`, `25CAA..25CAB`, and `25DFA..25DFB` decode as explicit Watcom NOP/LEA/MOV-EAX,EAX separators between directly reconstructed routines. A full object-1 direct relative CALL/JMP/Jcc target scan found no incoming target to any of these separator bytes. The object at `25DFC` begins a CS dword dispatch table and remains unresolved with its owning dispatcher for the next fold. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.'''
l=l.rstrip()+evidence+'\n'
ledger.write_text(l)

# --- Progress: restore valid Batch 51 and replace false Batch 52 checkpoint.
p=progress.read_text()
p=p.replace('object%20accounting-66.61%25-red','object%20accounting-67.62%25-red',1)
p=p.replace('**Verified translated instruction bytes:** **156,694**','**Verified translated instruction bytes:** **159,155**',1)
p=p.replace('**Proven non-code/data bytes:** **2,962**','**Proven non-code/data bytes:** **2,917**',1)
p=p.replace('**Reconciled object bytes:** **159,656 / 239,698 = 66.6071% (66.61%)**',
            '**Reconciled object bytes:** **162,072 / 239,698 = 67.6151% (67.62%)**',1)
p=p.replace('**Unresolved object bytes:** **80,042**','**Unresolved object bytes:** **77,626**',1)
head='## CODE.2 Batch 51 checkpoint'
assert head in p
p=p.split(head,1)[0].rstrip()+'''\n\n## CODE.2 Batch 51 checkpoint\n\nBatch 51 added **2,771 translated instruction bytes** plus **5 proven alignment bytes**. Cumulative accounting at that checkpoint is **154,218 translated + 2,894 non-code = 157,112 / 239,698 = 65.5458% (65.55%)**; unresolved was **82,586 bytes**.\n\nThe sequential frontier reaches `24A9B`; the next routine begins at `24A9C`. Preserve `240A8`'s role-dependent admissibility and default-one behavior, `243E0`'s empty-slot global gate, and `24478`'s asymmetric class/rank arbitration, repeated actor-role predicates, low-two-bit item class tests, per-side cooldown dword gates, and selector-0/1 return ownership. Known zero islands remain unresolved. Semantic audit remains not started.\n\n## CODE.2 Batch 52 integrity repair\n\nThe previously landed Batch 52 is invalid and has been fully removed: it disassembled raw object offsets beginning at `14A60` but labeled them with an erroneous `+10000h` shift beginning at `24A60`. Fresh immutable CODE.2 disassembly confirms `24478` continues through its RET at `24A9B`; `24A9C` is the true continuation. Batch 51's original boundary and accounting are restored before the corrected Batch 52 is counted.\n\n## CODE.2 Batch 52 checkpoint\n\nCorrected Batch 52 adds **4,937 translated instruction bytes** plus **23 proven alignment bytes**. Cumulative accounting is **159,155 translated + 2,917 non-code = 162,072 / 239,698 = 67.6151% (67.62%)**; unresolved is **77,626 bytes**.\n\nThe sequential frontier now reaches `25DFB`; the next object begins at `25DFC` with a CS dword dispatch table retained with its owning dispatcher. Preserve `24A9C`/`24B48` panel-slot arithmetic and cooldown dword ownership, `24BF4`'s signed clipping/alignment sequence, `24CC8`'s byte-only `C050` page toggle and capture lifecycle, `24F60`/`251A0`'s resource/display restoration ordering, `253F8`/`254D4` inventory-group ownership, `256C4`'s 22-byte record compaction, `25798`/`258F8`/`25A10` resource gates, and `25D24`'s low-word add-and-clamp behavior. Known zero islands remain unresolved. Semantic audit remains not started.\n'''
progress.write_text(p)

# --- Durable addendum: same repair checkpoint in compact form.
a=addendum.read_text()
assert head in a
a=a.split(head,1)[0].rstrip()+'''\n\n## CODE.2 Batch 51 checkpoint\nBatch 51 adds **2,771 translated bytes** and **5 proven alignment bytes**, bringing cumulative accounting to **154,218 translated + 2,894 non-code = 157,112 / 239,698 = 65.5458% (65.55%)**. The sequential frontier is `24A9C`. Preserve `240A8` role-dependent admissibility, `243E0` empty-slot gating, and `24478` asymmetric class/rank arbitration plus per-side cooldown and selector-0/1 return ownership.\n\n## CODE.2 Batch 52 integrity repair\nThe prior Batch 52 was removed because it analyzed object offsets beginning at `14A60` while labeling them with an erroneous `+10000h` shift. Immutable CODE.2 proves `24478` continues through `24A9B` and `24A9C` is the true continuation; Batch 51's original accounting is restored.\n\n## CODE.2 Batch 52 checkpoint\nCorrected Batch 52 adds **4,937 translated bytes** and **23 proven alignment bytes**, bringing cumulative accounting to **159,155 translated + 2,917 non-code = 162,072 / 239,698 = 67.6151% (67.62%)**. The sequential frontier is now `25DFC`, whose CS dword dispatch table remains with its owning dispatcher for the next fold. Preserve the signed clipping/page-capture behavior, byte-only page toggles, display/resource restoration order, inventory-group ownership, record compaction, resource gates, and low-word add/clamp behavior described in `PROGRESS.md`.\n'''
addendum.write_text(a)
