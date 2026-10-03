from pathlib import Path
import base64,zlib,re

root=Path('.')
cpp_p=root/'Code2/Code2.cpp'
prog_p=root/'Code2/PROGRESS.md'
led_p=root/'Code2/RESIDUAL_LEDGER.md'
add_p=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
inc=zlib.decompress(base64.b64decode((root/'Code2/batch20.zb64').read_text().strip())).decode()

cpp=cpp_p.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 64472u;' in cpp
assert 'constexpr u32 kVerifiedNonCodeBytes = 1212u;' in cpp
assert '// Batch 20:' not in cpp
cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 64472u;','constexpr u32 kVerifiedTranslatedBytes = 68666u;',1)
cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 1212u;','constexpr u32 kVerifiedNonCodeBytes = 1358u;',1)
pat=r'static_assert\(kVerifiedTranslatedBytes == 60401u \+.*?\);\n\n// -----------------------------------------------------------------------------\n// Batch 02:'
m=re.search(pat,cpp,re.S)
assert m, 'batch19 accounting assert not found'
replacement='''static_assert(kVerifiedTranslatedBytes == 64472u +\n              (0x0E5CCu-0x0E578u) + (0x0EA82u-0x0E5F0u) +\n              (0x0EABBu-0x0EA90u) + (0x0EAD8u-0x0EABCu) +\n              (0x0EB3Bu-0x0EAD8u) + (0x0EC19u-0x0EB3Cu) +\n              (0x0EEA5u-0x0EC1Cu) + (0x0EF14u-0x0EEE0u) +\n              (0x0EF32u-0x0EF14u) + (0x0EF6Du-0x0EF34u) +\n              (0x0EF9Du-0x0EF70u) + (0x0F167u-0x0EFA0u) +\n              (0x0F216u-0x0F168u) + (0x0F236u-0x0F218u) +\n              (0x0F271u-0x0F238u) + (0x0F290u-0x0F274u) +\n              (0x0F2B2u-0x0F290u) + (0x0F2D1u-0x0F2B4u) +\n              (0x0F305u-0x0F2D4u) + (0x0F368u-0x0F308u) +\n              (0x0F38Fu-0x0F368u) + (0x0F40Cu-0x0F390u) +\n              (0x0F4ADu-0x0F40Cu) + (0x0F53Du-0x0F4B0u) +\n              (0x0F5F7u-0x0F540u) + (0x0F66Cu-0x0F5F8u));\n\n// -----------------------------------------------------------------------------\n// Batch 02:'''
cpp=cpp[:m.start()]+replacement+cpp[m.end():]
marker='\n} // namespace code2\n'
assert cpp.endswith(marker)
cpp=cpp[:-len(marker)]+'\n'+inc+marker
cpp_p.write_text(cpp)

led=led_p.read_text()
assert '**Total proven non-code/data:** **1,212 bytes**.' in led
insert='''0E5CC..0E5EF | table | 36\n0EA82..0EA8F | noncode | 14\n0EABB..0EABB | alignment | 1\n0EB3B..0EB3B | alignment | 1\n0EC19..0EC1B | alignment | 3\n0EEA5..0EEAF | noncode | 11\n0EEB0..0EEDF | table | 48\n0EF32..0EF33 | alignment | 2\n0EF6D..0EF6F | alignment | 3\n0EF9D..0EF9F | alignment | 3\n0F167..0F167 | alignment | 1\n0F216..0F217 | alignment | 2\n0F236..0F237 | alignment | 2\n0F271..0F273 | alignment | 3\n0F2B2..0F2B3 | alignment | 2\n0F2D1..0F2D3 | alignment | 3\n0F305..0F307 | alignment | 3\n0F38F..0F38F | alignment | 1\n0F4AD..0F4AF | alignment | 3\n0F53D..0F53F | alignment | 3\n0F5F7..0F5F7 | alignment | 1\n'''
needle='<!-- RESIDUAL-RANGES-END -->'
assert needle in led
led=led.replace(needle,insert+needle,1)
led=led.replace('**Total proven non-code/data:** **1,212 bytes**.','**Total proven non-code/data:** **1,358 bytes**.',1)
led += '''\n\nBatch 20 evidence: `0E5CC..0E5EF` is the exact nine-entry CS dword jump table consumed at `0E79B`, with targets `0E7EA,0E845,0E802,0E7A3,0E7AE,0E7CC,0E815,0E7F5,0E83D`. `0EEB0..0EEDF` is the exact twelve-dword data block copied by `REP MOVSD` at `11642`, with values `AD,A7,B3,AE,BD,B4,C1,BE,C3,C2,C9,C4`. `0EA82..0EA8F` and `0EEA5..0EEAF` are zero padding bounded by directly reconstructed return/entry points; neither is referenced as data and a full object-1 direct relative control-flow scan found no incoming target to either range. All remaining Batch 20 ranges are explicit `NOP`, `LEA EAX,[EAX+0]`, or `MOV EAX,EAX` compiler separators with no direct incoming control-flow targets. The older zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
led_p.write_text(led)

prog=prog_p.read_text()
repls={
'object%20accounting-27.40%25-red':'object%20accounting-29.21%25-red',
'**Verified translated instruction bytes:** **64,472**':'**Verified translated instruction bytes:** **68,666**',
'**Proven non-code/data bytes:** **1,212**':'**Proven non-code/data bytes:** **1,358**',
'**Reconciled object bytes:** **65,684 / 239,698 = 27.4028% (27.40%)**':'**Reconciled object bytes:** **70,024 / 239,698 = 29.2134% (29.21%)**',
'**Unresolved object bytes:** **174,014**':'**Unresolved object bytes:** **169,674**',
}
for a,b in repls.items():
    assert a in prog,a
    prog=prog.replace(a,b,1)
prog += '''\n\n## Batch 20 — effect resolver, modal text UI, and nested item helpers (`0E578..0F66B`)\n\nBatch 20 adds **4,194 translated instruction bytes** plus **146 directly proven non-code/data bytes**. Cumulative accounting is **68,666 translated + 1,358 non-code = 70,024 / 239,698 = 29.2134% (29.21%)**; unresolved is **169,674 bytes**.\n\nThe sequential code frontier is now `0F66C`. This batch reconstructs the `0E5F0` effect resolver and its nine-way CS table, the 17-buffer modal text subsystem through `0EC1C`, equipment/stat aggregation at `0EFA0`, bitfield helpers, and recursive nested-item membership/removal/value routines through `0F5F8`. Preserve `0E5F0`'s distinct success code `14h` branches, zero-vs-one sound tail arguments, two-stage duration halving, and `7Fh` amount persistence; preserve `0EC1C`'s byte-vs-dword `C050` accesses and `0F5F8`'s final 16-bit truncation. Next code begins at `0F66C`.\n'''
prog_p.write_text(prog)

add=add_p.read_text()
add += '''\n\n## CODE.2 Batch 20 checkpoint\n\nBatch 20 added **4,194 translated bytes** plus **146 proven non-code/data bytes**. Cumulative accounting is **68,666 translated + 1,358 non-code = 70,024 / 239,698 = 29.2134% (29.21%)**; unresolved is **169,674 bytes**.\n\nThe sequential frontier now reaches `0F66B`; next code begins at `0F66C`. Preserve `0E5F0`'s nine-way `7E..86h` effect dispatch and special `4Ah`/`7Fh` paths, the 17 allocated `50h`-byte text buffers spanning `12BB0..12BF0`, the exact twelve-dword source block at `0EEB0`, `0EFA0`'s equipment/stat accumulation order, and the recursive item-tree semantics in `0F390/0F40C/0F4B0/0F540/0F5F8`. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
add_p.write_text(add)
