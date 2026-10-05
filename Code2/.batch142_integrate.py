from pathlib import Path
import re,sys
root=Path('.')
cpp=root/'Code2/Code2.cpp'; frag=root/'Code2/.batch142.fragment.cpp'; progress=root/'Code2/PROGRESS.md'; add=root/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; checkpoint=root/'Code2/RESUME_CHECKPOINT.md'
def integrate():
 s=cpp.read_text(); f=frag.read_text()
 if 'function_39c30' not in s:
  marker='} // namespace code2'
  if marker not in s: raise SystemExit('namespace sentinel missing')
  s=s.replace(marker,f+'\n'+marker,1)
 s,n=re.subn(r'kVerifiedTranslatedBytes\s*=\s*234041u','kVerifiedTranslatedBytes = 234326u',s,count=1)
 if n!=1: raise SystemExit('translated constant sentinel missing')
 pat=r'(static_assert\(kVerifiedTranslatedBytes == .*?)(\);)'; m=re.search(pat,s,re.S)
 if not m: raise SystemExit('translated static_assert missing')
 expr=m.group(1)
 if not expr.rstrip().endswith('+ 285u'):
  expr=expr.rstrip()+' + 285u'; s=s[:m.start(1)]+expr+s[m.end(1):]
 cpp.write_text(s)
 block='''\n## Batch 142 — arena ceiling probe and backing allocator\n\nBatch 142 reconciles **285 translated instruction bytes** across `39C30..39D4C`. `39C30..39C7A` probes the first arena end against the cached DS ceiling, refreshing that ceiling from LSL in extender modes 2..8 when required. `39C7B..39D4C` first runs arena cleanup, attempts DPMI 0501h allocation and records its handle/header, then preserves the DOS AH=48h fallback for requests no larger than 64 KiB including the probe allocation/free and selector-to-linear-base conversion. Cumulative accounting is **234,326 translated + 4,676 non-code/data = 239,002 / 239,698 = 99.7096% (99.71%)**; unresolved is **696 bytes**. Next independent boundary is `39D4D`.\n'''
 for p in (progress,add):
  t=p.read_text()
  if '## Batch 142 — arena ceiling probe and backing allocator' not in t:p.write_text(t+block)
 cp=checkpoint.read_text(); hand='''\n\n## Batch 142 handoff\n- Owned fold: `39C30..39D4C` (**285 translated bytes**).\n- Accounting: **234,326 translated + 4,676 non-code/data = 239,002 / 239,698 = 99.7096% (99.71%)**; unresolved **696**.\n- Owned hashes: `39C30..39C7A` = `255f47022b57ed7efae1916e80b54253c908f053cd5c58b95663befefae649ad`; `39C7B..39D4C` = `1a296ab0d7b3d9664b7faf7e57187092cd41e0a21ee5cc92f8be4e0534d5a526`.\n- Mandatory overlap `[0x39D0D,0x39DCD)` SHA-256: `94f4e729bca450c71b5654936d0bd5dbe8092dfe1ef7aa6a34a654069fcec591`.\n- Mandatory overlap `[0x39D0D,0x39D8D)` SHA-256: `d0d1b94f9616a9ccc0e661153c1837a43d2d6e6bc0090474e4f78f89f2d68e5f`.\n- Next independent boundary: `39D4D`.\n- `385B0..38866`, `36CEC..372F5`, `38A30..38A60`, and `39A8A..39BBB` remain deliberately unresolved where not otherwise reconciled by shared ownership.\n'''
 if '## Batch 142 handoff' not in cp:checkpoint.write_text(cp+hand)
def verify():
 s=cpp.read_text()
 for x in ('function_39c30','function_39c7b','kVerifiedTranslatedBytes = 234326u','kVerifiedNonCodeBytes = 4676u'):
  if x not in s:raise SystemExit('missing '+x)
 if s.count('function_39c30(')!=1 or s.count('function_39c7b(')!=1:raise SystemExit('duplicate batch142')
if __name__=='__main__': (integrate if sys.argv[1]=='integrate' else verify)()
