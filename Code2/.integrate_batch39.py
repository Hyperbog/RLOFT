from pathlib import Path
root=Path('Code2')
code_p=root/'Code2.cpp'; frag_p=root/'.batch39_fragment.cpp'
ledger_p=root/'RESIDUAL_LEDGER.md'; progress_p=root/'PROGRESS.md'; add_p=root/'CODE2_CONTINUITY_ADDENDUM.md'
code=code_p.read_text(); frag=frag_p.read_text().rstrip()+"\n\n"
assert code.rstrip().endswith('} // namespace code2')
code=code.rsplit('} // namespace code2',1)[0]+frag+'} // namespace code2\n'
code=code.replace('constexpr u32 kVerifiedTranslatedBytes = 119524u;','constexpr u32 kVerifiedTranslatedBytes = 120353u;',1)
code=code.replace('constexpr u32 kVerifiedNonCodeBytes = 2296u;','constexpr u32 kVerifiedNonCodeBytes = 2311u;',1)
old='''static_assert(kVerifiedTranslatedBytes == 118743u +\n              (0x1C0B9u-0x1BDACu));'''
new='''static_assert(kVerifiedTranslatedBytes == 119524u +\n              (0x1C121u-0x1C0C0u) + (0x1C160u-0x1C124u) +\n              (0x1C19Eu-0x1C160u) + (0x1C1DCu-0x1C1A0u) +\n              (0x1C21Au-0x1C1DCu) + (0x1C258u-0x1C21Cu) +\n              (0x1C2C2u-0x1C258u) + (0x1C327u-0x1C2C4u) +\n              (0x1C366u-0x1C328u) + (0x1C409u-0x1C368u));'''
assert old in code
code=code.replace(old,new,1)
code_p.write_text(code)

ledger=ledger_p.read_text(); marker='<!-- RESIDUAL-RANGES-END -->'
ranges='''1C121..1C123 | alignment | 3
1C19E..1C19F | alignment | 2
1C21A..1C21B | alignment | 2
1C2C2..1C2C3 | alignment | 2
1C327..1C327 | alignment | 1
1C366..1C367 | alignment | 2
1C409..1C40B | alignment | 3
'''
assert '1C121..1C123' not in ledger and marker in ledger
ledger=ledger.replace(marker,ranges+marker,1)
ledger=ledger.replace('**Total proven non-code/data:** **2,296 bytes**.','**Total proven non-code/data:** **2,311 bytes**.',1)
ledger=ledger.rstrip()+'''\n\nBatch 39 evidence: `1C121..1C123`, `1C19E..1C19F`, `1C21A..1C21B`, `1C2C2..1C2C3`, `1C327`, `1C366..1C367`, and `1C409..1C40B` decode as explicit NOP/LEA/MOV-EAX,EAX compiler separators between directly reconstructed routines. A full object-1 direct relative control-flow target scan found no incoming CALL/JMP/Jcc target to any of these bytes. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.\n'''
ledger_p.write_text(ledger)

progress=progress_p.read_text()
progress=progress.replace('object%20accounting-50.82%25-red','object%20accounting-51.17%25-red',1)
progress=progress.replace('**Verified translated instruction bytes:** **119,524**','**Verified translated instruction bytes:** **120,353**',1)
progress=progress.replace('**Proven non-code/data bytes:** **2,296**','**Proven non-code/data bytes:** **2,311**',1)
progress=progress.replace('**Reconciled object bytes:** **121,820 / 239,698 = 50.8223% (50.82%)**','**Reconciled object bytes:** **122,664 / 239,698 = 51.1744% (51.17%)**',1)
progress=progress.replace('**Unresolved object bytes:** **117,878**','**Unresolved object bytes:** **117,034**',1)
progress=progress.rstrip()+'''\n\n## Batch 39 — small checkpoint: bitset/text/dice/file helpers\n\nAdded **829 translated instruction bytes** plus **15 directly proven alignment bytes**. The sequential frontier now reaches `1C40B`; the next routine begins at `1C40C`.\n\nFidelity notes: `1C0C0` keeps both signed IDIV stages before the 90-degree scale; `1C124/1C160/1C1A0/1C1DC` preserve signed divide-by-32 remainder shift counts, including `1C1DC` returning `FFFFFFFFh` when selected bit 31 is arithmetically shifted back; `1C2C4` uses the encoded unsigned deadline comparison; `1C328` keeps low-32 IMUL/logical-shift dice scaling and wrapped additions; `1C368` reloads handle `22224` for each seek/read and XOR-decodes the payload backward with a 16-bit decrement loop.\n'''
progress_p.write_text(progress)

add=add_p.read_text().rstrip()+'''\n\n## CODE.2 Batch 39 checkpoint\n\nBatch 39 continues the smaller checkpoint cadence. It added **829 translated bytes** plus **15 proven non-code/data bytes**. Cumulative accounting is **120,353 translated + 2,311 non-code = 122,664 / 239,698 = 51.1744% (51.17%)**; unresolved is **117,034 bytes**.\n\nThe sequential frontier now reaches `1C40B`; the next routine begins at `1C40C`. Preserve `1C0C0`'s signed two-stage division, the bitset helpers' negative-index/remainder shift behavior and bit-31 SAR result, the unsigned timeout test in `1C2C4`, `1C328`'s low-32 multiply/wrap arithmetic, and `1C368`'s repeated handle reloads plus backward `DCh` decode. Continue with similarly small checkpoints.\n'''
add_p.write_text(add)
