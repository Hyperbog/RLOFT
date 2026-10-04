from pathlib import Path
import re

root=Path('Code2')
code_p=root/'Code2.cpp'
frag_p=root/'.batch31_fragment.cpp'
ledger_p=root/'RESIDUAL_LEDGER.md'
progress_p=root/'PROGRESS.md'
add_p=root/'CODE2_CONTINUITY_ADDENDUM.md'

code=code_p.read_text()
frag=frag_p.read_text().rstrip()+"\n\n"
assert 'Batch 31:' not in code
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 99711u;','constexpr u32 kVerifiedTranslatedBytes = 103537u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 1945u;','constexpr u32 kVerifiedNonCodeBytes = 2143u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 97832u +
              (0x16ADCu-0x16A88u) + (0x16CCDu-0x16ADCu) +
              (0x16D03u-0x16CD0u) + (0x17097u-0x16D04u) +
              (0x171CFu-0x170ACu) + (0x171F9u-0x171D0u));'''
new='''static_assert(kVerifiedTranslatedBytes == 99711u +
              (0x17838u-0x171FCu) + (0x179A2u-0x17838u) +
              (0x181B4u-0x17A68u));'''
assert old in code
code=code.replace(old,new,1)
marker='} // namespace code2'
pos=code.rfind(marker)
assert pos>=0
code=code[:pos]+frag+code[pos:]
code_p.write_text(code)

ledger=ledger_p.read_text()
assert '179A2..179A3' not in ledger
insert='179A2..179A3 | alignment | 2\n179A4..17A67 | table | 196\n'
ledger=ledger.replace('<!-- RESIDUAL-RANGES-END -->',insert+'<!-- RESIDUAL-RANGES-END -->',1)
ledger=ledger.replace('**Total proven non-code/data:** **1,945 bytes**.','**Total proven non-code/data:** **2,143 bytes**.',1)
ledger += ('\n\nBatch 31 evidence: `179A2..179A3` decodes as `MOV EAX,EAX` between the directly '
           'reconstructed `17838` return and the following dispatch table; it has no direct incoming '
           'relative control-flow target. `179A4..17A67` is the exact 49-dword CS jump table indexed '
           'by `(type-22h)` at `17C50`; all 49 entries were decoded and reconciled against the '
           '`17A68..181B3` owner. Known zero islands `006B9..006BF`, `0380D..0380F`, and '
           '`03A64..03A77` remain unresolved.\n')
ledger_p.write_text(ledger)

progress=progress_p.read_text()
progress=progress.replace('object%20accounting-42.41%25-red','object%20accounting-44.09%25-red',1)
progress=progress.replace('**Verified translated instruction bytes:** **99,711**','**Verified translated instruction bytes:** **103,537**',1)
progress=progress.replace('**Proven non-code/data bytes:** **1,945**','**Proven non-code/data bytes:** **2,143**',1)
progress=progress.replace('**Reconciled object bytes:** **101,656 / 239,698 = 42.4100% (42.41%)**','**Reconciled object bytes:** **105,680 / 239,698 = 44.0888% (44.09%)**',1)
progress=progress.replace('**Unresolved object bytes:** **138,042**','**Unresolved object bytes:** **134,018**',1)
progress_p.write_text(progress)

add=add_p.read_text().rstrip()+'''\n\n\n## CODE.2 Batch 31 checkpoint\n\nBatch 31 added **3,826 translated bytes** plus **198 proven non-code/data bytes**. Cumulative accounting is **103,537 translated + 2,143 non-code = 105,680 / 239,698 = 44.0888% (44.09%)**; unresolved is **134,018 bytes**.\n\nThe sequential frontier now reaches `181B3`; the next routine begins at `181B4`. Preserve `171FC`'s seven-dword section-offset header rewrite, signed `25619/2561B/258A2/258A4/258A6` count widths, the distinct `CB3C` terminator test (byte zero plus bit-10h clear) versus the `CD3C` byte-zero terminator, and the packed-text length emitted before `5760`. Preserve `17838`'s two different area-block lists depending on resource `37h`, the low-byte `40h` clear/set, and the unsigned `(random*4)>>15` scale. `17A68` owns the exact 49-entry table at `179A4..17A67`; preserve its initial `2ACDC` plus signed word-`+12` test, the high-word `CF4C` phase window, all resource predicates, and type `4Eh`'s mutation to type 1 with `10000000h` field plus `1C328(rank,8,0,0)` reroll. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
add_p.write_text(add)
