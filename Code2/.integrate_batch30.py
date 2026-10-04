from pathlib import Path
import re,base64,zlib

root=Path('.')
code_path=root/'Code2/Code2.cpp'
ledger_path=root/'Code2/RESIDUAL_LEDGER.md'
progress_path=root/'Code2/PROGRESS.md'
add_path=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'

code=code_path.read_text()
frag=zlib.decompress(base64.b64decode('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')).decode().rstrip()+'\n\n'
assert 'constexpr u32 kVerifiedTranslatedBytes = 97832u;' in code
assert 'constexpr u32 kVerifiedNonCodeBytes = 1916u;' in code
assert 'function_16a88' not in code
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 97832u;','constexpr u32 kVerifiedTranslatedBytes = 99711u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 1916u;','constexpr u32 kVerifiedNonCodeBytes = 1945u;',1)
start=code.index('static_assert(kVerifiedTranslatedBytes == 94703u +')
end=code.index('));',start)+3
new_assert='''static_assert(kVerifiedTranslatedBytes == 97832u +
              (0x16ADCu-0x16A88u) + (0x16CCDu-0x16ADCu) +
              (0x16D03u-0x16CD0u) + (0x17097u-0x16D04u) +
              (0x171CFu-0x170ACu) + (0x171F9u-0x171D0u));'''
code=code[:start]+new_assert+code[end:]
close='} // namespace code2'
pos=code.rfind(close)
assert pos!=-1
code=code[:pos]+frag+code[pos:]
code_path.write_text(code)

ledger=ledger_path.read_text()
marker='<!-- RESIDUAL-RANGES-END -->'
assert marker in ledger
ranges='''16CCD..16CCF | alignment | 3
16D03..16D03 | alignment | 1
17097..1709F | noncode | 9
170A0..170AA | noncode | 11
170AB..170AB | alignment | 1
171CF..171CF | alignment | 1
171F9..171FB | alignment | 3
'''
ledger=ledger.replace(marker,ranges+marker,1)
ledger=ledger.replace('**Total proven non-code/data:** **1,916 bytes**.','**Total proven non-code/data:** **1,945 bytes**.',1)
ledger += '''\n\nBatch 30 evidence: `16CCD..16CCF`, `16D03`, `170AB`, `171CF`, and `171F9..171FB` are explicit compiler NOP/LEA separators between directly decoded routine boundaries; a full object-1 direct relative control-flow scan found no incoming target to those separator bytes. `170A0..170AA` is the exact `levels.dat\\0` string copied from address `170A0` by the routine beginning at `181B4`; `17097..1709F` is the nine-byte zero pad immediately preceding that referenced string, with no direct code/data reference into the pad. The long save-file writer begins directly at `171FC` and is deliberately left for the next batch. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
ledger_path.write_text(ledger)

progress=progress_path.read_text()
progress=progress.replace('object%20accounting-41.61%25-red','object%20accounting-42.41%25-red',1)
progress=progress.replace('**Verified translated instruction bytes:** **97,832**','**Verified translated instruction bytes:** **99,711**',1)
progress=progress.replace('**Proven non-code/data bytes:** **1,916**','**Proven non-code/data bytes:** **1,945**',1)
progress=progress.replace('**Reconciled object bytes:** **99,748 / 239,698 = 41.6140% (41.61%)**','**Reconciled object bytes:** **101,656 / 239,698 = 42.4100% (42.41%)**',1)
progress=progress.replace('**Unresolved object bytes:** **139,950**','**Unresolved object bytes:** **138,042**',1)
progress_path.write_text(progress)

add=add_path.read_text().rstrip()+'''\n\n\n## CODE.2 Batch 30 checkpoint\n\nBatch 30 added **1,879 translated bytes** plus **29 proven non-code/data bytes**. Cumulative accounting is **99,711 translated + 1,945 non-code = 101,656 / 239,698 = 42.4100% (42.41%)**; unresolved is **138,042 bytes**.\n\nThe sequential frontier now reaches `171FB`; `171F9..171FB` is proven `LEA EAX,[EAX+0]` alignment and the next routine begins at `171FC`. Preserve `16A88`'s five 10-byte actor sub-blocks driven by four signed mapping bytes apiece; `16ADC`'s active-vs-disabled actor reset plus class-1/class-3/class-4 side effects; `16D04`'s handle-preloaded four-byte locals whose two-byte reads retain the handle's upper 16 bits on full-dword stores, including the offset-`10h` value copied forward to `288F0`; and `170AC`'s exact signed-width neighbor-offset table with 16-bit truncation. `170A0..170AA` is the directly referenced `levels.dat` string, preceded by nine proven zero-data bytes. The save-file writer at `171FC` remains intact for Batch 31. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
add_path.write_text(add)
