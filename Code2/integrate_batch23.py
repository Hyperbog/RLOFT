from pathlib import Path
import re

root=Path('.')
cpp_p=root/'Code2/Code2.cpp'
prog_p=root/'Code2/PROGRESS.md'
led_p=root/'Code2/RESIDUAL_LEDGER.md'
add_p=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
inc=(root/'Code2/batch23.staging.cpp').read_text()

cpp=cpp_p.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 75071u;' in cpp
assert 'constexpr u32 kVerifiedNonCodeBytes = 1417u;' in cpp
assert '// Batch 23:' not in cpp
cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 75071u;','constexpr u32 kVerifiedTranslatedBytes = 78245u;',1)
cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 1417u;','constexpr u32 kVerifiedNonCodeBytes = 1459u;',1)
pat=r'static_assert\(kVerifiedTranslatedBytes == 71409u \+.*?\);\n\n// -----------------------------------------------------------------------------\n// Batch 02:'
m=re.search(pat,cpp,re.S)
assert m, 'batch22 accounting assert not found'
replacement='''static_assert(kVerifiedTranslatedBytes == 75071u +
              (0x110B4u-0x10FACu) + (0x1118Bu-0x110B4u) +
              (0x11268u-0x1118Cu) + (0x112EAu-0x11268u) +
              (0x113A9u-0x112ECu) + (0x113F2u-0x113ACu) +
              (0x1162Eu-0x11404u) + (0x117A0u-0x11630u) +
              (0x11AD8u-0x117A0u) + (0x11C20u-0x11AD8u) +
              (0x11C2Cu-0x11C20u));

// -----------------------------------------------------------------------------
// Batch 02:'''
cpp=cpp[:m.start()]+replacement+cpp[m.end():]
marker='\n} // namespace code2\n'
assert cpp.endswith(marker)
cpp=cpp[:-len(marker)]+'\n'+inc+marker
cpp_p.write_text(cpp)

led=led_p.read_text()
assert '**Total proven non-code/data:** **1,417 bytes**.' in led
insert='''1118B..1118B | alignment | 1
112EA..112EB | alignment | 2
113A9..113AB | alignment | 3
113F2..113F3 | alignment | 2
113F4..11403 | table | 16
1162E..1162F | alignment | 2
11C2C..11C3B | table | 16
'''
needle='<!-- RESIDUAL-RANGES-END -->'
assert needle in led
led=led.replace(needle,insert+needle,1)
led=led.replace('**Total proven non-code/data:** **1,417 bytes**.','**Total proven non-code/data:** **1,459 bytes**.',1)
led += '''\n\nBatch 23 evidence: `1118B` is `NOP`; `112EA..112EB`, `113F2..113F3`, and `1162E..1162F` decode as `MOV EAX,EAX`; `113A9..113AB` decodes as `LEA EAX,[EAX+0]`. A full object-1 direct relative control-flow target scan found no incoming target to any of these separator bytes. `113F4..11403` is the exact four-entry CS table consumed at `1145E`, targeting `11466,11474,11482,11490`. `11C2C..11C3B` is the exact four-entry CS table consumed at `11CD4`, targeting `11CDC,11CF1,11D06,11D1B`. The older zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
led_p.write_text(led)

prog=prog_p.read_text()
repls={
'object%20accounting-31.91%25-red':'object%20accounting-33.25%25-red',
'**Verified translated instruction bytes:** **75,071**':'**Verified translated instruction bytes:** **78,245**',
'**Proven non-code/data bytes:** **1,417**':'**Proven non-code/data bytes:** **1,459**',
'**Reconciled object bytes:** **76,488 / 239,698 = 31.9102% (31.91%)**':'**Reconciled object bytes:** **79,704 / 239,698 = 33.2518% (33.25%)**',
'**Unresolved object bytes:** **163,210**':'**Unresolved object bytes:** **159,994**',
}
for a,b in repls.items():
    assert a in prog,a
    prog=prog.replace(a,b,1)
prog += '''\n\n## Batch 23 — panel lifecycle, inventory redraw, and item placement (`10FAC..11C2B`)

Batch 23 adds **3,174 translated instruction bytes** plus **42 directly proven alignment/table bytes**. Cumulative accounting is **78,245 translated + 1,459 non-code = 79,704 / 239,698 = 33.2518% (33.25%)**; unresolved is **159,994 bytes**.

The sequential code frontier is now `11C3C`; `11C2C..11C3B` is the proven four-entry CS table owned by that next routine. Preserve the four-page `C050` draw/toggle sequences in `10FAC/110B4`, `11404`'s raw shifted variant-byte write, the `EEB0` range copy and stale-offset boundary in `11630`, and `117A0`'s store-before-`0F238` ordering, nested-placement early returns, repeated `50DC` call, and forced zero variant when the old item is `-1`. `11AD8` deliberately toggles only `DI` on the all-panel path and only the low byte on the single-panel path.
'''
prog_p.write_text(prog)

add=add_p.read_text()
add += '''\n\n## CODE.2 Batch 23 checkpoint

Batch 23 added **3,174 translated bytes** plus **42 proven non-code/data bytes**. Cumulative accounting is **78,245 translated + 1,459 non-code = 79,704 / 239,698 = 33.2518% (33.25%)**; unresolved is **159,994 bytes**.

The sequential frontier now reaches `11C2B`; `11C2C..11C3B` is a proven four-entry CS table and the next routine begins at `11C3C`. Preserve `10FAC/110B4`'s four `C050` page cycles, `11404`'s four-way type table and raw shifted variant-byte flag update, `11630`'s exact `EEB0` range mapping plus encoded stale-local boundary for invalid external input, `117A0`'s store-before-`0F238` behavior and double `50DC` call, and `11AD8`'s low-word-vs-low-byte toggle asymmetry. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
add_p.write_text(add)
