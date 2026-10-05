from pathlib import Path
import os,re
R=Path('Code2');C=R/'Code2.cpp';P=R/'PROGRESS.md';A=R/'CODE2_CONTINUITY_ADDENDUM.md';K=R/'RESUME_CHECKPOINT.md';F=R/'.batch112.fragment.cpp'
def norm(s):return s.rstrip()+"\n"
def integrate():
 c=C.read_text();f=F.read_text().rstrip()+"\n"
 assert 'constexpr u32 kVerifiedTranslatedBytes = 220638u;' in c and '// Batch 112:' not in c
 c=c.replace('constexpr u32 kVerifiedTranslatedBytes = 220638u;','constexpr u32 kVerifiedTranslatedBytes = 221173u;',1)
 m=re.search(r'(static_assert\(kVerifiedTranslatedBytes ==[\s\S]*?)(\);)',c);assert m
 c=c[:m.start()]+m.group(1).rstrip()+' + 535u'+m.group(2)+c[m.end():]
 mark='\n} // namespace code2\n';assert c.endswith(mark)
 C.write_text(norm(c[:-len(mark)]+'\n\n'+f+mark))
 p=P.read_text()
 p=re.sub(r'object%20accounting-[0-9.]+%25','object%20accounting-94.12%25',p,count=1)
 p=re.sub(r'\*\*Verified translated instruction bytes:\*\* \*\*[0-9,]+\*\*','**Verified translated instruction bytes:** **221,173**',p,count=1)
 p=re.sub(r'\*\*Proven non-code/data bytes:\*\* \*\*[0-9,]+\*\*','**Proven non-code/data bytes:** **4,438**',p,count=1)
 p=re.sub(r'\*\*Reconciled object bytes:\*\* \*\*[0-9,]+ / 239,698 = [0-9.]+% \([0-9.]+%\)\*\*','**Reconciled object bytes:** **225,611 / 239,698 = 94.1230% (94.12%)**',p,count=1)
 p=re.sub(r'\*\*Unresolved object bytes:\*\* \*\*[0-9,]+\*\*','**Unresolved object bytes:** **14,087**',p,count=1)
 P.write_text(norm(p.rstrip()+'''\n## Batch 112 — DOS/extender startup body\n\nAdded **535 translated instruction bytes** (`35418..3562E`), bringing accounting to **221,173 translated + 4,438 non-code = 225,611 / 239,698 = 94.1230% (94.12%)**. `35418` preserves the aligned startup stack, DOS-version/extender probes, selector-sensitive PSP/environment access, command-tail trimming/copying, `no87=` environment detection, program-path copy, runtime zero-fill, and the exact register-width effects around the `395E2`/`39589` handoffs. Segment reads/writes outside DGROUP are explicit boundary operations rather than flattened guesses. The fold ends after `CALL 39589`; `3562F` remains a distinct shared-entry continuation because `352E9` jumps there independently. Next frontier: **`3562F`**.\n'''))
 a=A.read_text();A.write_text(norm(a.rstrip()+'''\n## CODE.2 Batch 112 checkpoint\n\nBatch 112 translates `35418..3562E`: **535 instruction bytes**, with no new non-code claim. Coverage is **225,611 / 239,698 = 94.1230% (94.12%)**, leaving **14,087 unresolved bytes**. Preserve the startup stack alignment, byte/word register writes, DOS/extender branch distinctions, selector-backed PSP/environment memory, REPZ-SCAS zero-count ZF inheritance from `AND DL,F0`, balanced startup pushes/pops, `no87=` detection, environment/program-path terminators, and the capped mode-1 zero-fill. `3562F` is not owned by this batch: it is both the fall-through after `3562A` and an independent target from `352E9`.\n'''))
def verify():
 c=C.read_text();p=P.read_text();a=A.read_text()
 assert 'constexpr u32 kVerifiedTranslatedBytes = 221173u;' in c and 'constexpr u32 kVerifiedNonCodeBytes = 4438u;' in c
 for x in ['Runtime112Boundary','function_35418','app112_seg8','app112_seg16','app112_seg32','app112_seg_store8','app112_seg_store16','static_assert(0x3562Fu-0x35418u==535u);']:
  assert x in c,x
 assert '225,611 / 239,698 = 94.1230% (94.12%)' in p
 assert 'CODE.2 Batch 112 checkpoint' in a

def checkpoint():
 sha=os.environ['BATCH_SHA']
 K.write_text(norm(f'''# CODE.2 resume checkpoint\n\nDurable handoff anchor; live repository/binary evidence wins.\n\n## Last committed substantive decompilation state\n- Last substantive batch commit: `{sha}` — `Integrate CODE.2 decompilation batch 112`.\n- Verified translated instruction bytes: **221,173**.\n- Proven non-code/data bytes: **4,438**.\n- Reconciled object bytes: **225,611 / 239,698 = 94.1230% (94.12%)**.\n- Unresolved object bytes: **14,087**.\n- Last integrated ownership fold: `35418..3562E`.\n- Next independently decoded instruction boundary: **`3562F`**.\n\n## Immutable identity\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n\n## Mandatory overlap anchor for next resume\n- Window `[0x355D0,0x356B0)` — 224 bytes; SHA-256 `ecd5a645438b040bd0094921dc102f1ee2372b4faf128e4c08e6d6257aaa3cd9`.\n- Narrow `[0x35600,0x35660)` — 96 bytes; SHA-256 `9dbad0bf97988fd2037d161af95bc52432793c9e91a635cb035b7790a5c411ae`.\n- Narrow bytes: `ab88d180e103f3aa3eb80cbd020083c00f24f0a33826010089353c260100b8ff000000e8ba3f000029ede85a3f000058eb235250ba1454030066b8013dcd216689c35a89d6fcac3c0075fb89f129d149b440cd215850b800000000baff000000`.\n\nInterpretation: `35418..3562E` is the translated DOS/extender startup body. `3562F` begins a shared continuation with `POP EAX`; it is reached by fall-through after `CALL 39589` and independently by the earlier `352E9` jump, so no `3562F+` bytes are counted in Batch 112.\n'''))
if __name__=='__main__':
 import sys
 {'integrate':integrate,'verify':verify,'checkpoint':checkpoint}[sys.argv[1]]()
