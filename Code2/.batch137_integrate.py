from pathlib import Path
import sys
root=Path(__file__).resolve().parents[1]
cpp=root/'Code2/Code2.cpp'; prog=root/'Code2/PROGRESS.md'; add=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; led=root/'Code2/RESIDUAL_LEDGER.md'; chk=root/'Code2/RESUME_CHECKPOINT.md'; frag=root/'Code2/.batch137.fragment.cpp'
def integrate():
 s=cpp.read_text(); f=frag.read_text(); s=s.replace('constexpr u32 kVerifiedTranslatedBytes = 231371u;','constexpr u32 kVerifiedTranslatedBytes = 232645u;')
 # extend accounting assertion by this batch's translated bytes
 old=' + 390u + 1227u);'; new=' + 390u + 1227u + 1274u);'; s=s.replace(old,new)
 marker='\n} // namespace code2\n'; s=s.replace(marker,f+marker) if 'function_3908c' not in s else s; cpp.write_text(s)
 p=prog.read_text(); p += '\n\n## Batch 137\n- `3908C..39146`: 187 translated bytes — carry/error wrappers and DOS-error-to-runtime-errno normalization.\n- `39147..39149`: 3 proven non-code separator bytes (`00 55 06`) between the error helper and interrupt bridge.\n- `3914A..39225`: 220 translated bytes — generic interrupt register-image bridge/save-restore machinery.\n- `39226..39525`: 768 translated bytes — 256 executable `INT imm8; RET` stubs, exactly 3 bytes each.\n- `39526..39588`: 99 translated bytes — bounded ASCII case-insensitive byte-string compare.\n- Batch total: 1,274 translated + 3 non-code = 1,277 reconciled bytes. Reconciled: **237,320 / 239,698 = 99.0079% (99.01%)**; unresolved: **2,378**.\n'; prog.write_text(p)
 a=add.read_text(); a += '\n\n### Batch 137\n`3908C..39588` is reconciled. `39147..39149` is a three-byte separator; `39226..39525` is executable code, not data: a complete 256-entry `INT n; RET` dispatch table addressed as `39226 + vector*3` by the surrounding bridges. The reconstruction keeps interrupt execution behind a register-image boundary while preserving the table ownership and exact byte accounting.\n'; add.write_text(a)
 l=led.read_text(); l += '\n- Batch 137: `39147..39149` — 3-byte proven separator (`00 55 06`) between completed routines.\n'; led.write_text(l)
 c=chk.read_text(); c += '\n\n## Batch 137 handoff\n- Substantive commit: `BATCH137_COMMIT`.\n- Owned fold: `3908C..39588` (1,274 translated + 3 proven non-code bytes).\n- Accounting: 232,645 translated + 4,675 non-code/data = 237,320 / 239,698 = 99.0079% (99.01%); unresolved 2,378.\n- Next independent boundary: `39589`.\n- `385B0..38866` and `36CEC..372F5` remain unresolved.\n'; chk.write_text(c)
def verify():
 s=cpp.read_text(); assert 'kVerifiedTranslatedBytes = 232645u' in s and 'function_39226' in s and '+ 1274u);' in s
 assert '237,320 / 239,698 = 99.0079% (99.01%)' in prog.read_text()
if __name__=='__main__': (integrate if sys.argv[1]=='integrate' else verify)()
