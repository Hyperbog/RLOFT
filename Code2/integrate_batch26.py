from pathlib import Path
import base64,zlib,re
root=Path('.')
inc=zlib.decompress(base64.b64decode((root/'Code2/batch26.zb64').read_text().strip())).decode()
cpp_path=root/'Code2/Code2.cpp'
cpp=cpp_path.read_text()
assert 'constexpr u32 kVerifiedTranslatedBytes = 85661u;' in cpp
assert 'constexpr u32 kVerifiedNonCodeBytes = 1579u;' in cpp
assert 'function_139ac' not in cpp
cpp=cpp.replace('constexpr u32 kVerifiedTranslatedBytes = 85661u;','constexpr u32 kVerifiedTranslatedBytes = 89498u;',1)
cpp=cpp.replace('constexpr u32 kVerifiedNonCodeBytes = 1579u;','constexpr u32 kVerifiedNonCodeBytes = 1650u;',1)
old=re.compile(r'static_assert\(kVerifiedTranslatedBytes == 81609u \+\n(?:.*\n){1,20}?\s*\(0x139ABu-0x13834u\)\);')
m=old.search(cpp)
assert m, 'batch25 accounting assert not found'
new='''static_assert(kVerifiedTranslatedBytes == 85661u +
              (0x13AD5u-0x139ACu) + (0x13C62u-0x13AD8u) +
              (0x13EF6u-0x13C88u) + (0x13F61u-0x13EF8u) +
              (0x13FD4u-0x13F70u) + (0x14116u-0x13FD4u) +
              (0x141B6u-0x14118u) + (0x14217u-0x141B8u) +
              (0x14309u-0x14218u) + (0x146B8u-0x1430Cu) +
              (0x146D9u-0x146B8u) + (0x148EEu-0x146DCu));'''
cpp=cpp[:m.start()]+new+cpp[m.end():]
needle='\n} // namespace code2\n'
assert cpp.endswith(needle)
cpp=cpp[:-len(needle)]+'\n'+inc+needle
cpp_path.write_text(cpp)

prog_path=root/'Code2/PROGRESS.md'
prog=prog_path.read_text()
prog=prog.replace('object%20accounting-36.40%25-red','object%20accounting-38.03%25-red',1)
prog=prog.replace('**Verified translated instruction bytes:** **85,661**','**Verified translated instruction bytes:** **89,498**',1)
prog=prog.replace('**Proven non-code/data bytes:** **1,579**','**Proven non-code/data bytes:** **1,650**',1)
prog=prog.replace('**Reconciled object bytes:** **87,240 / 239,698 = 36.3958% (36.40%)**','**Reconciled object bytes:** **91,148 / 239,698 = 38.0262% (38.03%)**',1)
prog=prog.replace('**Unresolved object bytes:** **152,458**','**Unresolved object bytes:** **148,550**',1)
prog_path.write_text(prog)

ledger_path=root/'Code2/RESIDUAL_LEDGER.md'
ledger=ledger_path.read_text()
assert '**Total proven non-code/data:** **1,579 bytes**.' in ledger
assert '13AD5..13AD7' not in ledger
add='''13AD5..13AD7 | alignment | 3
13C62..13C63 | alignment | 2
13C64..13C87 | table | 36
13EF6..13EF7 | alignment | 2
13F61..13F6F | noncode | 15
14116..14117 | alignment | 2
141B6..141B7 | alignment | 2
14217..14217 | alignment | 1
14309..1430B | alignment | 3
146D9..146DB | alignment | 3
148EE..148EF | alignment | 2
'''
ledger=ledger.replace('<!-- RESIDUAL-RANGES-END -->',add+'<!-- RESIDUAL-RANGES-END -->',1)
ledger=ledger.replace('**Total proven non-code/data:** **1,579 bytes**.','**Total proven non-code/data:** **1,650 bytes**.',1)
ledger += '''\n\nBatch 26 evidence: `13C64..13C87` is the exact nine-entry CS dword table consumed by the indirect jump at `13D06`, with targets `13D0E,13D1D,13D1D,13D1D,13D1D,13D69,13DB5,13E33,13E17`. `13F61..13F6F` is a fifteen-byte zero pad bounded by the directly reconstructed return at `13F60` and next entry at `13F70`; it has no data reference and no incoming direct relative control-flow target. All other Batch 26 residual ranges decode as bounded `NOP`, `LEA EAX,[EAX+0]`, or `MOV EAX,EAX` compiler separators. A full object-1 direct relative control-flow target scan found no incoming target to any Batch 26 residual byte other than the explicit table reference at `13D06`. The older zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
ledger_path.write_text(ledger)

add_path=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'
cont=add_path.read_text()
assert '## CODE.2 Batch 26 checkpoint' not in cont
cont += '''\n\n## CODE.2 Batch 26 checkpoint\n\nBatch 26 added **3,837 translated bytes** plus **71 proven non-code/data bytes**. Cumulative accounting is **89,498 translated + 1,650 non-code = 91,148 / 239,698 = 38.0262% (38.03%)**; unresolved is **148,550 bytes**.\n\nThe sequential frontier now reaches `148ED`; `148EE..148EF` is proven `MOV EAX,EAX` alignment and the next routine begins at `148F0`. Preserve `139AC`'s signed decimal filename substitution into `CINE00.ANM`, `13AD8`'s mode-`0Ch` stale-stack output boundary, `13C88`'s exact nine-way teardown table and `-2/-1` restore sentinels, `13FD4`'s in-place word-wrap terminator/restore behavior, `14118`'s party-count-dependent `[left|right]` text selection, `1430C`'s first-matching condition scan through terminator `21h`, and `146DC`'s two distinct wait paths plus `7FD4`-dependent preview remap. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
add_path.write_text(cont)
