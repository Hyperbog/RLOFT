from pathlib import Path
import base64, gzip, re

root=Path('Code2')
code_p=root/'Code2.cpp'
payload_p=root/'.batch32_fragment.cpp.gz.b64'
ledger_p=root/'RESIDUAL_LEDGER.md'
progress_p=root/'PROGRESS.md'
add_p=root/'CODE2_CONTINUITY_ADDENDUM.md'

code=code_p.read_text()
assert 'Batch 32:' not in code
frag=gzip.decompress(base64.b64decode(payload_p.read_text().strip())).decode().rstrip()+"\n\n"
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 103537u;','constexpr u32 kVerifiedTranslatedBytes = 106569u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 2143u;','constexpr u32 kVerifiedNonCodeBytes = 2147u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 99711u +
              (0x17838u-0x171FCu) + (0x179A2u-0x17838u) +
              (0x181B4u-0x17A68u));'''
new='''static_assert(kVerifiedTranslatedBytes == 103537u +
              (0x18D8Cu-0x181B4u));'''
assert old in code
code=code.replace(old,new,1)
marker='} // namespace code2'
pos=code.rfind(marker)
assert pos>=0
code=code[:pos]+frag+code[pos:]
code_p.write_text(code)

ledger=ledger_p.read_text()
assert '18D8C..18D8F' not in ledger
ledger=ledger.replace('<!-- RESIDUAL-RANGES-END -->','18D8C..18D8F | noncode | 4\n<!-- RESIDUAL-RANGES-END -->',1)
ledger=ledger.replace('**Total proven non-code/data:** **2,143 bytes**.','**Total proven non-code/data:** **2,147 bytes**.',1)
ledger += ('\n\nBatch 32 evidence: `18D8C..18D8F` is the four-byte zero-filled gap after the '
           'directly reconstructed `181B4..18D8B` loader return and before the independently decoded '
           '`18D90` entry. A full object-1 direct relative CALL/JMP/conditional-branch target scan found '
           'no incoming target to any byte in the gap. Known zero islands `006B9..006BF`, '
           '`0380D..0380F`, and `03A64..03A77` remain unresolved.\n')
ledger_p.write_text(ledger)

progress=progress_p.read_text()
progress=progress.replace('object%20accounting-44.09%25-red','object%20accounting-45.36%25-red',1)
progress=progress.replace('**Verified translated instruction bytes:** **103,537**','**Verified translated instruction bytes:** **106,569**',1)
progress=progress.replace('**Proven non-code/data bytes:** **2,143**','**Proven non-code/data bytes:** **2,147**',1)
progress=progress.replace('**Reconciled object bytes:** **105,680 / 239,698 = 44.0888% (44.09%)**','**Reconciled object bytes:** **108,716 / 239,698 = 45.3554% (45.36%)**',1)
progress=progress.replace('**Unresolved object bytes:** **134,018**','**Unresolved object bytes:** **130,982**',1)
progress_p.write_text(progress)

add=add_p.read_text().rstrip()+'''\n\n\n## CODE.2 Batch 32 checkpoint\n\nBatch 32 added **3,032 translated bytes** plus **4 proven non-code/data bytes**. Cumulative accounting is **106,569 translated + 2,147 non-code = 108,716 / 239,698 = 45.3554% (45.36%)**; unresolved is **130,982 bytes**.\n\nThe sequential frontier now reaches `18D8F`; `18D8C..18D8F` is proven zero padding and the next routine begins at `18D90`. Preserve `181B4`'s direct-save versus `levels.dat` fallback split, the legacy deferred 10-byte record redistribution from `CB3C` into `CD3C`, and the three compact 11-byte record conversions (including type `54h -> 58h` only in the alternate family). The direct-save path consumes but ignores the serialized primary-count word and still uses header `258A6`; the common tail performs a relative four-byte seek before the map body, uses a signed `<7FFh` packed-text-length gate, and closes the file before post-load state restoration. Preserve the path-dependent live EBX value (`0` direct, `3` legacy) passed to `1C160(20h, EBX)`, exact `BF38` save/restore, and the late `BF44` overrides for levels `17h`, `15h`, `0Bh`, and `0Ch`. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
add_p.write_text(add)
