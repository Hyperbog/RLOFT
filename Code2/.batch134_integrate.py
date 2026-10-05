from pathlib import Path
import re,sys
root=Path(__file__).resolve().parents[1]; cpp=root/'Code2/Code2.cpp'; frag=root/'Code2/.batch134.fragment.cpp'; prog=root/'Code2/PROGRESS.md'; add=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; chk=root/'Code2/RESUME_CHECKPOINT.md'
note='''\n## Batch 134 — formatted-output field parser\n\nSource identity, Batch 133 ownership, and mandatory overlap hashes were reverified before integration. Added **419 translated instruction bytes** across `38867..38A09`; no new non-code bytes are claimed. `385B0..38866` remains deliberately unowned pending a full formatter-core reconstruction.\n\n- `38867..389A6` — width/precision/length parser. Decimal fields wrap at 32 bits; `*` consumes one cdecl dword; negative width uses wrapping negation and sets left-justify; negative `*` precision becomes `FFFFFFFFh`; encoded modifiers `h/l/w/L/N/F` set the exact runtime flag bits.\n- `389A7..38A09` — flag-prefix parser. Preserves `+` over space precedence, independent `#`/left flags, and the fact that `0` changes the pad byte to ASCII zero rather than setting another flag bit.\n\nCumulative accounting: **229,754 translated + 4,672 non-code = 234,426 / 239,698 = 97.8006% (97.80%)**; **5,272 unresolved bytes**. Next independently decoded boundary: `38A0A`.\n'''
def integrate():
 s=cpp.read_text(); f=frag.read_text()
 if 'function_38867' not in s:
  pos=s.rfind('\n}')
  if pos<0: raise SystemExit('no final brace')
  s=s[:pos]+f+s[pos:]
 s=s.replace('kVerifiedTranslatedBytes = 229335u','kVerifiedTranslatedBytes = 229754u')
 m=re.search(r'(static_assert\(kVerifiedTranslatedBytes\s*==\s*)(.*?)(\);)',s,re.S)
 if m and '+ 419u' not in m.group(2): s=s[:m.start(2)]+m.group(2).rstrip()+' + 419u'+s[m.end(2):]
 cpp.write_text(s)
 p=prog.read_text().replace('object%20accounting-97.63%25','object%20accounting-97.80%25').replace('**Verified translated instruction bytes:** **229,335**','**Verified translated instruction bytes:** **229,754**').replace('**Reconciled object bytes:** **234,007 / 239,698 = 97.6258% (97.63%)**','**Reconciled object bytes:** **234,426 / 239,698 = 97.8006% (97.80%)**').replace('**Unresolved object bytes:** **5,691**','**Unresolved object bytes:** **5,272**')
 if '## Batch 134 —' not in p:p+=note
 prog.write_text(p)
 a=add.read_text()
 if 'Batch 134 continuation' not in a:a+='\n### Batch 134 continuation\n`38867..38A09` is reconciled as the formatted-output field parser and flag-prefix parser. Preserve 32-bit decimal wrap, cdecl `*` consumption, wrapping negative width, negative-star precision sentinel, exact modifier bits, `+` precedence over space, and zero-as-pad-byte behavior. `385B0..38866` remains unowned; next independent boundary is `38A0A`.\n'
 add.write_text(a)
def verify():
 s=cpp.read_text(); assert 'kVerifiedTranslatedBytes = 229754u' in s; assert 'function_38867' in s and 'function_389a7' in s
 p=prog.read_text(); assert '234,426 / 239,698 = 97.8006% (97.80%)' in p and '38A0A' in p
def checkpoint(sub):
 chk.write_text(f'''# CODE.2 resume checkpoint\n\nDurable handoff anchor; live repository/binary evidence wins.\n\n## Last committed substantive decompilation state\n- Last substantive batch commit: `{sub}` — `Integrate CODE.2 decompilation batch 134`.\n- Verified translated instruction bytes: **229,754**.\n- Proven non-code/data bytes: **4,672**.\n- Reconciled object bytes: **234,426 / 239,698 = 97.8006% (97.80%)**.\n- Unresolved object bytes: **5,272**.\n- Last integrated ownership fold: `38867..38A09` (independent of still-unowned `385B0..38866`).\n- Next independently decoded instruction boundary: **`38A0A`**.\n- The earlier large helper `36CEC..372F5` remains unresolved.\n\n## Immutable identity\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n\n## Mandatory overlap anchors for next resume\n- Window `[0x389CA,0x38A8A)` — 192 bytes; SHA-256 `0ad56774348dad1390f546331c7a16371ba91662b012344328a35866e86e91f6`.\n- Window `[0x389EA,0x38A6A)` — 128 bytes; SHA-256 `e4c9bff32a2b8043992ef8e02a2e467269ffd900ee1405a6e913b6373e050ab2`.\n- Batch-owned `38867..38A09` SHA-256: `17deff407d631e6e05710b7e16a05b03075ad62a72f42620a000a6cfbf24b086`.\n- Batch-owned bytes must not be recounted; overlap bytes are verification only.\n\n## Batch 134 interpretation\n- `38867..389A6`: width/precision/length parser for the formatted-output core.\n- `389A7..38A09`: flag-prefix parser.\n- `385B0..38866` is the formatter core and remains deliberately unowned until reconstructed as a whole.\n- `38A0A` begins the next independent string-length helper.\n''')
if __name__=='__main__':
 if sys.argv[1]=='integrate':integrate()
 elif sys.argv[1]=='verify':verify()
 elif sys.argv[1]=='checkpoint':checkpoint(sys.argv[2])
