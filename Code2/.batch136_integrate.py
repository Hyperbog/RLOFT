from pathlib import Path
import sys
root=Path(__file__).resolve().parents[1]
cpp=root/'Code2/Code2.cpp'; prog=root/'Code2/PROGRESS.md'; add=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; chk=root/'Code2/RESUME_CHECKPOINT.md'
frag=root/'Code2/.batch136.fragment.cpp'
def integrate():
 s=cpp.read_text(); f=frag.read_text()
 s=s.replace('constexpr u32 kVerifiedTranslatedBytes = 230144u;','constexpr u32 kVerifiedTranslatedBytes = 231371u;')
 s=s.replace('+ 419u + 390u);','+ 419u + 390u + 1227u);')
 marker='\n} // namespace code2\n'
 if 'function_38bc1' not in s: s=s.replace(marker,f+marker)
 cpp.write_text(s)
 p=prog.read_text(); p=p.replace('230,144','231,371').replace('234,816 / 239,698 = 97.9633% (97.96%)','236,043 / 239,698 = 98.4752% (98.48%)').replace('4,882','3,655')
 p += '\n\n## Batch 136\n- `38BC1..3906B`: 1,195 translated bytes — formatted-conversion dispatcher; consumes parsed arguments, handles integer/fixed/string/pointer/character forms, normalizes precision/sign/prefix state, and transfers to the previously reconstructed formatter shared tail.\n- `3906C..3908B`: 32 translated bytes — in-place NUL-terminated uppercase helper; its empty-string exit shares the earlier `38BBF` epilogue.\n- Batch total: 1,227 translated bytes. Reconciled: **236,043 / 239,698 = 98.4752% (98.48%)**; unresolved: **3,655**.\n'
 prog.write_text(p)
 a=add.read_text(); a += '\n\n### Batch 136\n`38BC1..3908B` is now instruction-accounted: the large conversion dispatcher (`38BC1..3906B`) plus the uppercase helper (`3906C..3908B`). The dispatcher deliberately models its final `JMP 38861` as a boundary transfer so the already-owned formatter tail is not duplicated or recounted. The uppercase helper likewise returns through the semantics of the shared `38BBF` epilogue.\n'
 add.write_text(a)
 c=chk.read_text(); c=c.replace('Last substantive batch commit: `02e2fcb08c56d6015e9be44706aa209753aa7e13` — `Integrate CODE.2 decompilation batch 135`.','Last substantive batch commit: `BATCH136_COMMIT` — `Integrate CODE.2 decompilation batch 136`.').replace('**230,144**','**231,371**').replace('**234,816 / 239,698 = 97.9633% (97.96%)**','**236,043 / 239,698 = 98.4752% (98.48%)**').replace('**4,882**','**3,655**')
 c += '\n\n## Batch 136 handoff\n- Owned fold: `38BC1..3908B` (1,227 translated bytes).\n- Next independent boundary: `3908C`.\n- `3908C..39146` begins DOS-error/carry translation helpers and is not counted by Batch 136.\n- `385B0..38866` and `36CEC..372F5` remain unresolved.\n'
 chk.write_text(c)
def verify():
 s=cpp.read_text(); assert 'kVerifiedTranslatedBytes = 231371u' in s; assert '+ 419u + 390u + 1227u);' in s; assert 'function_38bc1' in s; assert 'function_3906c' in s
 assert '236,043 / 239,698 = 98.4752% (98.48%)' in prog.read_text()
if __name__=='__main__': (integrate if sys.argv[1]=='integrate' else verify)()
