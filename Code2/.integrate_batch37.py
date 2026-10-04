from pathlib import Path
root=Path('Code2')
code_p=root/'Code2.cpp'; frag_p=root/'.batch37_fragment.cpp'
ledger_p=root/'RESIDUAL_LEDGER.md'; progress_p=root/'PROGRESS.md'; add_p=root/'CODE2_CONTINUITY_ADDENDUM.md'
code=code_p.read_text(); frag=frag_p.read_text().rstrip()+"\n\n"
assert code.rstrip().endswith('} // namespace code2')
code=code.rsplit('} // namespace code2',1)[0]+frag+'} // namespace code2\n'
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 118294u;','constexpr u32 kVerifiedTranslatedBytes = 118743u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 2270u;','constexpr u32 kVerifiedNonCodeBytes = 2273u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 114822u +\n              (0x1AF85u-0x1AE30u) + (0x1B1C2u-0x1AF90u) +\n              (0x1B27Cu-0x1B1C4u) + (0x1B392u-0x1B27Cu) +\n              (0x1B3F5u-0x1B394u) + (0x1B4F8u-0x1B3F8u) +\n              (0x1B6EBu-0x1B4F8u) + (0x1B75Eu-0x1B6ECu) +\n              (0x1BBD5u-0x1B760u));'''
new='''static_assert(kVerifiedTranslatedBytes == 118294u +\n              (0x1BD99u-0x1BBD8u));'''
assert old in code
code=code.replace(old,new,1)
code_p.write_text(code)

ledger=ledger_p.read_text(); marker='<!-- RESIDUAL-RANGES-END -->'
assert '1BD99..1BD9B' not in ledger and marker in ledger
ledger=ledger.replace(marker,'1BD99..1BD9B | alignment | 3\n'+marker,1)
ledger=ledger.replace('**Total proven non-code/data:** **2,270 bytes**.','**Total proven non-code/data:** **2,273 bytes**.',1)
ledger=ledger.rstrip()+'''\n\nBatch 37 evidence: `1BD99..1BD9B` decodes as `LEA EAX,[EAX+0]` between the reconstructed `1BBD8` return and the next ownership unit's table at `1BD9C`. A full object-1 direct relative control-flow scan found no incoming target to the separator. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
ledger_p.write_text(ledger)

progress=progress_p.read_text()
progress=progress.replace('object%20accounting-50.30%25-red','object%20accounting-50.49%25-red',1)
progress=progress.replace('**Verified translated instruction bytes:** **118,294**','**Verified translated instruction bytes:** **118,743**',1)
progress=progress.replace('**Proven non-code/data bytes:** **2,270**','**Proven non-code/data bytes:** **2,273**',1)
progress=progress.replace('**Reconciled object bytes:** **120,564 / 239,698 = 50.2983% (50.30%)**','**Reconciled object bytes:** **121,016 / 239,698 = 50.4869% (50.49%)**',1)
progress=progress.replace('**Unresolved object bytes:** **119,134**','**Unresolved object bytes:** **118,682**',1)
progress=progress.rstrip()+'''\n\n## Batch 37 — small checkpoint: moving-record step/retry wrapper\n\nAdded **449 translated instruction bytes** plus **3 directly proven alignment bytes**. The sequential frontier now reaches `1BD9B`; the next ownership unit begins with the four-entry table at `1BD9C` and its routine at `1BDAC`.\n\nFidelity notes: `1BBD8` preserves the type-9 low-word tile arithmetic, the encoded impossible duplicate `JE` as its effective fall-through semantics, descriptor-driven displacement, low-word fixed-point tile indexing, divide-by-zero fault boundary, type-8 failure state, and exactly three movement retries through `1B760`.\n'''
progress_p.write_text(progress)

add=add_p.read_text().rstrip()+'''\n\n## CODE.2 Batch 37 checkpoint\n\nUsing the smaller checkpoint cadence, Batch 37 added **449 translated bytes** plus **3 proven alignment bytes**. Cumulative accounting is **118,743 translated + 2,273 non-code = 121,016 / 239,698 = 50.4869% (50.49%)**; unresolved is **118,682 bytes**.\n\nThe sequential frontier is `1BD9B`. The next ownership unit begins at the four-entry table `1BD9C..1BDAB`, with code entry `1BDAC`. Preserve the `1BBD8` retry count, type-9 word-width arithmetic, movement descriptor radius/divisor reads, and the success-path orientation increment.\n'''
add_p.write_text(add)
