from pathlib import Path
root=Path('Code2')
code_p=root/'Code2.cpp'; frag_p=root/'.batch40_fragment.cpp'
ledger_p=root/'RESIDUAL_LEDGER.md'; progress_p=root/'PROGRESS.md'; add_p=root/'CODE2_CONTINUITY_ADDENDUM.md'
code=code_p.read_text(); frag=frag_p.read_text().rstrip()+"\n\n"
assert code.rstrip().endswith('} // namespace code2')
code=code.rsplit('} // namespace code2',1)[0]+frag+'} // namespace code2\n'
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 120353u;','constexpr u32 kVerifiedTranslatedBytes = 121232u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 2311u;','constexpr u32 kVerifiedNonCodeBytes = 2312u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 119524u +\n              (0x1C121u-0x1C0C0u) + (0x1C160u-0x1C124u) +\n              (0x1C19Eu-0x1C160u) + (0x1C1DCu-0x1C1A0u) +\n              (0x1C21Au-0x1C1DCu) + (0x1C258u-0x1C21Cu) +\n              (0x1C2C2u-0x1C258u) + (0x1C327u-0x1C2C4u) +\n              (0x1C366u-0x1C328u) + (0x1C409u-0x1C368u));'''
new='''static_assert(kVerifiedTranslatedBytes == 120353u +\n              (0x1C77Bu-0x1C40Cu));'''
assert old in code
code=code.replace(old,new,1)
code_p.write_text(code)

ledger=ledger_p.read_text(); marker='<!-- RESIDUAL-RANGES-END -->'
assert '1C77B..1C77B' not in ledger and marker in ledger
ledger=ledger.replace(marker,'1C77B..1C77B | alignment | 1\n'+marker,1)
ledger=ledger.replace('**Total proven non-code/data:** **2,311 bytes**.','**Total proven non-code/data:** **2,312 bytes**.',1)
ledger=ledger.rstrip()+'''\n\nBatch 40 evidence: `1C77B` decodes as a single `NOP` between the directly reconstructed `1C40C` return at `1C77A` and the next routine entry at `1C77C`. A full object-1 direct relative control-flow target scan found no incoming call/jump target to `1C77B`. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
ledger_p.write_text(ledger)

progress=progress_p.read_text()
progress=progress.replace('object%20accounting-51.17%25-red','object%20accounting-51.54%25-red',1)
progress=progress.replace('**Verified translated instruction bytes:** **120,353**','**Verified translated instruction bytes:** **121,232**',1)
progress=progress.replace('**Proven non-code/data bytes:** **2,311**','**Proven non-code/data bytes:** **2,312**',1)
progress=progress.replace('**Reconciled object bytes:** **122,664 / 239,698 = 51.1744% (51.17%)**','**Reconciled object bytes:** **123,544 / 239,698 = 51.5415% (51.54%)**',1)
progress=progress.replace('**Unresolved object bytes:** **117,034**','**Unresolved object bytes:** **116,154**',1)
progress=progress.rstrip()+'''\n\n## Batch 40 — small checkpoint: framed/tiled panel renderer\n\nAdded **879 translated instruction bytes** plus **1 directly proven alignment byte**. The sequential frontier now reaches `1C77B`; the next routine begins at `1C77C`.\n\nFidelity notes: `1C40C` preserves the encoded signed minimum-size tests, the low-byte-only `AND AL,E0h/F0h` rounding after addition, signed truncation-toward-zero half-offset arithmetic, random edge/corner tile selection, signed loop comparisons, and the final `2E5D4` page-release call.\n'''
progress_p.write_text(progress)

add=add_p.read_text().rstrip()+'''\n\n## CODE.2 Batch 40 checkpoint\n\nBatch 40 added **879 translated bytes** plus **1 proven non-code/data byte**. Cumulative accounting is **121,232 translated + 2,312 non-code = 123,544 / 239,698 = 51.5415% (51.54%)**; unresolved is **116,154 bytes**.\n\nThe sequential frontier now reaches `1C77B`; the next routine begins at `1C77C`. Preserve `1C40C`'s partial-register size rounding (`AND AL,E0h` / `AND AL,F0h`), x86 signed half-offset calculation, separate random edge families, and signed loop termination. Continue with the requested small checkpoint cadence.\n'''
add_p.write_text(add)
