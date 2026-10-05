from pathlib import Path
import re,sys
root=Path('.')
cpp=root/'Code2/Code2.cpp'; frag=root/'Code2/.batch140.fragment.cpp'
progress=root/'Code2/PROGRESS.md'; residual=root/'Code2/RESIDUAL_LEDGER.md'; add=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; checkpoint=root/'Code2/RESUME_CHECKPOINT.md'

def integrate():
 s=cpp.read_text(); f=frag.read_text()
 if 'function_39838' not in s:
  marker='} // namespace code2'
  if marker not in s: raise SystemExit('namespace sentinel missing')
  s=s.replace(marker,f+'\n'+marker,1)
 s,n=re.subn(r'kVerifiedTranslatedBytes\s*=\s*233331u', 'kVerifiedTranslatedBytes = 233831u',s,count=1)
 if n!=1: raise SystemExit('translated constant sentinel missing')
 s,n=re.subn(r'kVerifiedNonCodeBytes\s*=\s*\d+u', 'kVerifiedNonCodeBytes = 4676u',s,count=1)
 if n!=1: raise SystemExit('non-code constant missing')
 pat=r'(static_assert\(kVerifiedTranslatedBytes == .*?)(\);)'; m=re.search(pat,s,re.S)
 if not m: raise SystemExit('translated static_assert missing')
 expr=m.group(1)
 if not expr.rstrip().endswith('+ 500u'):
  expr=expr.rstrip()+' + 500u'; s=s[:m.start(1)]+expr+s[m.end(1):]
 pat2=r'(static_assert\(kVerifiedNonCodeBytes == .*?)(\);)'; m=re.search(pat2,s,re.S)
 if not m: raise SystemExit('non-code static_assert missing')
 expr=m.group(1)
 if not expr.rstrip().endswith('+ 1u'):
  expr=expr.rstrip()+' + 1u'; s=s[:m.start(1)]+expr+s[m.end(1):]
 cpp.write_text(s)
 block='''\n## Batch 140 — runtime arena allocation/free core\n\nBatch 140 reconciles **500 translated instruction bytes** at `39838..39A2B` plus the proven zero separator byte at `39837`. The fold contains the arena allocation splitter, arena free/coalescing path, and runtime arena-list unlink helper. Cumulative accounting is **233,831 translated + 4,676 non-code/data = 238,507 / 239,698 = 99.5031% (99.50%)**; unresolved is **1,191 bytes**. The next independent boundary is `39A2C`. `385B0..38866` and `36CEC..372F5` remain unresolved.\n'''
 for p in (progress,add):
  t=p.read_text()
  if '## Batch 140 — runtime arena allocation/free core' not in t:p.write_text(t+block)
 t=residual.read_text()
 if '`39837`' not in t: residual.write_text(t+'\n- `39837`: **1 byte**, proven zero separator between command-line parsing and arena allocation code (Batch 140).\n')
 cp=checkpoint.read_text(); hand='''\n\n## Batch 140 handoff\n- Owned fold: `39838..39A2B` (**500 translated bytes**) plus `39837` (**1 proven non-code byte**).\n- Accounting: **233,831 translated + 4,676 non-code/data = 238,507 / 239,698 = 99.5031% (99.50%)**; unresolved **1,191**.\n- Batch-owned code SHA-256: `d47a1b4f97620485574578a3883bd31dd1dd3bbbd10c2010f24f0efdeaa6594c`.\n- Mandatory overlap `[0x399EC,0x39AAC)` SHA-256: `c8e7676423dd051bdc1f2f04843cca4241946102176c137f767b00a5ace53198`.\n- Mandatory overlap `[0x39A0C,0x39A8C)` SHA-256: `76dbf5536b67442f881886f1f68c91cc9be1e2ccb6c54464c471ec40bcab6159`.\n- Next independent boundary: `39A2C`.\n- `385B0..38866` and `36CEC..372F5` remain unresolved.\n'''
 if '## Batch 140 handoff' not in cp:checkpoint.write_text(cp+hand)

def verify():
 s=cpp.read_text()
 for x in ('function_39838','function_398e0','function_399eb','kVerifiedTranslatedBytes = 233831u','kVerifiedNonCodeBytes = 4676u'):
  if x not in s:raise SystemExit('missing '+x)
 if s.count('function_39838(')!=1:raise SystemExit('duplicate batch140')

if __name__=='__main__': (integrate if sys.argv[1]=='integrate' else verify)()
