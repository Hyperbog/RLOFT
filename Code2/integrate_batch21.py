from pathlib import Path
import base64,zlib,re

root=Path('.')
cpp_p=root/'Code2/Code2.cpp'
prog_p=root/'Code2/PROGRESS.md'
led_p=root/'Code2/RESIDUAL_LEDGER.md'
add_p=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
inc=zlib.decompress(base64.b64decode((root/'Code2/batch21.zb64').read_text().strip())).decode()

cpp=cpp_p.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 68666u;' in cpp
assert 'constexpr u32 kVerifiedNonCodeBytes = 1358u;' in cpp
assert '// Batch 21:' not in cpp
cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 68666u;','constexpr u32 kVerifiedTranslatedBytes = 71409u;',1)
cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 1358u;','constexpr u32 kVerifiedNonCodeBytes = 1411u;',1)
pat=r'static_assert\(kVerifiedTranslatedBytes == 64472u \+.*?\);\n\n// -----------------------------------------------------------------------------\n// Batch 02:'
m=re.search(pat,cpp,re.S)
assert m, 'batch20 accounting assert not found'
replacement='''static_assert(kVerifiedTranslatedBytes == 68666u +\n              (0x0F767u-0x0F66Cu) + (0x0FD84u-0x0F790u) +\n              (0x0FDE3u-0x0FD84u) + (0x0FE42u-0x0FDE4u) +\n              (0x0FECFu-0x0FE44u) + (0x0FF3Bu-0x0FED0u) +\n              (0x100C5u-0x0FF3Cu) + (0x10106u-0x100C8u) +\n              (0x10156u-0x10108u));\n\n// -----------------------------------------------------------------------------\n// Batch 02:'''
cpp=cpp[:m.start()]+replacement+cpp[m.end():]
marker='\n} // namespace code2\n'
assert cpp.endswith(marker)
cpp=cpp[:-len(marker)]+'\n'+inc+marker
cpp_p.write_text(cpp)

led=led_p.read_text()
assert '**Total proven non-code/data:** **1,358 bytes**.' in led
insert='''0F767..0F767 | alignment | 1\n0F768..0F78F | table | 40\n0FDE3..0FDE3 | alignment | 1\n0FE42..0FE43 | alignment | 2\n0FECF..0FECF | alignment | 1\n0FF3B..0FF3B | alignment | 1\n100C5..100C7 | alignment | 3\n10106..10107 | alignment | 2\n10156..10157 | alignment | 2\n'''
needle='<!-- RESIDUAL-RANGES-END -->'
assert needle in led
led=led.replace(needle,insert+needle,1)
led=led.replace('**Total proven non-code/data:** **1,358 bytes**.','**Total proven non-code/data:** **1,411 bytes**.',1)
led += '''\n\nBatch 21 evidence: `0F768..0F78F` is the exact ten-entry CS dword table consumed by the indirect item-type dispatcher in `0F790`; entries 0..2 target `0FA85`, entries 3..6 target `0FB3D`, and entries 7..9 target `0F8D4`. `0F767`, `0FDE3`, `0FE42..0FE43`, `0FECF`, `0FF3B`, `100C5..100C7`, `10106..10107`, and `10156..10157` are bounded compiler separator/alignment bytes between directly reconstructed routines. A full object-1 direct relative control-flow target scan found no incoming call/jump target to any Batch 21 residual byte. The older zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
led_p.write_text(led)

prog=prog_p.read_text()
repls={
'object%20accounting-29.21%25-red':'object%20accounting-30.38%25-red',
'**Verified translated instruction bytes:** **68,666**':'**Verified translated instruction bytes:** **71,409**',
'**Proven non-code/data bytes:** **1,358**':'**Proven non-code/data bytes:** **1,411**',
'**Reconciled object bytes:** **70,024 / 239,698 = 29.2134% (29.21%)**':'**Reconciled object bytes:** **72,820 / 239,698 = 30.3799% (30.38%)**',
'**Unresolved object bytes:** **169,674**':'**Unresolved object bytes:** **166,878**',
}
for a,b in repls.items():
    assert a in prog,a
    prog=prog.replace(a,b,1)
prog += '''\n\n## Batch 21 — item descriptions, burden/rank helpers, character panel (`0F66C..10155`)\n\nBatch 21 adds **2,743 translated instruction bytes** plus **53 directly proven non-code/data bytes**. Cumulative accounting is **71,409 translated + 1,411 non-code = 72,820 / 239,698 = 30.3799% (30.38%)**; unresolved is **166,878 bytes**.\n\nThe sequential code frontier is now `10158`; `10156..10157` is proven alignment. This batch preserves `0F790`'s ten-way item-type table split, detailed-description gates, recursive container weight and odd half-unit suffix, `0FD84/0FE44` inventory burden and rank threshold behavior, the character-panel status/HP presentation in `0FF3C`, and the `100C8/10108` wrappers' `A7..C9` BF44 override plus `C050` page toggle.\n'''
prog_p.write_text(prog)

add=add_p.read_text()
add += '''\n\n## CODE.2 Batch 21 checkpoint\n\nBatch 21 added **2,743 translated bytes** plus **53 proven non-code/data bytes**. Cumulative accounting is **71,409 translated + 1,411 non-code = 72,820 / 239,698 = 30.3799% (30.38%)**; unresolved is **166,878 bytes**.\n\nThe sequential frontier now reaches `10155`; `10156..10157` is proven alignment and the next routine begins at `10158`. Preserve `0F790`'s ten-entry CS item-type dispatch (`2..4`, `5..8`, `9..11` families), recursive type-1 container weight and half-unit suffix behavior, `0FD84/0FE44` burden/rank thresholds, `0FF3C`'s portrait/HP/status presentation, and `100C8/10108`'s `BF44=3Ah` range gate plus `C050` toggle. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
add_p.write_text(add)
