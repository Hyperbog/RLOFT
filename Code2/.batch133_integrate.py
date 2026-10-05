from pathlib import Path
import re,sys
root=Path(__file__).resolve().parents[1]; cpp=root/'Code2/Code2.cpp'; frag=root/'Code2/.batch133.fragment.cpp'; prog=root/'Code2/PROGRESS.md'; add=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; chk=root/'Code2/RESUME_CHECKPOINT.md'
note='''\n## Batch 133 — current-directory, ASCII uppercase, and stream-flush wrappers\n\nSource identity, Batch 132 ownership, and mandatory overlap hashes were reverified before integration. Added **262 translated instruction bytes** across `384AA..385AF`; no new non-code bytes are claimed.\n\n- `384AA..3854D` — DOS current-directory builder: AH=47h into a private buffer, prefixes current drive as `X:\\`, optionally allocates exact `strlen+1`, reports runtime errors 1/5, then forwards through `33971` with the encoded capacity.\n- `3854E..3856D` — forwarding wrapper plus ASCII-only lowercase-to-uppercase helper.\n- `3856E..385AF` — all-stream wrapper and masked linked-stream traversal; matching streams are counted and state bit 10h gates the `37D60` flush call.\n\nCumulative accounting: **229,335 translated + 4,672 non-code = 234,007 / 239,698 = 97.6258% (97.63%)**; **5,691 unresolved bytes**. Next instruction boundary: `385B0`.\n'''
def integrate():
 s=cpp.read_text(); f=frag.read_text()
 if 'function_384aa' not in s:
  pos=s.rfind('\n}')
  if pos<0: raise SystemExit('no final brace')
  s=s[:pos]+f+s[pos:]
 s=re.sub(r'kVerifiedTranslatedBytes\s*=\s*229073u','kVerifiedTranslatedBytes = 229335u',s)
 m=re.search(r'(static_assert\(kVerifiedTranslatedBytes\s*==\s*)(.*?)(\);)',s,re.S)
 if m and '+ 262u' not in m.group(2): s=s[:m.start(2)]+m.group(2).rstrip()+' + 262u'+s[m.end(2):]
 cpp.write_text(s)
 p=prog.read_text().replace('object%20accounting-97.52%25','object%20accounting-97.63%25').replace('**Verified translated instruction bytes:** **229,073**','**Verified translated instruction bytes:** **229,335**').replace('**Reconciled object bytes:** **233,745 / 239,698 = 97.5165% (97.52%)**','**Reconciled object bytes:** **234,007 / 239,698 = 97.6258% (97.63%)**').replace('**Unresolved object bytes:** **5,953**','**Unresolved object bytes:** **5,691**')
 if '## Batch 133 —' not in p:p+=note
 prog.write_text(p)
 a=add.read_text()
 if 'Batch 133 continuation' not in a:a+='\n### Batch 133 continuation\n`384AA..385AF` is reconciled as the DOS current-directory builder, ASCII uppercase pair, and masked stream-flush traversal. Preserve exact `X:\\\\` prefix construction, allocation size `strlen+1`, errors 1/5, ASCII-only case fold, linked-list count semantics, and state-bit 10h flush gate. Next boundary is `385B0`.\n'
 add.write_text(a)
def verify():
 s=cpp.read_text(); assert 'kVerifiedTranslatedBytes = 229335u' in s; assert 'function_384aa' in s and 'function_38579' in s
 p=prog.read_text(); assert '234,007 / 239,698 = 97.6258% (97.63%)' in p and '385B0' in p
def checkpoint(sub):
 chk.write_text(f'''# CODE.2 resume checkpoint\n\nDurable handoff anchor; live repository/binary evidence wins.\n\n## Last committed substantive decompilation state\n- Last substantive batch commit: `{sub}` — `Integrate CODE.2 decompilation batch 133`.\n- Verified translated instruction bytes: **229,335**.\n- Proven non-code/data bytes: **4,672**.\n- Reconciled object bytes: **234,007 / 239,698 = 97.6258% (97.63%)**.\n- Unresolved object bytes: **5,691**.\n- Last integrated ownership fold: `384AA..385AF`.\n- Next independently decoded instruction boundary: **`385B0`**.\n- The earlier large helper `36CEC..372F5` remains unresolved.\n\n## Immutable identity\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n\n## Mandatory overlap anchors for next resume\n- Window `[0x38570,0x38630)` — 192 bytes; SHA-256 `5db1769db761188be424725d5e230e139245bbd78e122b43e07586521258ea1d`.\n- Window `[0x38590,0x38610)` — 128 bytes; SHA-256 `a563b3e0df695978fa4b2d4312beb4d7b7a7ef3c43947f82000a0d9919853942`.\n- Batch-owned `384AA..385AF` SHA-256: `2b8cfb9980b8ae9c6ccb071daa6d4f6a9926a8ab0fd336a01eb7a78b8d5881e6`.\n- Batch-owned bytes must not be recounted; overlap bytes are verification only.\n\n## Batch 133 interpretation\n- `384AA..3854D`: DOS current-directory builder with optional exact-size allocation.\n- `3854E..3856D`: forwarding wrapper plus ASCII lowercase-to-uppercase helper.\n- `3856E..385AF`: all-stream wrapper plus masked stream traversal/flush.\n- `385B0` begins the next independently decoded routine, a larger formatted-output family.\n''')
if __name__=='__main__':
 if sys.argv[1]=='integrate':integrate()
 elif sys.argv[1]=='verify':verify()
 elif sys.argv[1]=='checkpoint':checkpoint(sys.argv[2])
