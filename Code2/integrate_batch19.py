from pathlib import Path
import base64,zlib,re

root=Path('.')
cpp_p=root/'Code2/Code2.cpp'
prog_p=root/'Code2/PROGRESS.md'
led_p=root/'Code2/RESIDUAL_LEDGER.md'
add_p=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
inc=zlib.decompress(base64.b64decode((root/'Code2/batch19.zb64').read_text().strip())).decode()

cpp=cpp_p.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 60401u;' in cpp
assert 'constexpr u32 kVerifiedNonCodeBytes = 1147u;' in cpp
assert '// Batch 19:' not in cpp
cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 60401u;','constexpr u32 kVerifiedTranslatedBytes = 64472u;',1)
cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 1147u;','constexpr u32 kVerifiedNonCodeBytes = 1212u;',1)
pat=r'static_assert\(kVerifiedTranslatedBytes == 56834u \+.*?\);\n\n// -----------------------------------------------------------------------------\n// Batch 02:'
m=re.search(pat,cpp,re.S)
assert m, 'batch18 accounting assert not found'
replacement='''static_assert(kVerifiedTranslatedBytes == 60401u +\n              (0x0DACCu-0x0D550u) + (0x0DB94u-0x0DACCu) +\n              (0x0DBD3u-0x0DB94u) + (0x0DC64u-0x0DBD4u) +\n              (0x0DDA1u-0x0DC7Cu) + (0x0E008u-0x0DDA4u) +\n              (0x0E0BCu-0x0E028u) + (0x0E0F5u-0x0E0BCu) +\n              (0x0E282u-0x0E0F8u) + (0x0E578u-0x0E284u));\n\n// -----------------------------------------------------------------------------\n// Batch 02:'''
cpp=cpp[:m.start()]+replacement+cpp[m.end():]
marker='\n} // namespace code2\n'
assert cpp.endswith(marker)
cpp=cpp[:-len(marker)]+'\n'+inc+marker
cpp_p.write_text(cpp)

led=led_p.read_text()
assert '**Total proven non-code/data:** **1,147 bytes**.' in led
insert='''DBD3..DBD3 | alignment | 1\nDC64..DC7B | table | 24\nDDA1..DDA3 | alignment | 3\nE008..E027 | table | 32\nE0F5..E0F7 | alignment | 3\nE282..E283 | alignment | 2\n'''
needle='<!-- RESIDUAL-RANGES-END -->'
assert needle in led
led=led.replace(needle,insert+needle,1)
led=led.replace('**Total proven non-code/data:** **1,147 bytes**.','**Total proven non-code/data:** **1,212 bytes**.',1)
led += '''\n\nBatch 19 evidence: `DBD3` is a one-byte `NOP`; `DDA1..DDA3` and `E0F5..E0F7` decode as `LEA EAX,[EAX+0]`; `E282..E283` decodes as `MOV EAX,EAX`. A full object-1 direct relative control-flow target scan found no incoming target to any of these separator bytes. `DC64..DC7B` is the exact six-entry CS table consumed at `DCFE`, with targets `DD06, DD06, DD06, DD27, DD17, DD32`. `E008..E027` is the exact eight-entry CS table consumed at `E083`, with targets `E08A, E08E, E090, E094, E096, E09A, E0A0, E09E`. The older zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
led_p.write_text(led)

prog=prog_p.read_text()
repls={
'object%20accounting-25.68%25-red':'object%20accounting-27.40%25-red',
'**Verified translated instruction bytes:** **60,401**':'**Verified translated instruction bytes:** **64,472**',
'**Proven non-code/data bytes:** **1,147**':'**Proven non-code/data bytes:** **1,212**',
'**Reconciled object bytes:** **61,548 / 239,698 = 25.6773% (25.68%)**':'**Reconciled object bytes:** **65,684 / 239,698 = 27.4028% (27.40%)**',
'**Unresolved object bytes:** **178,150**':'**Unresolved object bytes:** **174,014**',
}
for a,b in repls.items():
    assert a in prog,a
    prog=prog.replace(a,b,1)
prog += '''\n\n## Batch 19 — record/effect resolution and party progression (`0D550..0E577`)\n\nBatch 19 adds **4,071 translated instruction bytes** plus **65 directly proven non-code/data bytes**. Cumulative accounting is **64,472 translated + 1,212 non-code = 65,684 / 239,698 = 27.4028% (27.40%)**; unresolved is **174,014 bytes**.\n\nThe sequential code frontier is now `0E578`. This batch reconstructs `0D550`'s large record/effect dispatcher with its shared late tails, the `0DACC` selection scan and fallback wrapper, per-track progression/stat helpers through `0DDA4`, directional map-cell clearing at `0E028`, random world-record placement at `0E0F8`, and the five-argument record damage/death path at `0E284`. Preserve `0DDA4`'s distinct per-track experience/class/level strides, the encoded `50DC(0,slot)` argument order, `0D550`'s selected-target damage suppression rule, and `0E284`'s type-specific early-return versus common cleanup/sound/XP paths.\n'''
prog_p.write_text(prog)

add=add_p.read_text()
add += '''\n\n## CODE.2 Batch 19 checkpoint\n\nBatch 19 added **4,071 translated bytes** plus **65 proven non-code/data bytes**. Cumulative accounting is **64,472 translated + 1,212 non-code = 65,684 / 239,698 = 27.4028% (27.40%)**; unresolved is **174,014 bytes**.\n\nThe sequential frontier now reaches `0E577`; next code begins at `0E578`. Preserve `0D550`'s shared error/sound tails and its three-argument entry contract, `0DDA4`'s separate track-indexed experience/class/level fields, the six-entry `DC64` and eight-entry `E008` CS tables, `0E0F8`'s three-attempt placement rule, and `0E284`'s five-argument stack contract plus its type-18 early revival return. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
add_p.write_text(add)
