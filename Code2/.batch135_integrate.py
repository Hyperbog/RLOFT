from pathlib import Path
import re,sys
root=Path(__file__).resolve().parents[1]; cpp=root/'Code2/Code2.cpp'; frag=root/'Code2/.batch135.fragment.cpp'; prog=root/'Code2/PROGRESS.md'; add=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; chk=root/'Code2/RESUME_CHECKPOINT.md'
note='''\n## Batch 135 — formatter-adjacent string and numeric helpers\n\nSource identity, Batch 134 ownership, and mandatory overlap hashes were reverified before integration. Added **390 translated instruction bytes** in four independent ranges: `38A0A..38A2F`, `38A61..38AC2`, `38AC3..38BA5`, and `38BA6..38BC0`; no new non-code bytes are claimed. `38A30..38A60` remains unowned because its exits share the formatter tail at `38862`.\n\n- `38A0A..38A2F` — selector-qualified bounded byte-string length.\n- `38A61..38AC2` — base-16 conversion, right alignment, zero fill, and exact terminator placement.\n- `38AC3..38BA5` — legacy signed 16.16 decimal/fraction materializer with default precision and encoded rounding pass.\n- `38BA6..38BC0` — address-distinct three-argument forwarding wrapper through `[12700h]`.\n\nCumulative accounting: **230,144 translated + 4,672 non-code = 234,816 / 239,698 = 97.9633% (97.96%)**; **4,882 unresolved bytes**. Next independent boundary: `38BC1`.\n'''
def integrate():
 s=cpp.read_text(); f=frag.read_text()
 if 'function_38a0a' not in s:
  pos=s.rfind('\n}')
  if pos<0: raise SystemExit('no final brace')
  s=s[:pos]+f+s[pos:]
 s=s.replace('kVerifiedTranslatedBytes = 229754u','kVerifiedTranslatedBytes = 230144u')
 m=re.search(r'(static_assert\(kVerifiedTranslatedBytes\s*==\s*)(.*?)(\);)',s,re.S)
 if m and '+ 390u' not in m.group(2): s=s[:m.start(2)]+m.group(2).rstrip()+' + 390u'+s[m.end(2):]
 cpp.write_text(s)
 p=prog.read_text().replace('object%20accounting-97.80%25','object%20accounting-97.96%25').replace('**Verified translated instruction bytes:** **229,754**','**Verified translated instruction bytes:** **230,144**').replace('**Reconciled object bytes:** **234,426 / 239,698 = 97.8006% (97.80%)**','**Reconciled object bytes:** **234,816 / 239,698 = 97.9633% (97.96%)**').replace('**Unresolved object bytes:** **5,272**','**Unresolved object bytes:** **4,882**')
 if '## Batch 135 —' not in p:p+=note
 prog.write_text(p)
 a=add.read_text()
 if 'Batch 135 continuation' not in a:a+='\n### Batch 135 continuation\nIndependent formatter-adjacent helpers `38A0A..38A2F`, `38A61..38AC2`, `38AC3..38BA5`, and `38BA6..38BC0` are reconciled. Keep `38A30..38A60` unowned because it jumps into shared formatter tail `38862`; the next independent boundary is `38BC1`.\n'
 add.write_text(a)
def verify():
 s=cpp.read_text(); assert 'kVerifiedTranslatedBytes = 230144u' in s; assert 'function_38a0a' in s and 'function_38ba6' in s
 p=prog.read_text(); assert '234,816 / 239,698 = 97.9633% (97.96%)' in p and '4,882' in p
def checkpoint(sub):
 chk.write_text(f'''# CODE.2 resume checkpoint\n\nDurable handoff anchor; live repository/binary evidence wins.\n\n## Last committed substantive decompilation state\n- Last substantive batch commit: `{sub}` — `Integrate CODE.2 decompilation batch 135`.\n- Verified translated instruction bytes: **230,144**.\n- Proven non-code/data bytes: **4,672**.\n- Reconciled object bytes: **234,816 / 239,698 = 97.9633% (97.96%)**.\n- Unresolved object bytes: **4,882**.\n- Batch 135 owned ranges: `38A0A..38A2F`, `38A61..38AC2`, `38AC3..38BA5`, `38BA6..38BC0`.\n- `38A30..38A60` remains unowned because it shares formatter tail `38862`.\n- Next independently decoded instruction boundary: **`38BC1`**.\n- `385B0..38866` and the earlier large helper `36CEC..372F5` remain unresolved.\n\n## Immutable identity\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n\n## Mandatory overlap anchors for next resume\n- Window `[0x38B81,0x38C41)` — 192 bytes; SHA-256 `35863b072d431c3c529680fec9c5f44156db6232df57e1b8d0998885b1f43181`.\n- Window `[0x38BA1,0x38C21)` — 128 bytes; SHA-256 `6009b726c3895066d334c0ea766f4321fc1788113b82d29f04e767819f76b27c`.\n- Span `38A0A..38BC0` (includes deliberately unowned `38A30..38A60`) SHA-256: `1848e3d68a7418be3b0afcd556af42198689d1eb5bf5c6d56c99de3668d73532`.\n- Owned bytes must not be recounted; overlap bytes are verification only.\n\n## Batch 135 interpretation\n- `38A0A..38A2F`: selector-qualified bounded byte-string length.\n- `38A61..38AC2`: zero-filled exact-width hexadecimal materializer.\n- `38AC3..38BA5`: signed 16.16 decimal/fraction materializer and rounding path.\n- `38BA6..38BC0`: indirect three-argument forwarding wrapper through `[12700h]`.\n- `38BC1` begins the next large conversion/dispatch helper.\n''')
if __name__=='__main__':
 if sys.argv[1]=='integrate':integrate()
 elif sys.argv[1]=='verify':verify()
 elif sys.argv[1]=='checkpoint':checkpoint(sys.argv[2])
