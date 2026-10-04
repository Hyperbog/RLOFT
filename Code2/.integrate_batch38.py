from pathlib import Path

root=Path('Code2')
code_p=root/'Code2.cpp'
frag_p=root/'.batch38_fragment.cpp'
ledger_p=root/'RESIDUAL_LEDGER.md'
progress_p=root/'PROGRESS.md'
add_p=root/'CODE2_CONTINUITY_ADDENDUM.md'

code=code_p.read_text()
frag=frag_p.read_text().rstrip()+"\n\n"
assert code.rstrip().endswith('} // namespace code2')
code=code.rsplit('} // namespace code2',1)[0]+frag+'} // namespace code2\n'
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 118743u;',
                  'constexpr u32 kVerifiedTranslatedBytes = 119524u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 2273u;',
                  'constexpr u32 kVerifiedNonCodeBytes = 2296u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 118294u +\n              (0x1BD99u-0x1BBD8u));'''
new='''static_assert(kVerifiedTranslatedBytes == 118743u +\n              (0x1C0B9u-0x1BDACu));'''
assert old in code
code=code.replace(old,new,1)
code_p.write_text(code)

ledger=ledger_p.read_text()
marker='<!-- RESIDUAL-RANGES-END -->'
assert marker in ledger
assert '1BD9C..1BDAB' not in ledger and '1C0B9..1C0BF' not in ledger
ledger=ledger.replace(marker,
    '1BD9C..1BDAB | table | 16\n'
    '1C0B9..1C0BF | padding | 7\n'+marker,1)
ledger=ledger.replace('**Total proven non-code/data:** **2,273 bytes**.',
                      '**Total proven non-code/data:** **2,296 bytes**.',1)
ledger=ledger.rstrip()+'''\n\nBatch 38 evidence: `1BD9C..1BDAB` is the exact four-entry CS dword table consumed by the indirect jump at `1BE11`, with targets `1BE70,1BE19,1BE4A,1BE6C`. `1C0B9..1C0BF` is seven zero bytes bounded by the `1BDAC` return at `1C0B8` and the next routine entry at `1C0C0`. A full object-1 direct relative control-flow target scan found no incoming target to either residual range. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
ledger_p.write_text(ledger)

progress=progress_p.read_text()
progress=progress.replace('object%20accounting-50.49%25-red','object%20accounting-50.82%25-red',1)
progress=progress.replace('**Verified translated instruction bytes:** **118,743**',
                          '**Verified translated instruction bytes:** **119,524**',1)
progress=progress.replace('**Proven non-code/data bytes:** **2,273**',
                          '**Proven non-code/data bytes:** **2,296**',1)
progress=progress.replace('**Reconciled object bytes:** **121,016 / 239,698 = 50.4869% (50.49%)**',
                          '**Reconciled object bytes:** **121,820 / 239,698 = 50.8223% (50.82%)**',1)
progress=progress.replace('**Unresolved object bytes:** **118,682**',
                          '**Unresolved object bytes:** **117,878**',1)
progress=progress.rstrip()+'''\n\n## Batch 38 — small checkpoint: world/record animation update\n\nAdded **781 translated instruction bytes** plus **23 directly proven non-code/data bytes**. The sequential frontier now reaches `1C0BF`; the next routine begins at `1C0C0`.\n\nFidelity notes: `1BDAC` owns the exact four-entry animation-state table at `1BD9C`; state 1 calls the `0D550` effect path before reloading descriptor timer `+25h`, state 2 uses timer 7 (tripled under record bit 80h), and state 3 clears the state bits before falling into the state-0 movement path. The three record-array passes preserve signed count reloads, in-place compaction with `DEC ESI` retry, the encoded always-equal `CMP EDI,EDI` artifact, and dword divide faults when metadata divisor byte `+5` is zero.\n'''
progress_p.write_text(progress)

add=add_p.read_text().rstrip()+'''\n\n## CODE.2 Batch 38 checkpoint\n\nBatch 38 continues the smaller checkpoint cadence. It added **781 translated bytes** plus **23 proven non-code/data bytes**. Cumulative accounting is **119,524 translated + 2,296 non-code = 121,820 / 239,698 = 50.8223% (50.82%)**; unresolved is **117,878 bytes**.\n\nThe sequential frontier now reaches `1C0BF`; the next routine begins at `1C0C0`. Preserve the `1BD9C` four-entry state table, descriptor timer reload after the `0D550` call, signed live count reloads for all three record arrays, compaction retry via pre-increment `DEC ESI`, the encoded `CMP EDI,EDI` artifact in the type-8 path, and zero-divisor x86 divide faults in the metadata-driven orientation updates. Continue with similarly small checkpoints.\n'''
add_p.write_text(add)
