from pathlib import Path
root=Path('Code2')
code_p=root/'Code2.cpp'; frag_p=root/'.batch41_fragment.cpp'
ledger_p=root/'RESIDUAL_LEDGER.md'; progress_p=root/'PROGRESS.md'; add_p=root/'CODE2_CONTINUITY_ADDENDUM.md'
code=code_p.read_text(); frag=frag_p.read_text().rstrip()+"\n\n"
assert code.rstrip().endswith('} // namespace code2')
code=code.rsplit('} // namespace code2',1)[0]+frag+'} // namespace code2\n'
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 121232u;','constexpr u32 kVerifiedTranslatedBytes = 122860u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 2312u;','constexpr u32 kVerifiedNonCodeBytes = 2456u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 120353u +\n              (0x1C77Bu-0x1C40Cu));'''
new='''static_assert(kVerifiedTranslatedBytes == 121232u +\n              (0x1C924u-0x1C77Cu) + (0x1C99Fu-0x1C924u) +\n              (0x1CD47u-0x1CA2Cu) + (0x1CE66u-0x1CD48u));'''
assert old in code
code=code.replace(old,new,1)
code_p.write_text(code)

ledger=ledger_p.read_text(); marker='<!-- RESIDUAL-RANGES-END -->'
for token in ('1C99F..1C99F','1C9A0..1CA2B','1CD47..1CD47','1CE66..1CE67'):
    assert token not in ledger
addition='''1C99F..1C99F | alignment | 1\n1C9A0..1CA2B | table | 140\n1CD47..1CD47 | alignment | 1\n1CE66..1CE67 | alignment | 2\n'''
ledger=ledger.replace(marker,addition+marker,1)
ledger=ledger.replace('**Total proven non-code/data:** **2,312 bytes**.','**Total proven non-code/data:** **2,456 bytes**.',1)
ledger=ledger.rstrip()+'''\n\nBatch 41 evidence: `1C9A0..1CA2B` is the exact 35-entry CS dword table selected by criterion `00h..22h` and consumed by the indirect jump at `1CA73`; all 35 destinations are reconciled in `function_1ca2c`. `1C99F` and `1CD47` decode as `NOP`, while `1CE66..1CE67` decodes as `MOV EAX,EAX`; a full object-1 direct relative CALL/JMP/Jcc target scan found no incoming target to those separator bytes. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
ledger_p.write_text(ledger)

progress=progress_p.read_text()
progress=progress.replace('object%20accounting-51.54%25-red','object%20accounting-52.28%25-red',1)
progress=progress.replace('**Verified translated instruction bytes:** **121,232**','**Verified translated instruction bytes:** **122,860**',1)
progress=progress.replace('**Proven non-code/data bytes:** **2,312**','**Proven non-code/data bytes:** **2,456**',1)
progress=progress.replace('**Reconciled object bytes:** **123,544 / 239,698 = 51.5415% (51.54%)**','**Reconciled object bytes:** **125,316 / 239,698 = 52.2808% (52.28%)**',1)
progress=progress.replace('**Unresolved object bytes:** **116,154**','**Unresolved object bytes:** **114,382**',1)
progress=progress.rstrip()+'''\n\n## Batch 41 — enlarged checkpoint: actor modal + selector family\n\nAdded **1,628 translated instruction bytes** plus **144 directly proven non-code/data bytes**. The sequential frontier now reaches `1CE67`; the next routine begins at `1CE68`.\n\nFidelity notes: `1C77C` preserves byte-only `C050` page toggles while passing the full dword to page/render callbacks, the actor graphic index combines signed byte `+12`, unsigned byte `+13 * 17h`, global `258E4`, and optional `+2Eh` from flag `0100h`, and the input wait remains release/press/release. `1C924` performs a random-start four-slot search that rejects `0080h` and `F240h`, then falls back to an unbounded `F240h`-only search. `1CA2C` owns the exact 35-entry criterion table and retains signed tests for bytes `+15..+17` only where the binary uses MOVSX. `1CD48` preserves the criterion-21h pushed-text stack reuse when it calls `1C924` and then `1C77C`, and always latches `arg0+80h` when arg0 is not `FFFFFFFFh`.\n'''
progress_p.write_text(progress)

add=add_p.read_text().rstrip()+'''\n\n## CODE.2 Batch 41 checkpoint\n\nBatch 41 increases the checkpoint size while retaining clean ownership boundaries. It added **1,628 translated bytes** plus **144 proven non-code/data bytes**. Cumulative accounting is **122,860 translated + 2,456 non-code = 125,316 / 239,698 = 52.2808% (52.28%)**; unresolved is **114,382 bytes**.\n\nThe sequential frontier now reaches `1CE67`; the next routine begins at `1CE68`, a much larger command dispatcher and should remain intact for the next ownership pass. Preserve `1C77C`'s full-dword versus low-byte `C050` asymmetry, `1C924`'s two-phase slot search, the exact 35-entry table at `1C9A0..1CA2B`, and `1CD48`'s criterion-21h stack reuse/latch behavior.\n'''
add_p.write_text(add)
