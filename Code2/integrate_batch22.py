from pathlib import Path
import base64,zlib,re

root=Path('.')
cpp_p=root/'Code2/Code2.cpp'
prog_p=root/'Code2/PROGRESS.md'
led_p=root/'Code2/RESIDUAL_LEDGER.md'
add_p=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
inc=zlib.decompress(base64.b64decode((root/'Code2/batch22.zb64').read_text().strip())).decode()

cpp=cpp_p.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 71409u;' in cpp
assert 'constexpr u32 kVerifiedNonCodeBytes = 1411u;' in cpp
assert '// Batch 22:' not in cpp
cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 71409u;','constexpr u32 kVerifiedTranslatedBytes = 75071u;',1)
cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 1411u;','constexpr u32 kVerifiedNonCodeBytes = 1417u;',1)
pat=r'static_assert\(kVerifiedTranslatedBytes == 68666u \+.*?\);\n\n// -----------------------------------------------------------------------------\n// Batch 02:'
m=re.search(pat,cpp,re.S)
assert m, 'batch21 accounting assert not found'
replacement='''static_assert(kVerifiedTranslatedBytes == 71409u +\n              (0x10A6Du-0x10158u) +\n              (0x10BBCu-0x10A70u) +\n              (0x10FA9u-0x10BBCu));\n\n// -----------------------------------------------------------------------------\n// Batch 02:'''
cpp=cpp[:m.start()]+replacement+cpp[m.end():]
marker='\n} // namespace code2\n'
assert cpp.endswith(marker)
cpp=cpp[:-len(marker)]+'\n'+inc+marker
cpp_p.write_text(cpp)

led=led_p.read_text()
assert '**Total proven non-code/data:** **1,411 bytes**.' in led
insert='''10A6D..10A6F | alignment | 3\n10FA9..10FAB | alignment | 3\n'''
needle='<!-- RESIDUAL-RANGES-END -->'
assert needle in led
led=led.replace(needle,insert+needle,1)
led=led.replace('**Total proven non-code/data:** **1,411 bytes**.','**Total proven non-code/data:** **1,417 bytes**.',1)
led += '''\n\nBatch 22 evidence: `10A6D..10A6F` and `10FA9..10FAB` both decode as `LEA EAX,[EAX+0]` compiler separators between directly reconstructed routine boundaries. A full object-1 direct relative control-flow target scan found no incoming target to either separator. The older zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
led_p.write_text(led)

prog=prog_p.read_text()
repls={
'object%20accounting-30.38%25-red':'object%20accounting-31.91%25-red',
'**Verified translated instruction bytes:** **71,409**':'**Verified translated instruction bytes:** **75,071**',
'**Proven non-code/data bytes:** **1,411**':'**Proven non-code/data bytes:** **1,417**',
'**Reconciled object bytes:** **72,820 / 239,698 = 30.3799% (30.38%)**':'**Reconciled object bytes:** **76,488 / 239,698 = 31.9102% (31.91%)**',
'**Unresolved object bytes:** **166,878**':'**Unresolved object bytes:** **163,210**',
}
for a,b in repls.items():
    assert a in prog,a
    prog=prog.replace(a,b,1)
prog += '''\n\n## Batch 22 — character-sheet presentation and equipment rendering (`10158..10FA8`)\n\nBatch 22 adds **3,662 translated instruction bytes** plus **6 directly proven alignment bytes**. Cumulative accounting is **75,071 translated + 1,417 non-code = 76,488 / 239,698 = 31.9102% (31.91%)**; unresolved is **163,210 bytes**.\n\nThe sequential code frontier is now `10FAC`. This batch reconstructs the large `10158` character-sheet presentation routine, `10A70`'s mode-dependent equipment grid, and `10BBC`'s side-aware equipped-item renderer. Preserve the paired six-row state lists, packed three-track class/XP/level layout, signed HP ratio and divide-error edge, mode-3 table-derived grid geometry, and the mirrored item sprite flip/unflip sequence.\n'''
prog_p.write_text(prog)

add=add_p.read_text()
add += '''\n\n## CODE.2 Batch 22 checkpoint\n\nBatch 22 added **3,662 translated bytes** plus **6 proven alignment bytes**. Cumulative accounting is **75,071 translated + 1,417 non-code = 76,488 / 239,698 = 31.9102% (31.91%)**; unresolved is **163,210 bytes**.\n\nThe sequential frontier now reaches `10FA8`; `10FA9..10FAB` is proven alignment and the next routine begins at `10FAC`. Preserve `10158`'s paired status lists and three-track class/XP/level display, its HP-ratio divide edge and threshold color selection, `10A70`'s mode-3 table/list geometry, and `10BBC`'s side-dependent equipment coordinates plus exact sprite flip/unflip around mirrored equipment slots. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
add_p.write_text(add)
