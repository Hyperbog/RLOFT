from pathlib import Path
import base64,zlib,re
root=Path('.')
inc=zlib.decompress(base64.b64decode((root/'Code2/batch25.zb64').read_text().strip())).decode()
cpp_path=root/'Code2/Code2.cpp'
cpp=cpp_path.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 81609u;' in cpp
assert 'constexpr u32 kVerifiedNonCodeBytes = 1499u;' in cpp
assert 'function_12988' not in cpp
cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 81609u;','constexpr u32 kVerifiedTranslatedBytes = 85661u;',1)
cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 1499u;','constexpr u32 kVerifiedNonCodeBytes = 1579u;',1)
old=re.compile(r'static_assert\(kVerifiedTranslatedBytes == 78245u \+\n(?:.*\n){1,12}?\s*\(0x12986u-0x12724u\)\);')
m=old.search(cpp)
assert m, 'batch24 accounting assert not found'
new='''static_assert(kVerifiedTranslatedBytes == 81609u +\n              (0x12C1Du-0x12988u) + (0x12E6Fu-0x12C20u) +\n              (0x1309Fu-0x12E70u) + (0x13118u-0x130A0u) +\n              (0x131CDu-0x13118u) + (0x13245u-0x131DCu) +\n              (0x13266u-0x13248u) + (0x13282u-0x13268u) +\n              (0x1329Au-0x13284u) + (0x132B1u-0x1329Cu) +\n              (0x132DBu-0x132B4u) + (0x1330Cu-0x132DCu) +\n              (0x133CBu-0x1330Cu) + (0x13488u-0x133CCu) +\n              (0x13599u-0x13488u) + (0x13832u-0x135C4u) +\n              (0x139ABu-0x13834u));'''
cpp=cpp[:m.start()]+new+cpp[m.end():]
needle='\n} // namespace code2\n'
assert cpp.endswith(needle)
cpp=cpp[:-len(needle)]+'\n'+inc+needle
cpp_path.write_text(cpp)

prog_path=root/'Code2/PROGRESS.md'
prog=prog_path.read_text()
prog=prog.replace('object%20accounting-34.67%25-red','object%20accounting-36.40%25-red',1)
prog=prog.replace('**Verified translated instruction bytes:** **81,609**','**Verified translated instruction bytes:** **85,661**',1)
prog=prog.replace('**Proven non-code/data bytes:** **1,499**','**Proven non-code/data bytes:** **1,579**',1)
prog=prog.replace('**Reconciled object bytes:** **83,108 / 239,698 = 34.6720% (34.67%)**','**Reconciled object bytes:** **87,240 / 239,698 = 36.3958% (36.40%)**',1)
prog=prog.replace('**Unresolved object bytes:** **156,590**','**Unresolved object bytes:** **152,458**',1)
prog_path.write_text(prog)

ledger_path=root/'Code2/RESIDUAL_LEDGER.md'
ledger=ledger_path.read_text()
assert '**Total proven non-code/data:** **1,499 bytes**.' in ledger
assert '12C1D..12C1F' not in ledger
add='''12C1D..12C1F | alignment | 3
12E6F..12E6F | alignment | 1
1309F..1309F | alignment | 1
131CD..131CF | noncode | 3
131D0..131DA | noncode | 11
131DB..131DB | alignment | 1
13245..13247 | alignment | 3
13266..13267 | alignment | 2
13282..13283 | alignment | 2
1329A..1329B | alignment | 2
132B1..132B3 | alignment | 3
132DB..132DB | alignment | 1
133CB..133CB | alignment | 1
13599..1359B | alignment | 3
1359C..135C3 | table | 40
13832..13833 | alignment | 2
139AB..139AB | alignment | 1
'''
ledger=ledger.replace('<!-- RESIDUAL-RANGES-END -->',add+'<!-- RESIDUAL-RANGES-END -->',1)
ledger=ledger.replace('**Total proven non-code/data:** **1,499 bytes**.','**Total proven non-code/data:** **1,579 bytes**.',1)
ledger += '''\n\nBatch 25 evidence: `131D0..131DA` is the exact NUL-terminated `CINE00.ANM` literal referenced by the next routine at `139B8`; `131CD..131CF` is the three-byte zero pad immediately before that referenced object. `1359C..135C3` is the exact ten-entry CS dword table consumed by the indirect jump at `13625`, targeting `1362D,1364B,13689,1369C,136B6,136DA,13752,137C0,136C0,1379F`. All other Batch 25 residual ranges decode as bounded `NOP`, `LEA EAX,[EAX+0]`, or `MOV EAX,EAX` compiler separators. A full object-1 direct relative control-flow target scan found no incoming target to any Batch 25 residual byte. The older zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
ledger_path.write_text(ledger)

add_path=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
cont=add_path.read_text()
assert '## CODE.2 Batch 25 checkpoint' not in cont
cont += '''\n\n## CODE.2 Batch 25 checkpoint\n\nBatch 25 added **4,052 translated bytes** plus **80 proven non-code/data bytes**. Cumulative accounting is **85,661 translated + 1,579 non-code = 87,240 / 239,698 = 36.3958% (36.40%)**; unresolved is **152,458 bytes**.\n\nThe sequential frontier now reaches `139AA`; `139AB` is proven `NOP` alignment and the next routine begins at `139AC`. Preserve `12988/12C20`'s two-phase modal input waits and page-toggle redraws, `12E70`'s ordered overlapping hit regions and cleanup gating, `13268/13284`'s x86 partial-register return semantics, `1330C/133CC`'s compact stream decode state in `7F88`, `13488`'s unsigned countdown/seek behavior, and `135C4`'s exact ten-way CS packet dispatch. `131D0` is referenced `CINE00.ANM` data, not code. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
add_path.write_text(cont)
