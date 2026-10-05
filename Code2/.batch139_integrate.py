from pathlib import Path
import sys
root=Path(__file__).resolve().parents[1]
cpp=root/'Code2/Code2.cpp'; prog=root/'Code2/PROGRESS.md'; add=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; chk=root/'Code2/RESUME_CHECKPOINT.md'; frag=root/'Code2/.batch139.fragment.cpp'
def integrate():
 s=cpp.read_text(); f=frag.read_text()
 s=s.replace('constexpr u32 kVerifiedTranslatedBytes = 232906u;','constexpr u32 kVerifiedTranslatedBytes = 233331u;')
 if '+ 1274u + 261u + 425u);' not in s: s=s.replace('+ 1274u + 261u);','+ 1274u + 261u + 425u);',1)
 if 'Runtime139Boundary' not in s:
  pos=s.rfind('} // namespace code2'); assert pos>=0; s=s[:pos]+f+'\n'+s[pos:]
 cpp.write_text(s)
 p=prog.read_text().replace('232,906','233,331').replace('237,581 / 239,698 = 99.1168% (99.12%)','238,006 / 239,698 = 99.2941% (99.29%)').replace('2,117','1,692')
 if '## Batch 139' not in p: p += '\n\n## Batch 139\n- `3968E..39751`: 196 translated bytes — command-line duplication/argv publication helper.\n- `39752..39836`: 229 translated bytes — quoted/unquoted command-line tokenizer with in-place compaction.\n- Batch total: 425 translated bytes. `39837` remains unowned. Reconciled: **238,006 / 239,698 = 99.2941% (99.29%)**; unresolved: **1,692**.\n'
 prog.write_text(p)
 a=add.read_text()
 if '### Batch 139' not in a: a += '\n\n### Batch 139\n`3968E..39836` is now instruction-accounted. The startup helper duplicates the command line, allocates the contiguous text/argv block, prefixes argv with the published program-name pointer, and publishes argc/argv. `39752` preserves quoted-token parsing plus the encoded backslash escape distinctions and in-place compaction. `39837` is deliberately left unowned.\n'
 add.write_text(a)
 c=chk.read_text()
 if '## Batch 139 handoff' not in c: c += '\n\n## Batch 139 handoff\n- Last substantive batch commit: `BATCH139_COMMIT` — `Integrate CODE.2 decompilation batch 139`.\n- Verified translated instruction bytes: **233,331**.\n- Proven non-code/data bytes: **4,675**.\n- Reconciled: **238,006 / 239,698 = 99.2941% (99.29%)**.\n- Unresolved: **1,692**.\n- Owned fold: `3968E..39836` (425 translated bytes).\n- Batch-owned SHA-256: `9361cfa6660db35e1f38bafff110a3fe1c64336808c941e5b7d7e2ed2cf458fd`.\n- Mandatory overlap `[0x397F7,0x398B7)` SHA-256: `38dec662da86650cbf9cfc26e2d0f65fb608879a5aedf711fc4c76ecf1557128`.\n- Mandatory overlap `[0x39817,0x39897)` SHA-256: `e99dd5778c0474da4d54ca658eaa8e664f9473eadec59b57408973c629e529a0`.\n- Next independent boundary: `39838`; byte `39837` remains unowned.\n- `385B0..38866` and `36CEC..372F5` remain unresolved.\n'
 chk.write_text(c)
def verify():
 s=cpp.read_text(); assert 'kVerifiedTranslatedBytes = 233331u' in s; assert '+ 1274u + 261u + 425u);' in s; assert 'Runtime139Boundary' in s and 'function_39752' in s
 assert '238,006 / 239,698 = 99.2941% (99.29%)' in prog.read_text()
if __name__=='__main__': (integrate if sys.argv[1]=='integrate' else verify)()
