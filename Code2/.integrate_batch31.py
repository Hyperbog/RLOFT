from pathlib import Path
import re,base64,zlib
root=Path('.')
code_path=root/'Code2/Code2.cpp'
ledger_path=root/'Code2/RESIDUAL_LEDGER.md'
progress_path=root/'Code2/PROGRESS.md'
add_path=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
frag=zlib.decompress(base64.b64decode((root/'Code2/.batch31_payload.txt').read_text())).decode().rstrip()+'\n\n'
code=code_path.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 99711u;' in code
assert 'constexpr u32 kVerifiedNonCodeBytes = 1945u;' in code
assert 'function_171fc' not in code
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 99711u;','constexpr u32 kVerifiedTranslatedBytes = 103537u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 1945u;','constexpr u32 kVerifiedNonCodeBytes = 2143u;',1)
start=code.index('static_assert(kVerifiedTranslatedBytes == 97832u +')
end=code.index('));',start)+3
new_assert='''static_assert(kVerifiedTranslatedBytes == 99711u +
              (0x17838u-0x171FCu) + (0x179A2u-0x17838u) +
              (0x181B4u-0x17A68u));'''
code=code[:start]+new_assert+code[end:]
close='} // namespace code2'
pos=code.rfind(close)
assert pos!=-1
code=code[:pos]+frag+code[pos:]
code_path.write_text(code)

ledger=ledger_path.read_text()
marker='<!-- RESIDUAL-RANGES-END -->'
assert marker in ledger
ranges='''179A2..179A3 | alignment | 2
179A4..17A67 | table | 196
'''
ledger=ledger.replace(marker,ranges+marker,1)
ledger=ledger.replace('**Total proven non-code/data:** **1,945 bytes**.','**Total proven non-code/data:** **2,143 bytes**.',1)
ledger += '''\n\nBatch 31 evidence: `179A2..179A3` decodes as the explicit `MOV EAX,EAX` compiler separator immediately after the `17838` return. `179A4..17A67` is the exact 49-dword CS jump table indexed by `(type-22h)` at `17C50`; all 49 entries were decoded and reconciled, including shared default `18191` and the `18121` type-`4Eh` mutation path. A full object-1 direct relative control-flow scan found no incoming call/branch target from outside this residual range. The next routine begins directly at `181B4`. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
ledger_path.write_text(ledger)

progress=progress_path.read_text()
progress=progress.replace('object%20accounting-42.41%25-red','object%20accounting-44.09%25-red',1)
progress=progress.replace('**Verified translated instruction bytes:** **99,711**','**Verified translated instruction bytes:** **103,537**',1)
progress=progress.replace('**Proven non-code/data bytes:** **1,945**','**Proven non-code/data bytes:** **2,143**',1)
progress=progress.replace('**Reconciled object bytes:** **101,656 / 239,698 = 42.4100% (42.41%)**','**Reconciled object bytes:** **105,680 / 239,698 = 44.0888% (44.09%)**',1)
progress=progress.replace('**Unresolved object bytes:** **138,042**','**Unresolved object bytes:** **134,018**',1)
progress_path.write_text(progress)

add=add_path.read_text().rstrip()+'''\n\n\n## CODE.2 Batch 31 checkpoint\n\nBatch 31 added **3,826 translated bytes** plus **198 proven non-code/data bytes**. Cumulative accounting is **103,537 translated + 2,143 non-code = 105,680 / 239,698 = 44.0888% (44.09%)**; unresolved is **134,018 bytes**.\n\nThe sequential frontier now reaches `181B3`; `179A2..179A3` is proven compiler alignment and `179A4..17A67` is the exact 49-entry CS table owned by `17A68`. The next routine begins at `181B4`. Preserve `171FC`'s signed-IDIV three-digit filename substitution, its delayed seven-dword section-offset header at file offset 1, and the distinct terminators used by the `CB3C` and `CD3C` dynamic record families; `17838`'s resource-`37h` dependent level exclusion sets and wrapped `(random*4)>>15` delay calculation; and `17A68`'s clear/recompute of record bit 1, pre-dispatch special types, exact 49-way `22h..52h` table, and type-`4Eh` mutation into type 1 with `10000000h` pointer and a rerolled word. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
add_path.write_text(add)
