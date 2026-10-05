from pathlib import Path
import sys
r=Path.cwd(); cpp=r/'Code2/Code2.cpp'; pth=r/'Code2/PROGRESS.md'; led=r/'Code2/RESIDUAL_LEDGER.md'; add=r/'Code2/CODE2_CONTINUITY_ADDENDUM.md'; frag=r/'Code2/.batch127.fragment.cpp'
def once(s,a,b):
    if a not in s: raise SystemExit('missing '+a[:60])
    return s.replace(a,b,1)
def integrate():
    s=cpp.read_text(); s=once(s,'225869u','226302u'); s=once(s,'4650u','4665u')
    s=once(s,'+ 277u + 505u);','+ 277u + 505u + 433u);')
    close='} // namespace code2'; f=frag.read_text(); s=s.rsplit(close,1)[0]+f+'\n'+close+s.rsplit(close,1)[1]; cpp.write_text(s)
    p=pth.read_text(); p=once(p,'object%20accounting-96.17%25-red','object%20accounting-96.36%25-red'); p=once(p,'**Verified translated instruction bytes:** **225,869**','**Verified translated instruction bytes:** **226,302**'); p=once(p,'**Proven non-code/data bytes:** **4,650**','**Proven non-code/data bytes:** **4,665**'); p=once(p,'**Reconciled object bytes:** **230,519 / 239,698 = 96.1706% (96.17%)**','**Reconciled object bytes:** **230,967 / 239,698 = 96.3575% (96.36%)**'); p=once(p,'**Unresolved object bytes:** **9,179**','**Unresolved object bytes:** **8,731**'); p+='''\n\n## Batch 127 — planar permutation and DOS vector wrappers\n\nAdded **433 translated instruction bytes** and **15 proven zero separator bytes** across `37810..379CF`. `37810..37957` is the type-1 planar payload permutation/conversion helper using the scratch permutation table at `10610h`; it preserves the optional header-byte skip, four-way index mapping, in-place strip swaps, and header collapse. `37958..3798C` and `3798D..379C0` are DOS set/get interrupt-vector wrappers with the encoded extender-mode 2..8 split. `379C1..379CF` is a 15-byte zero separator before the next routine at `379D0`. Accounting: **226,302 translated + 4,665 non-code/data = 230,967 / 239,698 = 96.3575% (96.36%)**, leaving **8,731 unresolved bytes**.\n'''; pth.write_text(p)
    led.write_text(led.read_text()+'''\n\n## Batch 127 additions\n- `379C1..379CF` — 15 zero separator bytes between the DOS vector wrapper and the independently decoded routine at `379D0`.\n\nBatch 127 adds **15 proven non-code/alignment bytes**, cumulative **4,665**.\n''')
    add.write_text(add.read_text()+'''\n\n## CODE.2 Batch 127 checkpoint\nBatch 127 reconciles `37810..379CF`: **433 translated + 15 separator bytes**. Coverage is **230,967 / 239,698 = 96.3575% (96.36%)**, with **8,731 unresolved bytes**. Preserve the `37810` scratch-table permutation and the DOS vector wrappers' extender-mode 2..8 split. Next boundary is `379D0`; `36CEC..372F5` remains unresolved.\n''')
def verify():
    s=cpp.read_text()
    for x in ('226302u','4665u','function_37810','function_37958','function_3798d','+ 505u + 433u);'):
        if x not in s: raise SystemExit('verify '+x)
    if '230,967 / 239,698 = 96.3575% (96.36%)' not in pth.read_text(): raise SystemExit('progress')
if __name__=='__main__': {'integrate':integrate,'verify':verify}[sys.argv[1]]()
