from pathlib import Path
import re,sys,hashlib
root=Path(__file__).resolve().parents[1]; cpp=root/'Code2/Code2.cpp'; frag=root/'Code2/.batch132.fragment.cpp'; prog=root/'Code2/PROGRESS.md'; led=root/'Code2/RESIDUAL_LEDGER.md'; add=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; chk=root/'Code2/RESUME_CHECKPOINT.md'
old_t,new_t=228141,229073; nc=4672; rec=233745; unr=5953
note='''\n## Batch 132 — DOS read/write/seek/delete and drive wrappers\n\nSource identity and Batch 131 overlap were reverified before integration. Added **932 translated instruction bytes** across `38106..384A9`; no new non-code bytes are claimed.\n\n- `38106..3821E` — descriptor read path: validates descriptor flags, preserves binary/device direct reads, and preserves text-mode CR removal, Ctrl-Z EOF/seek-back, and bit-20h single-read behavior.\n- `3821F..383EC` — descriptor write path: append seek, binary/device direct write, DOS-version-dependent 80h/200h staging, LF->CRLF expansion, short-write error 12h, and DOS error translation.\n- `383ED..38406` — forwarding seek wrapper.\n- `38407..38451` — DOS AH=42h seek with 32-bit DX:AX result and translated failure.\n- `38452..38473` — DOS AH=41h unlink/delete wrapper.\n- `38474..3848B` — current-drive query, converting DOS zero-based AL to one-based 32-bit output.\n- `3848C..384A9` — default-drive setter, converting one-based input to DOS zero-based DL and publishing returned drive count.\n\nCumulative accounting: **229,073 translated + 4,672 non-code = 233,745 / 239,698 = 97.5165% (97.52%)**; **5,953 unresolved bytes**. Next instruction boundary: `384AA`.\n'''
def integrate():
 s=cpp.read_text(); f=frag.read_text();
 if 'function_38106' not in s:
  pos=s.rfind('\n}')
  if pos<0: raise SystemExit('no final brace')
  s=s[:pos]+f+s[pos:]
 s=re.sub(r'kVerifiedTranslatedBytes\s*=\s*228141u', 'kVerifiedTranslatedBytes = 229073u', s)
 # cumulative assertion is intentionally amended by a final +932 term rather than normalized.
 m=re.search(r'(static_assert\(kVerifiedTranslatedBytes\s*==\s*)(.*?)(\);)',s,re.S)
 if m and '+ 932u' not in m.group(2): s=s[:m.start(2)]+m.group(2).rstrip()+' + 932u'+s[m.end(2):]
 cpp.write_text(s)
 p=prog.read_text();p=p.replace('object%20accounting-97.13%25','object%20accounting-97.52%25').replace('**Verified translated instruction bytes:** **228,141**','**Verified translated instruction bytes:** **229,073**').replace('**Reconciled object bytes:** **232,813 / 239,698 = 97.1276% (97.13%)**','**Reconciled object bytes:** **233,745 / 239,698 = 97.5165% (97.52%)**').replace('**Unresolved object bytes:** **6,885**','**Unresolved object bytes:** **5,953**')
 if '## Batch 132 —' not in p:p+=note
 prog.write_text(p)
 a=add.read_text();
 if 'Batch 132' not in a:a+='\n### Batch 132 continuation\n`38106..384A9` is reconciled as the DOS descriptor read/write/seek/delete/current-drive/default-drive family. Preserve text-mode CR stripping, Ctrl-Z rewind/EOF, LF->CRLF expansion, short-write error 12h, descriptor bits 01h/02h/20h/40h/80h, and one-based public drive numbering. Next boundary is `384AA`.\n'
 add.write_text(a)
def verify():
 s=cpp.read_text();assert 'kVerifiedTranslatedBytes = 229073u' in s;assert 'function_38106' in s and 'function_3848c' in s
 p=prog.read_text();assert '233,745 / 239,698 = 97.5165% (97.52%)' in p
 assert '384AA' in p
def checkpoint(sub):
 chk.write_text(f'''# CODE.2 resume checkpoint\n\nDurable handoff anchor; live repository/binary evidence wins.\n\n## Last committed substantive decompilation state\n- Last substantive batch commit: `{sub}` — `Integrate CODE.2 decompilation batch 132`.\n- Verified translated instruction bytes: **229,073**.\n- Proven non-code/data bytes: **4,672**.\n- Reconciled object bytes: **233,745 / 239,698 = 97.5165% (97.52%)**.\n- Unresolved object bytes: **5,953**.\n- Last integrated ownership fold: `38106..384A9`.\n- Next independently decoded instruction boundary: **`384AA`**.\n- The earlier large helper `36CEC..372F5` remains unresolved.\n\n## Immutable identity\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n\n## Batch 132 interpretation\n- `38106..3821E`: descriptor read / text translation.\n- `3821F..383EC`: descriptor write / append and LF-to-CRLF translation.\n- `383ED..38451`: seek wrappers.\n- `38452..38473`: unlink/delete.\n- `38474..384A9`: current/default drive wrappers.\n- `384AA` begins the next independently decoded routine.\n''')
if __name__=='__main__':
 if sys.argv[1]=='integrate':integrate()
 elif sys.argv[1]=='verify':verify()
 elif sys.argv[1]=='checkpoint':checkpoint(sys.argv[2])
