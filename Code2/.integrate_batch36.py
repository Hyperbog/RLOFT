from pathlib import Path
root=Path('Code2')
code_p=root/'Code2.cpp'; frag_p=root/'.batch36_fragment.cpp'
ledger_p=root/'RESIDUAL_LEDGER.md'; progress_p=root/'PROGRESS.md'; add_p=root/'CODE2_CONTINUITY_ADDENDUM.md'
code=code_p.read_text(); frag=frag_p.read_text().rstrip()+"\n\n"
assert code.rstrip().endswith('} // namespace code2')
code=code.rsplit('} // namespace code2',1)[0]+frag+'} // namespace code2\n'
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 114822u;','constexpr u32 kVerifiedTranslatedBytes = 115725u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 2246u;','constexpr u32 kVerifiedNonCodeBytes = 2259u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 111780u +\n              ((0x1AE30u-0x1A220u)-46u));'''
new='''static_assert(kVerifiedTranslatedBytes == 114822u +\n              (0x1AF85u-0x1AE30u) + (0x1B1C2u-0x1AF90u));'''
assert old in code
code=code.replace(old,new,1)
code_p.write_text(code)

ledger=ledger_p.read_text()
marker='<!-- RESIDUAL-RANGES-END -->'
assert '1AF85..1AF8F' not in ledger and marker in ledger
ledger=ledger.replace(marker,'1AF85..1AF8F | padding | 11\n1B1C2..1B1C3 | alignment | 2\n'+marker,1)
ledger=ledger.replace('**Total proven non-code/data:** **2,246 bytes**.','**Total proven non-code/data:** **2,259 bytes**.',1)
ledger=ledger.rstrip()+'''\n\nBatch 36 evidence: `1AF85..1AF8F` is an eleven-byte zero pad bounded by the directly reconstructed `1AE30` return at `1AF84` and the next function entry at `1AF90`; it has no direct relative control-flow target. `1B1C2..1B1C3` decodes as `MOV EAX,EAX` between the `1AF90` return and the next entry at `1B1C4`, with no incoming direct relative target. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
ledger_p.write_text(ledger)

progress=progress_p.read_text()
progress=progress.replace('object%20accounting-48.84%25-red','object%20accounting-49.22%25-red',1)
progress=progress.replace('**Verified translated instruction bytes:** **114,822**','**Verified translated instruction bytes:** **115,725**',1)
progress=progress.replace('**Proven non-code/data bytes:** **2,246**','**Proven non-code/data bytes:** **2,259**',1)
progress=progress.replace('**Reconciled object bytes:** **117,068 / 239,698 = 48.8398% (48.84%)**','**Reconciled object bytes:** **117,984 / 239,698 = 49.2219% (49.22%)**',1)
progress=progress.replace('**Unresolved object bytes:** **122,630**','**Unresolved object bytes:** **121,714**',1)
progress=progress.rstrip()+'''\n\n## Batch 36 — small checkpoint: input axis + direction priority\n\nAdded **903 translated instruction bytes** plus **13 directly proven non-code/data bytes**. The sequential frontier now reaches `1B1C3`; the next routine begins at `1B1C4`.\n\nFidelity notes: `1AE30` keeps the encoded low-byte button masks, signed 16-bit INT 33h axis results, `C04C` eight-unit repeat offset, signed `[-A0h,+A0h]` clamp, and the tail jump to `1F820`. `1AF90` builds its eight-byte priority map from the signed high halves of two fixed-point deltas, preserves x86-style absolute-value ordering, and returns the caller's third argument unchanged when a requested rank is absent; rank four instead uses `(random*8)>>15`.\n'''
progress_p.write_text(progress)

add=add_p.read_text().rstrip()+'''\n\n## CODE.2 Batch 36 checkpoint\n\nBatch 36 intentionally uses the smaller checkpoint cadence requested by the user. It added **903 translated bytes** plus **13 proven non-code/data bytes**. Cumulative accounting is **115,725 translated + 2,259 non-code = 117,984 / 239,698 = 49.2219% (49.22%)**; unresolved is **121,714 bytes**.\n\nThe sequential frontier now reaches `1B1C3`; the next routine begins at `1B1C4`. Preserve `1AE30`'s low-byte button tests, signed INT 33h axis values, `C04C` repeat-offset behavior and tail jump, plus `1AF90`'s third-argument stack-slot fallback when its priority scan misses. Continue with similarly sized 1–2 KB checkpoints.\n'''
add_p.write_text(add)
