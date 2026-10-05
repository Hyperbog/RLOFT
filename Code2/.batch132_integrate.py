from pathlib import Path
import re,sys
root=Path(__file__).resolve().parents[1];cpp=root/'Code2/Code2.cpp';frag=root/'Code2/.batch132.fragment.cpp';prog=root/'Code2/PROGRESS.md';add=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md';chk=root/'Code2/RESUME_CHECKPOINT.md'
note='''\n## Batch 132 — DOS read/write/seek/delete and drive wrappers\n\nSource identity and Batch 131 overlap were reverified before integration. Added **932 translated instruction bytes** across `38106..384A9`; no new non-code bytes are claimed.\n\n- `38106..3821E` — descriptor read path: descriptor validation, direct binary/device reads, text CR removal, Ctrl-Z EOF/seek-back, bit-20h single-read behavior.\n- `3821F..383EC` — descriptor write path: append seek, direct binary/device write, DOS-version-dependent 80h/200h staging, LF->CRLF expansion, short-write error 12h.\n- `383ED..38451` — seek wrappers; `38452..38473` — unlink/delete; `38474..384A9` — current/default drive wrappers.\n\nCumulative accounting: **229,073 translated + 4,672 non-code = 233,745 / 239,698 = 97.5165% (97.52%)**; **5,953 unresolved bytes**. Next instruction boundary: `384AA`.\n'''
def integrate():
 s=cpp.read_text();f=frag.read_text()
 if 'function_38106' not in s:
  pos=s.rfind('\n}');assert pos>=0;s=s[:pos]+f+s[pos:]
 s=re.sub(r'kVerifiedTranslatedBytes\s*=\s*228141u','kVerifiedTranslatedBytes = 229073u',s)
 m=re.search(r'(static_assert\(kVerifiedTranslatedBytes\s*==\s*)(.*?)(\);)',s,re.S)
 if m and '+ 932u' not in m.group(2):s=s[:m.start(2)]+m.group(2).rstrip()+' + 932u'+s[m.end(2):]
 cpp.write_text(s)
 p=prog.read_text().replace('object%20accounting-97.13%25','object%20accounting-97.52%25').replace('**Verified translated instruction bytes:** **228,141**','**Verified translated instruction bytes:** **229,073**').replace('**Reconciled object bytes:** **232,813 / 239,698 = 97.1276% (97.13%)**','**Reconciled object bytes:** **233,745 / 239,698 = 97.5165% (97.52%)**').replace('**Unresolved object bytes:** **6,885**','**Unresolved object bytes:** **5,953**')
 if '## Batch 132 —' not in p:p+=note
 prog.write_text(p)
 a=add.read_text()
 if 'Batch 132 continuation' not in a:a+='\n### Batch 132 continuation\n`38106..384A9` is reconciled as the DOS descriptor read/write/seek/delete/current-drive/default-drive family. Preserve CR stripping, Ctrl-Z rewind/EOF, LF->CRLF expansion, short-write error 12h, descriptor bits 01h/02h/20h/40h/80h, and one-based public drive numbering. Next boundary `384AA`.\n'
 add.write_text(a)
def verify():
 s=cpp.read_text();assert 'kVerifiedTranslatedBytes = 229073u' in s and 'function_38106' in s and 'function_3848c' in s
 assert '233,745 / 239,698 = 97.5165% (97.52%)' in prog.read_text()
def checkpoint(sub):
 chk.write_text(f'''# CODE.2 resume checkpoint\n\nDurable handoff anchor; live repository/binary evidence wins.\n\n## Last committed substantive decompilation state\n- Last substantive batch commit: `{sub}` — `Integrate CODE.2 decompilation batch 132`.\n- Verified translated instruction bytes: **229,073**.\n- Proven non-code/data bytes: **4,672**.\n- Reconciled object bytes: **233,745 / 239,698 = 97.5165% (97.52%)**.\n- Unresolved object bytes: **5,953**.\n- Last integrated ownership fold: `38106..384A9`.\n- Next independently decoded instruction boundary: **`384AA`**.\n- The earlier large helper `36CEC..372F5` remains unresolved.\n\n## Immutable identity\n- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.\n- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.\n\n## Mandatory overlap anchors for next resume\n- Window `[0x3846A,0x3852A)` — 192 bytes; SHA-256 `417d6fbacd24084f2ccbcb551422463d4718f73fac8b417c7224f30c43a47e69`.\n- Window `[0x3848A,0x3850A)` — 128 bytes; SHA-256 `f31e8bfb9dd2aa79ced424606f15a6cd29295b197193b5707ac2c6acc434a28e`.\n- Batch-owned `38106..384A9` SHA-256: `ccf5a4605203508d0499c54a07100d92e9a8b93599fe1a90f908efc246978360`.\n- Batch-owned bytes must not be recounted; overlap bytes are verification only.\n\n## Batch 132 interpretation\n- `38106..3821E`: descriptor read / text translation.\n- `3821F..383EC`: descriptor write / append and LF-to-CRLF translation.\n- `383ED..38451`: seek wrappers.\n- `38452..38473`: unlink/delete.\n- `38474..384A9`: current/default drive wrappers.\n- `384AA` begins the next independently decoded routine.\n''')
if __name__=='__main__':
 if sys.argv[1]=='integrate':integrate()
 elif sys.argv[1]=='verify':verify()
 elif sys.argv[1]=='checkpoint':checkpoint(sys.argv[2])
