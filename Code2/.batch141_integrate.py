from pathlib import Path
import re,sys
root=Path('.')
cpp=root/'Code2/Code2.cpp'; frag=root/'Code2/.batch141.fragment.cpp'
progress=root/'Code2/PROGRESS.md'; add=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; checkpoint=root/'Code2/RESUME_CHECKPOINT.md'

def integrate():
 s=cpp.read_text(); f=frag.read_text()
 if 'function_39a2c' not in s:
  marker='} // namespace code2'
  if marker not in s: raise SystemExit('namespace sentinel missing')
  s=s.replace(marker,f+'\n'+marker,1)
 s,n=re.subn(r'kVerifiedTranslatedBytes\s*=\s*233831u', 'kVerifiedTranslatedBytes = 234041u',s,count=1)
 if n!=1: raise SystemExit('translated constant sentinel missing')
 pat=r'(static_assert\(kVerifiedTranslatedBytes == .*?)(\);)'; m=re.search(pat,s,re.S)
 if not m: raise SystemExit('translated static_assert missing')
 expr=m.group(1)
 if not expr.rstrip().endswith('+ 210u'):
  expr=expr.rstrip()+' + 210u'; s=s[:m.start(1)]+expr+s[m.end(1):]
 cpp.write_text(s)
 block='''\n## Batch 141 — arena cleanup and sorted arena initialization\n\nBatch 141 reconciles **210 translated instruction bytes** in two independent folds: `39A2C..39A89` and `39BBC..39C2F`. `39A2C` walks the runtime arena list, unlinks exhausted arena records, and releases their DPMI or DOS backing allocation according to the stored ownership field. `39BBC` inserts a new arena record into the address-sorted runtime list and initializes its sentinel/free-block header. The intervening `39A8A..39BBB` resize/grow helper remains deliberately unowned. Cumulative accounting is **234,041 translated + 4,676 non-code/data = 238,717 / 239,698 = 99.5907% (99.59%)**; unresolved is **981 bytes**. Next independent boundary is `39C30`.\n'''
 for p in (progress,add):
  t=p.read_text()
  if '## Batch 141 — arena cleanup and sorted arena initialization' not in t:p.write_text(t+block)
 cp=checkpoint.read_text(); hand='''\n\n## Batch 141 handoff\n- Owned folds: `39A2C..39A89` (**94 translated bytes**) and `39BBC..39C2F` (**116 translated bytes**), total **210 translated bytes**.\n- `39A8A..39BBB` remains deliberately unowned.\n- Accounting: **234,041 translated + 4,676 non-code/data = 238,717 / 239,698 = 99.5907% (99.59%)**; unresolved **981**.\n- Owned hashes: `39A2C..39A89` = `451a5752be1bf4c604227216aa9ebfe58f9fb7d1e165a79b646669d652908cee`; `39BBC..39C2F` = `4912611d6dd6dc6e7dcad4f22ecb29896a60cfc5c7f988e95a6b24dc34339b4e`.\n- Mandatory overlap `[0x39BF0,0x39CB0)` SHA-256: `e4582468c6f6ce35e793943b3c01172300d5d5749873d32a894ba0031c89c5a8`.\n- Mandatory overlap `[0x39C10,0x39C90)` SHA-256: `39e834ab542dfc7cda72bc2512b3d551cc53085ce61da03780c0a57ac7bf62c5`.\n- Next independent boundary: `39C30`.\n- `385B0..38866`, `36CEC..372F5`, and `39A8A..39BBB` remain unresolved.\n'''
 if '## Batch 141 handoff' not in cp:checkpoint.write_text(cp+hand)

def verify():
 s=cpp.read_text()
 for x in ('function_39a2c','function_39bbc','kVerifiedTranslatedBytes = 234041u','kVerifiedNonCodeBytes = 4676u'):
  if x not in s:raise SystemExit('missing '+x)
 if s.count('function_39a2c(')!=1 or s.count('function_39bbc(')!=1:raise SystemExit('duplicate batch141')

if __name__=='__main__': (integrate if sys.argv[1]=='integrate' else verify)()
